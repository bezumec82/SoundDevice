/*
 * TI Common Platform Time Sync
 *
 * Copyright (C) 2012 Richard Cochran <richardcochran@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <linux/err.h>
#include <linux/if.h>
#include <linux/hrtimer.h>
#include <linux/module.h>
#include <linux/net_tstamp.h>
#include <linux/ptp_classify.h>
#include <linux/time.h>
#include <linux/uaccess.h>
#include <linux/workqueue.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <linux/irqreturn.h>
#include <linux/interrupt.h>
#include <linux/of_irq.h>

#include "cpts.h"
#include "ptp_bc.h"

#define CPTS_SKB_TX_WORK_TIMEOUT	1 /* jiffies */
#define LATCH_TMR			0
#define AM57xx_IDK			0
#define DEBUG_MAX			1
#define DEBUG_PPS			1
#define DEBUG_PPS_TMR_IRQ	0
#define DEBUG_PPS_TMR		0
#define DEBUG_OVERFLOW		0
#define DEBUG_INIT			0

/* Obvious things */
#define ENABLE		1
#define DISABLE		0
#define TMR_CNTR_MAX	0xFFFFFFFFUL
#define NS_IN_SEC		1000000000UL

struct cpts_skb_cb_data {
	unsigned long tmo;
};

#define cpts_read32(c, r)	readl_relaxed(&c->reg->r)
#define cpts_write32(c, v, r)	writel_relaxed(v, &c->reg->r)

#define READ_TCRR(odt) __omap_dm_timer_read((odt), OMAP_TIMER_COUNTER_REG, 0)
#define READ_TCLR(odt) __omap_dm_timer_read((odt), OMAP_TIMER_CTRL_REG, 0)
#define READ_TCAP(odt) __omap_dm_timer_read((odt), OMAP_TIMER_CAPTURE_REG, 0)
#define WRITE_TCRR(odt, val) __omap_dm_timer_write((odt), \
				OMAP_TIMER_COUNTER_REG, (val), 0)
#define WRITE_TLDR(odt, val) __omap_dm_timer_write((odt), \
				OMAP_TIMER_LOAD_REG, (val), 0)
#define WRITE_TMAR(odt, val) __omap_dm_timer_write((odt), \
				OMAP_TIMER_MATCH_REG, (val), 0)
#define WRITE_TCLR(odt, val) __omap_dm_timer_write((odt), \
				OMAP_TIMER_CTRL_REG, (val), 0)
#define WRITE_TSICR(odt, val) __omap_dm_timer_write((odt), \
				OMAP_TIMER_IF_CTRL_REG, (val), 0)



/* !!! Base !!! */
#define CPTS_TMR_CLK_RATE			24000000
#define CPTS_TMR_CLK_PERIOD_NS		(NS_IN_SEC/CPTS_TMR_CLK_RATE)
/* !!! */


#define CPTS_PPS_FREQUENCY_HZ			1
#define CPTS_PPS_WIDTH_MS				1
#define CPTS_PPS_WIDTH_TICKS			( CPTS_PPS_WIDTH_MS * (CPTS_TMR_CLK_RATE/1000) ) /* 10ms */
#define CPTS_PPS_PERIOD_TICKS			CPTS_TMR_CLK_RATE/CPTS_PPS_FREQUENCY_HZ

#define CPTS_PPS_PERIOD_NS				(u32)( ( (u64)CPTS_PPS_PERIOD_TICKS * (u64)NS_IN_SEC ) / (u64)CPTS_TMR_CLK_RATE )

#if( CPTS_PPS_WIDTH_TICKS > CPTS_PPS_PERIOD_TICKS )
	#error "Wrong PPS settings"
#endif

/* Not less than 1ms until annual */
#define CPTS_TS_THRESH_NS		(CPTS_PPS_PERIOD_NS - 1*1000*1000) //1ms

#define CPTS_OUT_OF_SYNC_TMR_TICKS		CPTS_PPS_PERIOD_TICKS/2

#define CPTS_TMR_RELOAD_CNT		(TMR_CNTR_MAX - CPTS_PPS_PERIOD_TICKS + 1)
#define CPTS_TMR_CMP_CNT		(CPTS_TMR_RELOAD_CNT + CPTS_PPS_WIDTH_TICKS) //PPS WIDTH

#define CPTS_MAX_TS_ACCSS_T_NS			1000
#define CPTS_MAX_TS_ACCSS_T_TICKS		(CPTS_MAX_TS_ACCSS_T_NS / CPTS_TMR_CLK_PERIOD_NS)

static u32 tmr_reload_cnt = CPTS_TMR_RELOAD_CNT;
static u32 tmr_reload_cnt_prev = CPTS_TMR_RELOAD_CNT;


static int pps_tmr_irq_cnt = 0;

static void cpts_tmr_init(struct cpts *cpts);
static void cpts_tmr_reinit(struct cpts *cpts);
static irqreturn_t cpts_1pps_tmr_interrupt(int irq, void *dev_id);
static void cpts_tmr_poll(struct cpts *cpts);
static void cpts_pps_schedule(struct cpts *cpts);

static int cpts_event_port(struct cpts_event *event)
{
	return (event->high >> PORT_NUMBER_SHIFT) & PORT_NUMBER_MASK;
}

static int cpts_match(struct sk_buff *skb, unsigned int ptp_class,
			  u16 ts_seqid, u8 ts_msgtype);

static int event_expired(struct cpts_event *event)
{
	return time_after(jiffies, event->tmo);
}

static int event_type(struct cpts_event *event)
{
	return (event->high >> EVENT_TYPE_SHIFT) & EVENT_TYPE_MASK;
}

static int cpts_fifo_pop(struct cpts *cpts, u32 *high, u32 *low)
{
	u32 r = cpts_read32(cpts, intstat_raw);

	if (r & TS_PEND_RAW) {
		*high = cpts_read32(cpts, event_high);
		*low  = cpts_read32(cpts, event_low);
		cpts_write32(cpts, EVENT_POP, event_pop);
		return 0;
	}
	return -1;
}

