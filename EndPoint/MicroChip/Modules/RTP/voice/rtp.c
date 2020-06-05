#include "global.h"
#include "linux/poll.h"

#define RTP_EVENT_COUNT         16
#define DEF_PACKET_FACTOR       2
#define MAX_PACKET_FACTOR       18
#define MAX_FRAME_SIZE          1500

/* jitter buffer default settings */
#define UPPER_WATERMARK         10
#define UPPER_DROP_WATERMARK    8
#define LOWER_DROP_WATERMARK    3
#define LOWER_WATERMARK         1

#define VOICE_SLABNAME          "voice_sessions"
#define RTP_CLASSNAME           "kvoice_rtp"


#define GET_SESSION \
    session_t *s; \
    s = (session_t *)file->private_data;\
    if (s==NULL) return -EBADF;
    
#define GET_CHANNEL \
    channel_t *c; \
    c = s->channel; \
    if (c==NULL) return -ENODEV;


/* Functions prototypes */
void module_inc_use_count(void);
void module_dec_use_count(void);

/* Variables */
unsigned int s_active               = 0;
static struct file_operations rtp_fops;
static int rtp_major                = 0;
struct kmem_cache *session_cache    = NULL;
static struct class*  rtp_class     = NULL;
static struct device* rtp_device    = NULL;

LIST_HEAD(rtp_sessions);
struct semaphore rtp_sessions_lock;

static void process_tx(unsigned long data)
{
    u8 *skb_data;
    struct sk_buff *skb;
    session_t* s = (session_t*)data;
    channel_t *c;
    int tx_pt, nread;
    int encoded;

    /* start of channels processing */
    /* in bundle case the channel processing
       loop will be here */
    c = s->channel;
    if (c == NULL) return;

    encoded = c->encoded_bytes(c);
    if (encoded < s->code_frame_size) return;
    while (encoded >= s->code_frame_size)
    {
        /* skb allocated here temporary, until bundle absent */ 
        skb = dev_alloc_skb(MAX_FRAME_SIZE);
        if (skb == NULL) return;
        skb_reserve(skb, ETH_RTP_HDR_SIZE);
        /* bytes count inside "read_frame" function */
        skb_data=(u8 *)skb_put(skb, s->code_frame_size);
        nread = c->read_enc_frame(c, &tx_pt, skb_data, s->code_frame_size);
        /* end of channels processing */
        skb_payloadtype(skb)=s->parm.tx_pt;
        skb_session(skb)=s;
        skb_ts(skb)=s->pcm_frame_size;
        rtp_send_skb(skb);      
        s->stat.ptx++;
        s->stat.tx_last = jiffies;

        encoded -= s->code_frame_size;
    }
}

static int rtp_begin_session(struct file *file, struct rtp_connection_params* ios)
{
    int err;
    unsigned long copied;
    GET_SESSION
    
    CHECK_PARAM(ios, VERIFY_READ);

    copied = copy_from_user(&s->parm, ios, sizeof(struct rtp_connection_params));
    
    s->active_filter = 0;
    s->filter.recv_addr = 0;
    s->filter.ssrc = 0;
    s->stat.tx_last = 0xFFFFFFFF;
    s->stat.rx_last = 0xFFFFFFFF;

    if (s->parm.packet_factor <= 0) s->parm.packet_factor = DEF_PACKET_FACTOR;
    if (s->parm.packet_factor > MAX_PACKET_FACTOR) s->parm.packet_factor = MAX_PACKET_FACTOR;

    /* precalc code_frame_size for TX process */
    switch (s->parm.rx_pt) {
        case PAYLOAD_TYPE_G711A:
        case PAYLOAD_TYPE_G711U:
            s->code_frame_size = s->parm.packet_factor * G711_DSP_FRAME_SIZE;
            s->pcm_frame_size = s->parm.packet_factor * G711_PCM_FRAME_SIZE;
        break;
        case PAYLOAD_TYPE_G711A_MCC:
        case PAYLOAD_TYPE_G711U_MCC:
        case PAYLOAD_TYPE_G723:
        case PAYLOAD_TYPE_G729:
            s->code_frame_size = 0;
            s->pcm_frame_size = 0;
        break;
    }

    #define saddr_c ((unsigned char *)&(s->parm.send_addr))
    RPRINTK("starting session: %d.%d.%d.%d/%d/%d PT(rx/tx)=%d/%d, PF=%d, VAD=%s%s\n",
        saddr_c[0], saddr_c[1], saddr_c[2], saddr_c[3],
        s->parm.send_port, s->parm.recv_port,
        s->parm.rx_pt, s->parm.tx_pt,
        s->parm.packet_factor,
        s->parm.vad ? "on":"off",
        s->parm.rate ? ", high rate":"");
    RPRINTK("rtp session: %p\n", s);

    err = rtp_create_connection(s);
    if(err) {
        RPRINTK("failed to create rtp session, error code=%d\n", err);
        return err;
    }
    
    down(&rtp_sessions_lock);
    list_add(&s->node, &rtp_sessions);
    up(&rtp_sessions_lock);

    s_active++;
    return 0;
}