static int cpts_purge_events(struct cpts *cpts)
{
	struct list_head *this, *next;
	struct cpts_event *event;
	int removed = 0;

	list_for_each_safe(this, next, &cpts->events) {
		event = list_entry(this, struct cpts_event, list);
		if (event_expired(event)) {
			list_del_init(&event->list);
			list_add(&event->list, &cpts->pool);
			++removed;
		}
	}

	if (removed)
		pr_debug("cpts: event pool cleaned up %d\n", removed);
	return removed ? 0 : -1;
}

static bool cpts_match_tx_ts(struct cpts *cpts, struct cpts_event *event)
{
	struct sk_buff *skb, *tmp;
	u16 seqid;
	u8 mtype;
	bool found = false;

	mtype = (event->high >> MESSAGE_TYPE_SHIFT) & MESSAGE_TYPE_MASK;
	seqid = (event->high >> SEQUENCE_ID_SHIFT) & SEQUENCE_ID_MASK;

	/* no need to grab txq.lock as access is always done under cpts->lock */
	skb_queue_walk_safe(&cpts->txq, skb, tmp) {
		struct skb_shared_hwtstamps ssh;
		unsigned int class = ptp_classify_raw(skb);
		struct cpts_skb_cb_data *skb_cb =
					(struct cpts_skb_cb_data *)skb->cb;

		if (cpts_match(skb, class, seqid, mtype)) {
			u64 ns = timecounter_cyc2time(&cpts->tc, event->low);

			memset(&ssh, 0, sizeof(ssh));
			ssh.hwtstamp = ns_to_ktime(ns);
			skb_tstamp_tx(skb, &ssh);
			found = true;
			__skb_unlink(skb, &cpts->txq);
			dev_consume_skb_any(skb);
			dev_dbg(cpts->dev, "match tx timestamp mtype %u seqid %04x\n",
				mtype, seqid);
		} else if (time_after(jiffies, skb_cb->tmo)) {
			/* timeout any expired skbs over 1s */
			dev_dbg(cpts->dev,
				"expiring tx timestamp mtype %u seqid %04x\n",
				mtype, seqid);
			__skb_unlink(skb, &cpts->txq);
			dev_consume_skb_any(skb);
		}
	}

	return found;
}

/*
 * Returns zero if matching event type was found.
 */
static int cpts_fifo_read(struct cpts *cpts, int match)
{
	int i, type = -1;
	u32 hi, lo;
	struct cpts_event *event;

	for (i = 0; i < CPTS_FIFO_DEPTH; i++) {
		if (cpts_fifo_pop(cpts, &hi, &lo))
			break;

		if (list_empty(&cpts->pool) && cpts_purge_events(cpts)) {
			pr_err("cpts: event pool empty\n");
			return -1;
		}

		event = list_first_entry(&cpts->pool, struct cpts_event, list);
		event->tmo = jiffies +
				 msecs_to_jiffies(CPTS_EVENT_RX_TX_TIMEOUT);
		event->high = hi;
		event->low = lo;
		type = event_type(event);
		switch (type) {
		case CPTS_EV_HW:
			event->tmo +=
				msecs_to_jiffies(CPTS_EVENT_HWSTAMP_TIMEOUT);
			list_del_init(&event->list);
			list_add_tail(&event->list, &cpts->events);
			break;
		case CPTS_EV_TX:
			if (cpts_match_tx_ts(cpts, event)) {
				/* if the new event matches an existing skb,
				 * then don't queue it
				 */
				break;
			}
		case CPTS_EV_PUSH:
		case CPTS_EV_RX:
			list_del_init(&event->list);
			list_add_tail(&event->list, &cpts->events);
			break;
		case CPTS_EV_ROLL:
		case CPTS_EV_HALF:
			break;
		default:
			pr_err("cpts: unknown event type\n");
			break;
		}
		if (type == match)
			break;
	}
	return type == match ? 0 : -1;
}

static cycle_t cpts_systim_read(const struct cyclecounter *cc)
{
	u64 val = 0;
	struct cpts_event *event;
	struct list_head *this, *next;
	struct cpts *cpts = container_of(cc, struct cpts, cc);

	cpts_write32(cpts, TS_PUSH, ts_push);
	if (cpts_fifo_read(cpts, CPTS_EV_PUSH))
		printk("cpts: unable to obtain a time stamp\n");

	list_for_each_safe(this, next, &cpts->events) {
		event = list_entry(this, struct cpts_event, list);
		if (event_type(event) == CPTS_EV_PUSH) {
			list_del_init(&event->list);
			list_add(&event->list, &cpts->pool);
			val = event->low;
			break;
		}
	}

	return val;
}

/* PTP clock operations */

static int cpts_ptp_adjfreq(struct ptp_clock_info *ptp, s32 ppb)
{
	u64 adj;
	u32 diff, mult;
	int neg_adj = 0;
	unsigned long flags;
	struct cpts *cpts = container_of(ptp, struct cpts, info);

	if (ppb < 0) {
		neg_adj = 1;
		ppb = -ppb;
	}
	mult = cpts->cc_mult;
	adj = mult;
	adj *= ppb;
	diff = div_u64(adj, 1000000000ULL);

	spin_lock_irqsave(&cpts->lock, flags);

	timecounter_read(&cpts->tc);

	cpts->cc.mult = neg_adj ? mult - diff : mult + diff;

	spin_unlock_irqrestore(&cpts->lock, flags);
#if(0)
	tmr_reload_cnt = neg_adj ? 
		CPTS_TMR_RELOAD_CNT - (ppb + 0) / (CPTS_TMR_CLK_PERIOD_NS*10) :
		CPTS_TMR_RELOAD_CNT + (ppb + 0) / (CPTS_TMR_CLK_PERIOD_NS*10);
#else
	tmr_reload_cnt = neg_adj ? 
		CPTS_TMR_RELOAD_CNT - ppb / (CPTS_TMR_CLK_PERIOD_NS * CPTS_PPS_FREQUENCY_HZ) :
		CPTS_TMR_RELOAD_CNT + ppb / (CPTS_TMR_CLK_PERIOD_NS * CPTS_PPS_FREQUENCY_HZ);
#endif

	return 0;
}

static int cpts_ptp_adjtime(struct ptp_clock_info *ptp, s64 delta)
{
	unsigned long flags;
	struct cpts *cpts = container_of(ptp, struct cpts, info);

	spin_lock_irqsave(&cpts->lock, flags);
	timecounter_adjtime(&cpts->tc, delta);
	spin_unlock_irqrestore(&cpts->lock, flags);

	return 0;
}

static int cpts_ptp_gettime(struct ptp_clock_info *ptp, struct timespec64 *ts)
{
	u64 ns;
	unsigned long flags;
	struct cpts *cpts = container_of(ptp, struct cpts, info);

	spin_lock_irqsave(&cpts->lock, flags);
	ns = timecounter_read(&cpts->tc);
	spin_unlock_irqrestore(&cpts->lock, flags);

	*ts = ns_to_timespec64(ns);

	return 0;
}

static int cpts_ptp_settime(struct ptp_clock_info *ptp,
				const struct timespec64 *ts)
{
	u64 ns;
	unsigned long flags;
	struct cpts *cpts = container_of(ptp, struct cpts, info);

	ns = timespec64_to_ns(ts);

	spin_lock_irqsave(&cpts->lock, flags);
	timecounter_init(&cpts->tc, &cpts->cc, ns);
	spin_unlock_irqrestore(&cpts->lock, flags);

	return 0;
}

static int cpts_report_ts_events(struct cpts *cpts)
{
	struct list_head *this, *next;
	struct ptp_clock_event pevent;
	struct cpts_event *event;
	int reported = 0, ev;

	list_for_each_safe(this, next, &cpts->events) {
		event = list_entry(this, struct cpts_event, list);
		ev = event_type(event);
		if ((ev == CPTS_EV_HW) &&
			(cpts->hw_ts_enable &
			 (1 << (cpts_event_port(event) - 1)))) {
			list_del_init(&event->list);
			list_add(&event->list, &cpts->pool);
			/* report the event */
			pevent.timestamp =
				timecounter_cyc2time(&cpts->tc, event->low);
			pevent.timestamp -= cpts->pps_latch_offset;
			pevent.type = PTP_CLOCK_EXTTS;
			pevent.index = cpts_event_port(event) - 1;
			++reported;
			continue;
		}
	}
	return reported;
}

/* PPS */
static void cpts_pps_kworker(struct kthread_work *work)
{
	struct cpts *cpts = container_of(work, struct cpts, pps_work.work);

	cpts_pps_schedule(cpts);
}

static inline void cpts_pps_stop(struct cpts *cpts)
{
	u32 v;

	/* disable timer */
	v = READ_TCLR(cpts->odt);
	v &= ~BIT(0);
	WRITE_TCLR(cpts->odt, v);
#if(DEBUG_PPS_TMR)
	printk("PPS timer stopped\n");
#endif
}

static inline void cpts_pps_start(struct cpts *cpts)
{
	u32 v;

	cpts_tmr_reinit(cpts);

	/* enable timer */
	v = READ_TCLR(cpts->odt);
/* Timer Control Register
 * bit 0:	ST	Start timer */
	v |= BIT(0);
	WRITE_TCLR(cpts->odt, v);
#if(DEBUG_PPS_TMR)
	printk("PPS timer started\n");
#endif
}

static int cpts_pps_enable(struct cpts *cpts, int on)
{
	on = (on? 1 : 0);

	if (cpts->pps_enable == on)
		return 0;

	cpts->pps_enable = on;

	/* will stop after up coming pulse */
	if (!on)
	{ 
		printk("cpts_pps_enable : STOP\n"); 
		return 0; 
	}

	printk("Starting PPS\n");
	cpts_pps_start(cpts);
#if(0) //let first interrupt happen w/out correction
	cpts_tmr_poll(cpts);
#endif
	return 0;
}

static int cpts_pps_init(struct cpts *cpts)
{
	int err;

	cpts->pps_enable = DISABLE;

#ifdef CONFIG_OMAP_DM_TIMER
	omap_dm_timer_enable(cpts->odt);
#endif
	cpts_tmr_init(cpts);

	kthread_init_delayed_work(&cpts->pps_work, cpts_pps_kworker);
	cpts->pps_kworker = kthread_create_worker(0, "pps0");

	if (IS_ERR(cpts->pps_kworker)) {
		err = PTR_ERR(cpts->pps_kworker);
		pr_err("failed to create cpts pps worker %d\n", err);
		// TBD:add error handling
		return -1;
	}

	return 0;
}

static void cpts_pps_schedule(struct cpts *cpts)
{
	if (cpts->pps_enable == DISABLE) {
		printk("Disable\n");
		cpts_pps_stop(cpts);
	} else {
		cpts_tmr_poll(cpts);
	}
}

static int cpts_ptp_enable(struct ptp_clock_info *ptp,
	struct ptp_clock_request *rq, int on)
{
	struct cpts *cpts = container_of(ptp, struct cpts, info);
	bool ok;

	switch (rq->type) {
	case PTP_CLK_REQ_PPS:
		if (cpts->use_1pps) {
			ok = ptp_bc_clock_sync_enable(cpts->bc_clkid, on);
			if (!ok) {
				pr_info("cpts error: bc clk sync pps enable denied\n");
				return -EBUSY;
			}
		}
		printk("PPS query received.\n");
		return cpts_pps_enable(cpts, on);
	default:
		break;
	}

	return -EOPNOTSUPP;
}

static long cpts_overflow_check(struct ptp_clock_info *ptp)
{
	struct cpts *cpts = container_of(ptp, struct cpts, info);
	unsigned long delay = cpts->ov_check_period;
	struct timespec64 ts;
	unsigned long flags;

	spin_lock_irqsave(&cpts->lock, flags);
	ts = ns_to_timespec64(timecounter_read(&cpts->tc));

	if (cpts->hw_ts_enable)
		cpts_report_ts_events(cpts);
	if (!skb_queue_empty(&cpts->txq))
		delay = CPTS_SKB_TX_WORK_TIMEOUT;
	spin_unlock_irqrestore(&cpts->lock, flags);
#if(DEBUG_OVERFLOW)
	if(printk_ratelimit()) {
		printk("cpts overflow check at %lld.%09lu\n", ts.tv_sec, ts.tv_nsec);
	}
#endif
	return (long)delay;
}