void inline bind_channel(session_t* s, channel_t* c)
{
    s->channel = c;
    c->link = s;

    c->decoder_upper_watermark      = s->code_frame_size * UPPER_WATERMARK;
    c->decoder_upper_drop_watermark = s->code_frame_size * UPPER_DROP_WATERMARK;
    c->decoder_lower_drop_watermark = s->code_frame_size * LOWER_DROP_WATERMARK;
    c->decoder_lower_watermark      = s->code_frame_size * LOWER_WATERMARK;

    c->ufw = 0;
    c->ufw = 0;
}

void inline unbind_channel(session_t* s, channel_t* c)
{
    if (c==NULL) {
        printk("attempt to release NULL channel\n");
        return;
    }
    s->channel = NULL;
    c->link = NULL;
    RPRINTK("stopped session for channel %d\n", c->tdm);
    free_channel(c);
}

static int rtp_get_stat_info(struct file *file, struct rtp_stat_info* ios)
{
    struct rtp_stat_info    stat;
    unsigned long copied;
    GET_SESSION
    
    stat.ptx = s->stat.ptx;
    stat.prx = s->stat.prx;
    stat.tx_dropped = s->stat.tx_dropped;
    stat.rx_dropped = s->stat.rx_dropped;
    
    if (time_after(jiffies, ((unsigned long)s->stat.tx_last)))
    {
        stat.tx_last = jiffies - s->stat.tx_last;
        stat.tx_last *= 1000;
        stat.tx_last /= HZ;
    }
    else
    {
        s->stat.tx_last = 0xFFFFFFFF;
        stat.tx_last = 0xFFFFFFFF;
    }

    if (time_after(jiffies, ((unsigned long)s->stat.rx_last)))
    {
        stat.rx_last = jiffies - s->stat.rx_last;
        stat.rx_last *= 1000;
        stat.rx_last /= HZ;
    }
    else
    {
        s->stat.tx_last = 0xFFFFFFFF;
        stat.tx_last = 0xFFFFFFFF;
    }

    copied = copy_to_user(ios, &stat, sizeof(struct rtp_stat_info));    
    return 0;
}