static struct ptp_clock_info cpts_info = {
	.owner		= THIS_MODULE,
	.name		= "CTPS timer",
	.max_adj	= 1000000,
	.n_ext_ts	= 0,
	.n_pins		= 0,
	.pps		= 0,
	.adjfreq	= cpts_ptp_adjfreq,
	.adjtime	= cpts_ptp_adjtime,
	.gettime64	= cpts_ptp_gettime,
	.settime64	= cpts_ptp_settime,
	.enable		= cpts_ptp_enable,
	.do_aux_work	= cpts_overflow_check,
};

static int cpts_match(struct sk_buff *skb, unsigned int ptp_class,
	u16 ts_seqid, u8 ts_msgtype)
{
	u16 *seqid;
	unsigned int offset = 0;
	u8 *msgtype, *data = skb->data;

	if (ptp_class & PTP_CLASS_VLAN)
		offset += VLAN_HLEN;

	switch (ptp_class & PTP_CLASS_PMASK) {
	case PTP_CLASS_IPV4:
		offset += ETH_HLEN + IPV4_HLEN(data + offset) + UDP_HLEN;
		break;
	case PTP_CLASS_IPV6:
		offset += ETH_HLEN + IP6_HLEN + UDP_HLEN;
		break;
	case PTP_CLASS_L2:
		offset += ETH_HLEN;
		break;
	default:
		return 0;
	}

	if (skb->len + ETH_HLEN < offset + OFF_PTP_SEQUENCE_ID + sizeof(*seqid))
		return 0;

	if (unlikely(ptp_class & PTP_CLASS_V1))
		msgtype = data + offset + OFF_PTP_CONTROL;
	else
		msgtype = data + offset;

	seqid = (u16 *)(data + offset + OFF_PTP_SEQUENCE_ID);

	return (ts_msgtype == (*msgtype & 0xf) && ts_seqid == ntohs(*seqid));
}

static u64 cpts_find_ts(struct cpts *cpts, struct sk_buff *skb, int ev_type)
{
	u64 ns = 0;
	struct cpts_event *event;
	struct list_head *this, *next;
	unsigned int class = ptp_classify_raw(skb);
	unsigned long flags;
	u16 seqid;
	u8 mtype;

	if (class == PTP_CLASS_NONE)
		return 0;

	spin_lock_irqsave(&cpts->lock, flags);
	cpts_fifo_read(cpts, -1);
	list_for_each_safe(this, next, &cpts->events) {
		event = list_entry(this, struct cpts_event, list);
		if (event_expired(event)) {
			list_del_init(&event->list);
			list_add(&event->list, &cpts->pool);
			continue;
		}
		mtype = (event->high >> MESSAGE_TYPE_SHIFT) & MESSAGE_TYPE_MASK;
		seqid = (event->high >> SEQUENCE_ID_SHIFT) & SEQUENCE_ID_MASK;
		if (ev_type == event_type(event) &&
			cpts_match(skb, class, seqid, mtype)) {
			ns = timecounter_cyc2time(&cpts->tc, event->low);
			list_del_init(&event->list);
			list_add(&event->list, &cpts->pool);
			break;
		}
	}

	if (ev_type == CPTS_EV_TX && !ns) {
		struct cpts_skb_cb_data *skb_cb =
				(struct cpts_skb_cb_data *)skb->cb;
		/* Not found, add frame to queue for processing later.
		 * The periodic FIFO check will handle this.
		 */
		skb_get(skb);
		/* get the timestamp for timeouts */
		skb_cb->tmo = jiffies + msecs_to_jiffies(100);
		__skb_queue_tail(&cpts->txq, skb);
		ptp_schedule_worker(cpts->clock, 0);
	}
	spin_unlock_irqrestore(&cpts->lock, flags);

	return ns;
}

int cpts_rx_timestamp(struct cpts *cpts, struct sk_buff *skb)
{
	u64 ns;
	struct skb_shared_hwtstamps *ssh;

	if (!cpts->rx_enable)
		return -EPERM;
	ns = cpts_find_ts(cpts, skb, CPTS_EV_RX);
	if (!ns)
		return -ENOENT;
	ssh = skb_hwtstamps(skb);
	memset(ssh, 0, sizeof(*ssh));
	ssh->hwtstamp = ns_to_ktime(ns);

	return 0;
}
EXPORT_SYMBOL_GPL(cpts_rx_timestamp);

int cpts_tx_timestamp(struct cpts *cpts, struct sk_buff *skb)
{
	u64 ns;
	struct skb_shared_hwtstamps ssh;

	if (!(skb_shinfo(skb)->tx_flags & SKBTX_IN_PROGRESS))
		return -EPERM;
	ns = cpts_find_ts(cpts, skb, CPTS_EV_TX);
	if (!ns)
		return -ENOENT;
	memset(&ssh, 0, sizeof(ssh));
	ssh.hwtstamp = ns_to_ktime(ns);
	skb_tstamp_tx(skb, &ssh);

	return 0;
}
EXPORT_SYMBOL_GPL(cpts_tx_timestamp);

int cpts_register(struct cpts *cpts)
{
	int err, i;

	skb_queue_head_init(&cpts->txq);
	INIT_LIST_HEAD(&cpts->events);
	INIT_LIST_HEAD(&cpts->pool);
	for (i = 0; i < CPTS_MAX_EVENTS; i++)
		list_add(&cpts->pool_data[i].list, &cpts->pool);

	clk_enable(cpts->refclk);

	cpts_write32(cpts, CPTS_EN, control);
	cpts_write32(cpts, TS_PEND_EN, int_enable);

	timecounter_init(&cpts->tc, &cpts->cc, ktime_to_ns(ktime_get_real()));

	cpts->clock = ptp_clock_register(&cpts->info, cpts->dev);
	if (IS_ERR(cpts->clock)) {
		err = PTR_ERR(cpts->clock);
		cpts->clock = NULL;
		goto err_ptp;
	}
	cpts->phc_index = ptp_clock_index(cpts->clock);

	ptp_schedule_worker(cpts->clock, cpts->ov_check_period);
	cpts_write32(cpts, cpts_read32(cpts, control) |
			 HW4_TS_PUSH_EN, control);

	if (cpts->use_1pps)
		cpts->bc_clkid = ptp_bc_clock_register();

	pr_info("cpts ptp bc clkid %d\n", cpts->bc_clkid);
	return 0;

err_ptp:
	clk_disable(cpts->refclk);
	return err;
}
EXPORT_SYMBOL_GPL(cpts_register);

void cpts_unregister(struct cpts *cpts)
{
	if (WARN_ON(!cpts->clock))
		return;

	ptp_clock_unregister(cpts->clock);
	cpts->clock = NULL;

	cpts_write32(cpts, 0, int_enable);
	cpts_write32(cpts, 0, control);

	/* Drop all packet */
	skb_queue_purge(&cpts->txq);

	clk_disable(cpts->refclk);
}
EXPORT_SYMBOL_GPL(cpts_unregister);

static void cpts_calc_mult_shift(struct cpts *cpts)
{
	u64 frac, maxsec, ns;
	u32 freq;

	freq = clk_get_rate(cpts->refclk);

	/* Calc the maximum number of seconds which we can run before
	 * wrapping around.
	 */
	maxsec = cpts->cc.mask;
	do_div(maxsec, freq);
	/* limit conversation rate to 10 sec as higher values will produce
	 * too small mult factors and so reduce the conversion accuracy
	 */
	if (maxsec > 10)
		maxsec = 10;

	/* Calc overflow check period (maxsec / 2) */
	cpts->ov_check_period = (HZ * maxsec) / 2;
	cpts->ov_check_period_slow = cpts->ov_check_period;

	dev_info(cpts->dev, "cpts: overflow check period %lu (jiffies)\n",
		 cpts->ov_check_period);

	if (cpts->cc.mult || cpts->cc.shift)
		return;

	clocks_calc_mult_shift(&cpts->cc.mult, &cpts->cc.shift,
				   freq, NSEC_PER_SEC, maxsec);

	frac = 0;
	ns = cyclecounter_cyc2ns(&cpts->cc, freq, cpts->cc.mask, &frac);

	dev_info(cpts->dev,
		 "CPTS: ref_clk_freq:%u calc_mult:%u calc_shift:%u error:%lld nsec/sec\n",
		 freq, cpts->cc.mult, cpts->cc.shift, (ns - NSEC_PER_SEC));
}

static int cpts_of_1pps_parse(struct cpts *cpts, struct device_node *node)
{
	struct device_node *np = NULL;

	np = of_parse_phandle(node, "timers", 0);
	if (!np) {
		dev_err(cpts->dev, "device node lookup for pps timer failed\n");
		return -ENXIO;
	}

	cpts->pps_tmr_irqn = of_irq_get(np, 0);
	if (!cpts->pps_tmr_irqn)
		dev_err(cpts->dev, "cannot get 1pps timer interrupt number\n");

#ifdef CONFIG_OMAP_DM_TIMER
	cpts->odt = omap_dm_timer_request_by_node(np);
#endif
	if (IS_ERR(cpts->odt)) {
		dev_err(cpts->dev, "request for 1pps timer failed: %ld\n",
			PTR_ERR(cpts->odt));
		return PTR_ERR(cpts->odt);
	}

	cpts->pins = devm_pinctrl_get(cpts->dev);
	if (IS_ERR(cpts->pins)) {
		dev_err(cpts->dev, "request for 1pps pins failed: %ld\n",
			PTR_ERR(cpts->pins));
		return PTR_ERR(cpts->pins);
	}

	cpts->pin_state_pwm_on = pinctrl_lookup_state(cpts->pins, "pwm_on");
	if (IS_ERR(cpts->pin_state_pwm_on)) {
		dev_err(cpts->dev, "lookup for pwm_on pin state failed: %ld\n",
			PTR_ERR(cpts->pin_state_pwm_on));
		return PTR_ERR(cpts->pin_state_pwm_on);
	}

	cpts->pin_state_pwm_off = pinctrl_lookup_state(cpts->pins, "pwm_off");
	if (IS_ERR(cpts->pin_state_pwm_off)) {
		dev_err(cpts->dev, "lookup for pwm_off pin state failed: %ld\n",
			PTR_ERR(cpts->pin_state_pwm_off));
		return PTR_ERR(cpts->pin_state_pwm_off);
	}
	return 0;
}

static int cpts_of_parse(struct cpts *cpts, struct device_node *node)
{
	int ret = -EINVAL;
	u32 prop;

	if (!of_property_read_u32(node, "cpts_clock_mult", &prop))
		cpts->cc.mult = prop;

	if (!of_property_read_u32(node, "cpts_clock_shift", &prop))
		cpts->cc.shift = prop;

	if ((cpts->cc.mult && !cpts->cc.shift) ||
		(!cpts->cc.mult && cpts->cc.shift))
		goto of_error;

	if (!of_property_read_u32(node, "cpts-rftclk-sel", &prop)) {
		if (prop & ~CPTS_RFTCLK_SEL_MASK) {
			dev_err(cpts->dev, "cpts: invalid cpts_rftclk_sel.\n");
			goto of_error;
		}
		cpts->caps |= CPTS_CAP_RFTCLK_SEL;
		cpts->rftclk_sel = prop & CPTS_RFTCLK_SEL_MASK;
	}

	if (!of_property_read_u32(node, "cpts-ext-ts-inputs", &prop))
		cpts->ext_ts_inputs = prop;

	/* get timer for 1PPS */
	ret = cpts_of_1pps_parse(cpts, node);
	cpts->use_1pps = (ret == 0);

	return 0;

of_error:
	dev_err(cpts->dev, "CPTS: Missing property in the DT.\n");
	return ret;
}

struct cpts *cpts_create(struct device *dev, void __iomem *regs,
			 struct device_node *node)
{
	struct cpts *cpts;
	int ret;

	cpts = devm_kzalloc(dev, sizeof(*cpts), GFP_KERNEL);
	if (!cpts)
		return ERR_PTR(-ENOMEM);

	cpts->dev = dev;
	cpts->reg = (struct cpsw_cpts __iomem *)regs;
	spin_lock_init(&cpts->lock);

	ret = cpts_of_parse(cpts, node);
	if (ret)
		return ERR_PTR(ret);

	cpts->refclk = devm_clk_get(dev, "cpts");
	if (IS_ERR(cpts->refclk)) {
		dev_err(dev, "Failed to get cpts refclk\n");
		return ERR_PTR(PTR_ERR(cpts->refclk));
	}