static int rtp_add_channel(struct file *file, struct rtp_channel_info* ios)
{
    channel_t* c;
    struct rtp_channel_info prm;
    unsigned long copied;
    unsigned int caps;
    
    GET_SESSION
    
    CHECK_PARAM(ios, VERIFY_READ);
    CHECK_PARAM(ios, VERIFY_WRITE);

    copied = copy_from_user(&prm, ios, sizeof(struct rtp_channel_info));

    RPRINTK("tdm=%d, dsp=%d, ch=%d\n", prm.tdm, prm.dev_no, prm.chn);
    caps = caps_from_pt(s->parm.rx_pt, s->parm.tx_pt);
    RPRINTK("inherited from session: PT(rx/tx)=%d/%d. caps=%08x\n", s->parm.rx_pt, s->parm.tx_pt, caps);
    
    if (prm.tdm >= 0) {
        c = get_channel_tdm(prm.tdm, caps);
        if(c==NULL) {
            RPRINTK("fail to get channel on tdm=%d\n", prm.tdm);
            return -ENODEV;
        }
        goto done;
    }
    
    if (prm.dev_no >= 0) {
        c = get_channel(prm.dev_no, prm.chn, caps);
        if(c==NULL) {
            RPRINTK("cannot get channel: dsp(%d,%d)\n", prm.dev_no, prm.chn);
            return -ENODEV;
        }
        goto done;
    } else {
        c = alloc_channel(caps);
        if(c==NULL) {
            RPRINTK("cannot allocate channel\n");
            return -ENODEV;
        }
    }

    /* TODO:    if add more than one channel need to check PF   */
    /* temporary code designed for one channel only             */
    
done:
    bind_channel(s, c);

    if (c->parent->dsp >= 0)
        prm.dev_no = (int)c->parent->dsp;
    else
        prm.dev_no = -1;

    prm.tdm = c->tdm;
    prm.chn = c->slot;
    RPRINTK("return %d\n", prm.tdm);
    copied = copy_to_user(ios, &prm, sizeof(struct rtp_channel_info));
    return c->tdm;
}

static int rtp_start_tx(struct file *file)
{
    GET_SESSION
    if (s->active == 0) {
        msec_sched_add_task(process_tx, (unsigned long)s, s->parm.packet_factor);
        s->active = 1;
        RPRINTK("done[timing step=%d]\n",s->parm.packet_factor);
    } else {
        RPRINTK("already active\n");
    }
    return 0;
}

static int rtp_stop_tx(struct file *file)
{
    GET_SESSION
    if (s->active) {
        msec_sched_remove_task(process_tx, (unsigned long)s);
        s->active = 0;
        RPRINTK("done \n");
    } else {
        RPRINTK("try to stop inactive transmission\n");
    }
    return 0;
}

static int rtp_reset_filter(struct file *file)
{
    GET_SESSION
    if (s->active_filter) {
        s->active_filter = 0;
        s->filter.recv_addr = 0;
        s->filter.ssrc = 0;
        RPRINTK("reset filter\n");
    } else {
        RPRINTK("try to reset unset filter\n");
    }
    return 0;
}

static int rtp_set_filter(struct file *file, struct rtp_filter_params* flt)
{
    unsigned char *recv_addr;
    unsigned long copied;
    
    GET_SESSION
    
    s->active_filter = 1;   
    copied = copy_from_user(&s->filter, flt, sizeof(struct rtp_filter_params));
    
    recv_addr = (unsigned char *)&(s->filter.recv_addr);

    RPRINTK("set filter: IP=%d.%d.%d.%d, SSRC=0x%08lX\n",
        recv_addr[0], recv_addr[1], recv_addr[2], recv_addr[3], s->filter.ssrc);

    return 0;
}
/*--- End of function ------------------------------------------------------*/
int rtp_free_channel(struct file *file, unsigned int arg)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeclaration-after-statement" 
    GET_SESSION
    GET_CHANNEL
#pragma GCC diagnostic pop

    unbind_channel(s,c);
    return 0;
}
/*--- End of function ------------------------------------------------------*/
static int rtp_open(struct inode *inode, struct file *file)
{
    session_t* s;
    module_inc_use_count();

    s = kmem_cache_alloc(session_cache, GFP_KERNEL);
    if (s==NULL) return -ENOMEM;

    memset(s, 0, sizeof(session_t));
    init_waitqueue_head(&s->pwait);
    file->private_data = s;
    INIT_LIST_HEAD(&s->node);
    return 0;
}
/*--- End of function ------------------------------------------------------*/
static int rtp_release(struct inode *inode, struct file *file)
{
    module_dec_use_count();
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeclaration-after-statement" 
    GET_SESSION
#pragma GCC diagnostic pop
    /* stop transmission */
    if (s->active) {
        msec_sched_remove_task(process_tx, (unsigned long)s);
        s->active = 0;
    }
    /* release network resources */
    local_bh_disable();

    rtp_close_connection(s);
    list_del(&s->node);
    s_active--;

    local_bh_enable();

    unbind_channel(s, s->channel);
    kmem_cache_free(session_cache, s);
    return 0;
}
/*--- End of function ------------------------------------------------------*/
unsigned int rtp_poll(struct file *file, struct poll_table_struct *pt)
{
    GET_SESSION

    poll_wait(file, &s->pwait, pt);
    if (s->stat.prx > RTP_EVENT_COUNT) return POLLIN;
    return 0;
}
/*--- End of function ------------------------------------------------------*/
int rtp_ctl_init(void *context)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
    session_cache = kmem_cache_create( VOICE_SLABNAME, sizeof(session_t), 0, SLAB_HWCACHE_ALIGN, NULL, NULL);
#else 
    session_cache = kmem_cache_create( VOICE_SLABNAME, sizeof(session_t), 0, SLAB_HWCACHE_ALIGN, NULL);
#endif 
    if(!session_cache) return -ENOMEM;
    rtp_major = register_chrdev(0, RTP_DEVNAME, &rtp_fops);
    
    if (rtp_major < 0)
    {
        kmem_cache_destroy(session_cache);
        printk(KERN_DEBUG  "failed to register device \"" RTP_DEVNAME "\" with error %d\n", rtp_major);
        return -EBUSY;
    }
    
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
    rtp_class = class_create(THIS_MODULE, RTP_CLASSNAME);
    if (IS_ERR(rtp_class))
    {
        kmem_cache_destroy(session_cache);
        unregister_chrdev(rtp_major, RTP_DEVNAME);
        printk(KERN_ALERT "Failed to register device class '%s'\n", RTP_CLASSNAME);
        return PTR_ERR(rtp_class);
    }

    rtp_device = device_create(rtp_class, NULL, MKDEV(rtp_major, 0), NULL, RTP_DEVNAME);
    if (IS_ERR(rtp_device))
    {
        class_destroy(rtp_class);
        kmem_cache_destroy(session_cache);
        unregister_chrdev(rtp_major, RTP_DEVNAME);
        printk(KERN_ALERT "Failed to create the device '%s'\n", RTP_DEVNAME);
        
        return PTR_ERR(rtp_device);
    }
#endif
    
    INIT_LIST_HEAD(&rtp_sessions);
    sema_init(&rtp_sessions_lock, 1);
    return 0;
}
/*--- End of function ------------------------------------------------------*/
void rtp_ctl_deinit(void *context)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
    device_destroy(rtp_class, MKDEV(rtp_major, 0));
    class_unregister(rtp_class);
    class_destroy(rtp_class);
#endif
    
    unregister_chrdev(rtp_major, RTP_DEVNAME);
    if(session_cache) kmem_cache_destroy(session_cache);
}
/*--- End of function ------------------------------------------------------*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
int rtp_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
#else
long rtp_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#endif
{
    switch (cmd)
    {
        case RTP_BEGIN_SESSION:     return rtp_begin_session(file, (struct rtp_connection_params*) arg);

        case RTP_ADD_CHANNEL:       return rtp_add_channel(file, (struct rtp_channel_info*) arg);
        case RTP_REMOVE_CHANNEL:    return rtp_free_channel(file, (unsigned int)arg);

        case RTP_START_TX:          return rtp_start_tx(file);
        case RTP_STOP_TX:           return rtp_stop_tx(file);
        
        case RTP_SET_FILTER:        return rtp_set_filter(file, (struct rtp_filter_params*) arg);
        case RTP_RESET_FILTER:      return rtp_reset_filter(file);
        
        case RTP_STAT_INFO:         return rtp_get_stat_info(file, (struct rtp_stat_info*) arg);
    }
    return 0;/*-EINVAL;*/
}
/*--- End of function ------------------------------------------------------*/
static struct file_operations rtp_fops = {
    open:           rtp_open,
    release:        rtp_release,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
    ioctl:          rtp_ioctl,
#else
    unlocked_ioctl: rtp_ioctl,
    compat_ioctl:   rtp_ioctl,
#endif
    poll:           rtp_poll
};
/*--- End of function ------------------------------------------------------*/