	clk_prepare(cpts->refclk);

	if (cpts->caps & CPTS_CAP_RFTCLK_SEL)
		cpts_write32(cpts, cpts->rftclk_sel, rftclk_sel);

	cpts->cc.read = cpts_systim_read;
	cpts->cc.mask = CLOCKSOURCE_MASK(32);
	cpts->info = cpts_info;

	if (cpts->ext_ts_inputs)
		cpts->info.n_ext_ts = cpts->ext_ts_inputs;

	cpts_calc_mult_shift(cpts);
	/* save cc.mult original value as it can be modified
	 * by cpts_ptp_adjfreq().
	 */
	cpts->cc_mult = cpts->cc.mult;

	if (cpts->pps_tmr_irqn) {
		ret = devm_request_irq(dev, cpts->pps_tmr_irqn,
					   cpts_1pps_tmr_interrupt,
					   0, "1pps_timer", cpts);
		if (ret < 0) {
			dev_err(dev, "unable to request 1pps timer IRQ %d (%d)\n",
				cpts->pps_tmr_irqn, ret);
			return ERR_PTR(ret);
		}
	}

	if (cpts->use_1pps) {
		ret = cpts_pps_init(cpts);

		if (ret < 0) {
			dev_err(dev, "unable to init PPS resource (%d)\n",
				ret);
			return ERR_PTR(ret);
		}

		/* Enable 1PPS related features	*/
		cpts->info.pps			= 1;
		cpts->info.n_ext_ts		= CPTS_MAX_LATCH;
		cpts->info.n_per_out	= 1;
	}

	return cpts;
}
EXPORT_SYMBOL_GPL(cpts_create);

void cpts_release(struct cpts *cpts)
{
	if (!cpts)
		return;

#ifdef CONFIG_OMAP_DM_TIMER
	if (cpts->odt) {
		omap_dm_timer_disable(cpts->odt);
		omap_dm_timer_free(cpts->odt);
	}
	if (cpts->odt) {
		devm_pinctrl_put(cpts->pins);
	}

#endif
	if (cpts->pps_kworker) {
		kthread_cancel_delayed_work_sync(&cpts->pps_work);
		kthread_destroy_worker(cpts->pps_kworker);
	}

	if (WARN_ON(!cpts->refclk))
		return;

	clk_unprepare(cpts->refclk);
}
EXPORT_SYMBOL_GPL(cpts_release);

static u64 cpts_ts_read(struct cpts *cpts)
{
	u64 ns = 0;
	struct cpts_event *event;
	struct list_head *this, *next;

	if (cpts_fifo_read(cpts, CPTS_EV_PUSH))
		printk("cpts: ts_read: unable to obtain a time stamp\n");

	list_for_each_safe(this, next, &cpts->events) {
		event = list_entry(this, struct cpts_event, list);
		if (event_type(event) == CPTS_EV_PUSH) {
			list_del_init(&event->list);
			list_add(&event->list, &cpts->pool);
			ns = timecounter_cyc2time(&cpts->tc, event->low);
			break;
		}
	}

	return ns;
}

enum cpts_1pps_state {
	/* Initial state: try to SYNC to the CPTS timestamp */
	INIT = 0,
	/* Sync State: track the clock drift, trigger timer
	 * adjustment when the clock drift exceed 1 clock
	 * boundary declare out of sync if the clock difference is more
	 * than a 1ms
	 */
	SYNC = 1,
	/* Adjust state: Wait for time adjust to take effect at the
	 * timer reload time
	 */
	ADJUST = 2,
	/* Wait state: PTP timestamp has been verified,
	 * wait for next check period
	 */
	WAIT = 3
};

static void cpts_tmr_reinit(struct cpts *cpts)
{
	/* re-initialize timer for 1pps generator */
	WRITE_TCLR(cpts->odt, 0);
/* When the auto-reload mode is enabled (TCLR AR bit = 1),
 * the TCRR is reloaded with the Timer Load Register (TLDR) value
 * after a counting overflow. */
	WRITE_TLDR(cpts->odt, CPTS_TMR_RELOAD_CNT);
	WRITE_TCRR(cpts->odt, CPTS_TMR_RELOAD_CNT);
	WRITE_TMAR(cpts->odt, CPTS_TMR_CMP_CNT);
/* Timer Control Register
 * bit12 :		PT		Pulse or toggle mode on PORTIMERPWM output pin : 
 * 							Toggle
 * bit10-11 :	TRG		Trigger output mode on PORTIMERPWM output pin :
 * 							Trigger on overflow and match
 * bit1 :		AR		Auto-reload timer */
	WRITE_TCLR(cpts->odt, BIT(12) | 2 << 10 | BIT(6) | BIT(1));
#if(0)
/* Timer Synchronous Interface Control Register
 * bit2 :		POSTED	Posted mode active */
	WRITE_TSICR(cpts->odt, BIT(2));
#endif
	cpts->count_prev = 0xFFFFFFFF;
	cpts->pps_state = INIT;
#if(DEBUG_PPS_TMR)
	printk("PPS timer reinitialized\n");
#endif
}

static void cpts_tmr_init(struct cpts *cpts)
{
	struct clk *parent;
	int ret;

	if (!cpts)
		return;

	parent = clk_get(&cpts->odt->pdev->dev, "sys_clkin_ck");
	if (IS_ERR(parent)) {
		pr_err("%s: %s not found\n", __func__, "sys_clkin_ck");
		return;
	}

	ret = clk_set_parent(cpts->odt->fclk, parent);
	if (ret < 0)
		pr_err("%s: failed to set %s as parent\n", __func__,
			   "sys_clkin_ck");

	/* initialize timer16 for 1pps generator */
	cpts_tmr_reinit(cpts);

	writel_relaxed(OMAP_TIMER_INT_OVERFLOW, cpts->odt->irq_ena);
	__omap_dm_timer_write(cpts->odt, OMAP_TIMER_WAKEUP_EN_REG,
				  OMAP_TIMER_INT_OVERFLOW, 0);

	pinctrl_select_state(cpts->pins, cpts->pin_state_pwm_off);
}

/* The reload counter value is going to affect all cycles after the next SYNC
 * check. Therefore, we need to change the next expected drift value by
 * updating the ts_correct value
 */
static int ts_correct;
static void update_ts_correct(void)
{
	if (tmr_reload_cnt > tmr_reload_cnt_prev)
		ts_correct -= (tmr_reload_cnt - tmr_reload_cnt_prev) * CPTS_TMR_CLK_PERIOD_NS;
	else
		ts_correct += (tmr_reload_cnt_prev - tmr_reload_cnt) * CPTS_TMR_CLK_PERIOD_NS;
}

u32 ts_push_avrg(u32 ts_push_imm)
{
	u32 ret_val;
	static u32 ts_push_sum;
	static u32 ts_push_cnt;
	ts_push_sum += ts_push_imm;
	ts_push_cnt++;
	ret_val = ts_push_sum/ts_push_cnt;
	return ret_val;
}

static void cpts_tmr_poll(struct cpts *cpts)
{
	unsigned long flags;
	u32 tmr_count, tmr_count2, count_exp, tmr_diff_abs, ts_push_ticks;
	s32 tmr_diff = 0;
	int ts_val;
	static int ts_val_prev;
	u64 cpts_ts_rem, cpts_ts, tmp64;
	static u64 cpts_ts_trans;
	bool updated = false;
	static bool first;

	if (!cpts)
		return;

	spin_lock_irqsave(&cpts->lock, flags);

	tmr_count = READ_TCRR(cpts->odt);
	cpts_write32(cpts, TS_PUSH, ts_push);
	tmr_count2 = READ_TCRR(cpts->odt);
	ts_push_ticks = ts_push_avrg(tmr_count2 - tmr_count); /* push call weight in ticks */
	
	tmp64 = cpts_ts_read(cpts);
	cpts_ts = tmp64;
/*
 * do_div() is NOT a C function. It wants to return
 * two values (the quotient and the remainder), but
 * since that doesn't work very well in C, what it
 * does is:
 * - modifies the 64-bit dividend _in_place_
 * - returns the 32-bit remainder
 */
	cpts_ts_rem = do_div(tmp64, CPTS_PPS_PERIOD_NS);
	tmp64 = cpts_ts;

	/* Timer poll state machine */
	switch (cpts->pps_state) {
	case INIT:
		pps_tmr_irq_cnt = 0;
		if ((cpts_ts_rem < CPTS_TS_THRESH_NS) &&
			(ts_push_ticks < CPTS_MAX_TS_ACCSS_T_TICKS)) {
			/* The nominal delay of this operation about 9 ticks
			 * We are able to compensate for the normal range 8-17
			 * However, the simple compensation fails when the delay
			 * is getting big, just skip this sample */
			
			 /* Adjust timer abruptly to the reminder  */
			tmp64 = (CPTS_PPS_PERIOD_NS - cpts_ts_rem); //annual ts
			do_div(tmp64, CPTS_TMR_CLK_PERIOD_NS); //from ns to ticks
			count_exp = (u32)tmp64;
			count_exp = TMR_CNTR_MAX - count_exp + 1; //invert for timer

			u32 pps_tmr_count_reg = count_exp + /* Move timer to the ts reminder */
				( READ_TCRR(cpts->odt) - tmr_count2 ) + /* time in ticks from ts push to this point */
				ts_push_ticks; /* push call weight in ticks */
			WRITE_TCRR(cpts->odt, pps_tmr_count_reg); /* Timer Counter Register */
			WRITE_TLDR(cpts->odt, tmr_reload_cnt);
			WRITE_TMAR(cpts->odt, CPTS_TMR_CMP_CNT);
#if(DEBUG_INIT)
			printk( GRN"INIT : pps_tmr_count_reg = %d, count_exp = %d, tmr_reload_cnt = %d\n" \
					"tmr_count = %d, tmr_count2 = %d, ts_push_ticks = %d\n,"\
					"CPTS_TMR_CMP_CNT = %d, CPTS_TMR_RELOAD_CNT = %d, CPTS_TMR_CLK_PERIOD_NS = %lu\n"
					"CPTS_PPS_PERIOD_NS = %u, cpts_ts_rem = %llu\n"NORM,
					pps_tmr_count_reg, count_exp, tmr_reload_cnt,
					tmr_count, tmr_count2, ts_push_ticks, 
					(s32)CPTS_TMR_CMP_CNT, (s32)CPTS_TMR_RELOAD_CNT, CPTS_TMR_CLK_PERIOD_NS,
					CPTS_PPS_PERIOD_NS, cpts_ts_rem );
#else
			if(printk_ratelimit()) { printk( GRN"INIT\n"NORM); }
#endif
			{
				cpts->pps_state = WAIT;
				first = true;
				tmr_reload_cnt_prev = tmr_reload_cnt;
				cpts_ts_trans = (cpts_ts - cpts_ts_rem) + CPTS_PPS_PERIOD_NS; //timestamp of the next meeting
				printk("cpts_tmr_poll: exit INIT state\n");
			}
		} 
		else
		{
#if(DEBUG_MAX)
			printk( RED"AGAIN : cpts_ts_rem = %llu, CPTS_TS_THRESH_NS = %d\n"\
					"ts_push_ticks = %d, tmr_count = %d, tmr_count2 = %d\n"\
					"CPTS_MAX_TS_ACCSS_T_TICKS=%lu\n"NORM,
					cpts_ts_rem, CPTS_TS_THRESH_NS,
					ts_push_ticks, tmr_count, tmr_count2,
					CPTS_MAX_TS_ACCSS_T_TICKS );
#else
			if(printk_ratelimit()) { printk( RED"AGAIN\n"); }
#endif
		}
		break;

	case ADJUST:
		/* Wait for the ldr load to take effect */
		if (cpts_ts >= cpts_ts_trans) {
		#if(0)
			u64 ts = cpts->hw_timestamp;
			ts_offset = do_div(ts, CPTS_PPS_PERIOD_NS);
		#else
			u32 ts_offset = cpts_ts_rem;
		# if(DEBUG_MAX)
			//if(printk_ratelimit()) { printk(MAG"off=%dns\n"NORM, ts_offset); }
		# endif
		#endif
			ts_val = (ts_offset >= CPTS_PPS_PERIOD_NS/2) ?
				(ts_offset - CPTS_PPS_PERIOD_NS) :	//undershoot - negative value
				(ts_offset);						//overshoot - positive value

			/* restore the timer period */
			WRITE_TLDR(cpts->odt, tmr_reload_cnt);

			if (tmr_reload_cnt != tmr_reload_cnt_prev)
				update_ts_correct();

			cpts_ts_trans += CPTS_PPS_PERIOD_NS;
			cpts->pps_state = WAIT;

			tmr_reload_cnt_prev = tmr_reload_cnt;
			ts_val_prev = ts_val;
			//if(printk_ratelimit()) { printk(GRN"adj\n"NORM); }
		}
		break;

	case WAIT:
		/* Wait for the next poll period when the adjustment
		 * has been taken effect */
		if (cpts_ts < cpts_ts_trans)
		{
			if(printk_ratelimit()) { printk( RED"early\n"); }
			break;
		}
		cpts->pps_state = SYNC;
		/* pass through */

	case SYNC:
		{
#if(0)
			if(printk_ratelimit()) { printk("sync\n"); }
#endif
			s32 ts_adj;
		#if(0)
			u64 ts = cpts->hw_timestamp;
			u32 ts_offset = do_div(ts, CPTS_PPS_PERIOD_NS);
		#else
			u32 ts_offset = cpts_ts_rem;
		#endif

			ts_val = (ts_offset >= (CPTS_PPS_PERIOD_NS/2)) ?
				(ts_offset - CPTS_PPS_PERIOD_NS) :	//undershoot	- negative value
				(ts_offset);						//overshoot		- positive value
			/* ts_adj should include the current error and the expected
			 * drift for the next two cycles */
			if (first) {
				ts_adj = ts_val;
				first = false;
			} else
				ts_adj = ts_val + (ts_val - ts_val_prev + ts_correct) * 2;

			tmr_diff = (ts_adj < 0) ? \
					(ts_adj - (s32)CPTS_TMR_CLK_PERIOD_NS/2) / (s32)CPTS_TMR_CLK_PERIOD_NS :
					(ts_adj + (s32)CPTS_TMR_CLK_PERIOD_NS/2) / (s32)CPTS_TMR_CLK_PERIOD_NS;
			/* adjust for the error in the current cycle due to the old (incorrect) reload count
			 * we only make the adjustment if the counter change is more than 1 because the
			 * couner will change back and forth at the frequency tick boundary
			 */
			if (tmr_reload_cnt != tmr_reload_cnt_prev) {
				if (tmr_reload_cnt > tmr_reload_cnt_prev)
					tmr_diff += (tmr_reload_cnt -
							 tmr_reload_cnt_prev - 1);
				else
					tmr_diff -= (tmr_reload_cnt_prev -
							 tmr_reload_cnt - 1);
			}
#if(0)
			if(printk_ratelimit()) {
				printk( CYN"ts_val = %d, ts_val_prev = %d\n"NORM,
						ts_val, ts_val_prev);
			}
#endif
			ts_correct = tmr_diff * CPTS_TMR_CLK_PERIOD_NS;
			ts_val_prev = ts_val;
			tmr_diff_abs = abs(tmr_diff);

			//If new counter value or diff should be made
			if (tmr_diff_abs || (tmr_reload_cnt != tmr_reload_cnt_prev)) {
				updated = true;
				if (tmr_diff_abs < CPTS_OUT_OF_SYNC_TMR_TICKS) {
					/* adjust ldr time for one period
					 * instead of updating the tcrr directly */
					u32 reload = tmr_reload_cnt + (u32)tmr_diff;
#if(0)
					if(printk_ratelimit()) { printk(MAG"reload=%d\n"NORM, reload); }
#endif
					WRITE_TLDR(cpts->odt, reload);
					cpts->pps_state = ADJUST;
				} else {
					cpts->pps_state = INIT;
					printk( RED"OUT OF SYNC : tmr_diff = %d, ts_val = %d, ts_val_prev=%d,\n"
							"ts_offset = %d, ts_correct = %d, ts_adj=%d\n,"
							"tmr_reload_cnt = %d, tmr_reload_cnt_prev = %d\n"NORM, 
							tmr_diff, ts_val, ts_val_prev,
							ts_offset, ts_correct, ts_adj,
							tmr_reload_cnt, tmr_reload_cnt_prev);
					break;
				}
			} else {
				cpts->pps_state = WAIT;
			}

			cpts_ts_trans = (cpts_ts - cpts_ts_rem) + CPTS_TMR_CLK_RATE;
			tmr_reload_cnt_prev = tmr_reload_cnt;
#if(DEBUG_MAX)
			if(printk_ratelimit()) { printk(CYN"off=%dns\n"NORM, ts_offset); }
#endif
			break;
		} /* case SYNC */

	} /* switch */

	spin_unlock_irqrestore(&cpts->lock, flags);

	cpts->count_prev = tmr_count;
#if(0)
	if(updated)
		printk( CYN"tmr_diff = %d, tmr_reload_cnt = %u\n"NORM,
				tmr_diff, tmr_reload_cnt);
#endif
}


static irqreturn_t cpts_1pps_tmr_interrupt(int irq, void *dev_id)
{
	struct cpts *cpts = (struct cpts*)dev_id;

	writel_relaxed(OMAP_TIMER_INT_OVERFLOW, cpts->odt->irq_stat);
	kthread_queue_delayed_work(cpts->pps_kworker, &cpts->pps_work,
		msecs_to_jiffies(CPTS_PPS_WIDTH_MS));
#if(DEBUG_PPS_TMR_IRQ)
	if(!pps_tmr_irq_cnt)
		printk("PPS timer irq\n");
	if(pps_tmr_irq_cnt <= 10)
		printk("+\n");
	if(pps_tmr_irq_cnt <= 1000)
		pps_tmr_irq_cnt++;
	if ((pps_tmr_irq_cnt % 100) == 0)
		printk("cpts_1pps_tmr_interrupt %d\n", pps_tmr_irq_cnt);
#endif
	return IRQ_HANDLED;
}

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("TI CPTS driver");
MODULE_AUTHOR("Richard Cochran <richardcochran@gmail.com>");
