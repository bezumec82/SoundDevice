/* microElk */

#include <linux/clk-provider.h>
#include <linux/clkdev.h>
#include <linux/clk/at91_pmc.h>
#include <linux/of.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>

#include "pmc.h"

#define PERIPHERAL_MAX          64
#define PERIPHERAL_ID_MIN       2

#define GENERATED_SOURCE_MAX    6
#define GENERATED_MAX_DIV       0xff

#define SDHC0_PID               31
#define SDHC1_PID               32

struct clk_generated {
    struct clk_hw       hw;
    struct regmap *     regmap;
    struct clk_range    range;
    spinlock_t *        lock;
    u32                 id;
    u32                 gck_div;
    u8                  clk_src;
};

#define to_clk_generated(hw) container_of(hw, struct clk_generated, hw)

static int                      clk_generated_enable(struct clk_hw * hw)
{
    struct clk_generated *  gck = to_clk_generated(hw);
    unsigned long           flags;
//    int                     offset  = AT91_PMC_PCER;

    if ( (gck->id!=SDHC0_PID) && (gck->id!=SDHC1_PID) )
    {
    __PRINTK(YEL,   "Enable generated clock for PID %u. GCKCSS = %u. GCKDIV = %u.\n",
                    gck->id, gck->clk_src, gck->gck_div);
    }
    spin_lock_irqsave(gck->lock, flags);
#if (1)
        regmap_write        (
                            gck->regmap,
                            AT91_PMC_PCR,
                            (gck->id & AT91_PMC_PCR_PID_MASK)
                            );
        regmap_update_bits  (
                            gck->regmap,
                            AT91_PMC_PCR,
                            AT91_PMC_PCR_GCKCSS_MASK          | AT91_PMC_PCR_GCKDIV_MASK          | AT91_PMC_PCR_CMD | AT91_PMC_PCR_GCKEN,
                            AT91_PMC_PCR_GCKCSS(gck->clk_src) | AT91_PMC_PCR_GCKDIV(gck->gck_div) | AT91_PMC_PCR_CMD | AT91_PMC_PCR_GCKEN
                            );
#else
        regmap_update_bits  (
                            gck->regmap,
                            AT91_PMC_PCR,
                            AT91_PMC_PCR_CMD,
                            AT91_PMC_PCR_CMD
                            );
        regmap_update_bits  (
                            gck->regmap,
                            AT91_PMC_PCR,
                            AT91_PMC_PCR_PID_MASK             | AT91_PMC_PCR_CMD,
                            (gck->id & AT91_PMC_PCR_PID_MASK) | AT91_PMC_PCR_CMD
                            );
        regmap_update_bits  (
                            gck->regmap,
                            AT91_PMC_PCR,
                            AT91_PMC_PCR_GCKDIV_MASK          | AT91_PMC_PCR_GCKCSS_MASK          | AT91_PMC_PCR_CMD | AT91_PMC_PCR_GCKEN | AT91_PMC_PCR_EN,
                            AT91_PMC_PCR_GCKDIV(gck->gck_div) | AT91_PMC_PCR_GCKCSS(gck->clk_src) | AT91_PMC_PCR_CMD | AT91_PMC_PCR_GCKEN | 0
                            );
#if (1)
    #define PMC_PCER0_ID_MIN        2
    #define PMC_PCER0_ID_MAX        31
    #define PERIPHERAL_MASK(id)     (1 << ((id) & PMC_PCER0_ID_MAX))

            if (gck->id < PMC_PCER0_ID_MIN) { goto exit; }
            if (gck->id > PMC_PCER0_ID_MAX) { offset = AT91_PMC_PCER1; }
            __PRINTK(GRN, "Enable clock. REG = 0x%x\n", offset);
            regmap_write    (
                            gck->regmap,
                            offset,
                            PERIPHERAL_MASK(gck->id)
                            );
    exit:
#endif

#endif

    spin_unlock_irqrestore(gck->lock, flags);
    return 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

static void                     clk_generated_disable(struct clk_hw *hw)
{
    struct clk_generated *gck = to_clk_generated(hw);
    unsigned long flags;
    
    if ( (gck->id!=SDHC0_PID) && (gck->id!=SDHC1_PID) )
    {
        __PRINTK(RED,   "Disable generated clock for PID %u.\n",
                        gck->id);
    }
    spin_lock_irqsave(gck->lock, flags);
        regmap_write        (
                            gck->regmap,
                            AT91_PMC_PCR,
                            (gck->id & AT91_PMC_PCR_PID_MASK)
                            );
        regmap_update_bits  (
                            gck->regmap,
                            AT91_PMC_PCR,
                            AT91_PMC_PCR_CMD | AT91_PMC_PCR_GCKEN ,
                            AT91_PMC_PCR_CMD | 0                    
                            );
    spin_unlock_irqrestore(gck->lock, flags);
}
/*--- End of the function ------------------------------------------------------------------------*/

static int                      clk_generated_is_enabled(struct clk_hw *hw)
{
    struct clk_generated *gck = to_clk_generated(hw);
    unsigned long flags;
    unsigned int status;

    spin_lock_irqsave(gck->lock, flags);
        regmap_write    (
                        gck->regmap,
                        AT91_PMC_PCR,
                        (gck->id & AT91_PMC_PCR_PID_MASK)
                        );
        regmap_read     (
                        gck->regmap,
                        AT91_PMC_PCR,
                        &status
                        );
    spin_unlock_irqrestore(gck->lock, flags);

    return status & AT91_PMC_PCR_GCKEN ? 1 : 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

static unsigned long            clk_generated_recalc_rate   (
                                                            struct clk_hw * hw,
                                                            unsigned long   parent_rate
                                                            )
{
    struct clk_generated *  gck         = to_clk_generated(hw);
    unsigned long           recalc_rate = DIV_ROUND_CLOSEST(parent_rate, gck->gck_div + 1);
#if DEBUG_GEN_CLK_SEVERE
    __PRINTK(CYN,   "Recalculated rate for PID %u is : %lu\n",
                    gck->id, recalc_rate);
#endif
    return recalc_rate;
}

static void clk_generated_best_diff (
                                    struct clk_rate_request *   req,
                                    struct clk_hw *             parent,
                                    unsigned long               parent_rate, 
                                    u32                         div,
                                    int *                       best_diff, 
                                    long *                      best_rate
                                    )
{
	unsigned long   tmp_rate;
	int             tmp_diff;

	if (!div)
    {
		tmp_rate = parent_rate;
    }
	else
    {
		tmp_rate = parent_rate / div;
    }
	tmp_diff = abs(req->rate - tmp_rate);

	if (*best_diff < 0 || *best_diff > tmp_diff) {
		*best_rate = tmp_rate;
		*best_diff = tmp_diff;
		req->best_parent_rate = parent_rate;
		req->best_parent_hw = parent;
	}
}


static int                      clk_generated_determine_rate    (
                                                                struct clk_hw *             hw,
                                                                struct clk_rate_request *   req
                                                                )
{
    struct clk_generated *  gck         = to_clk_generated(hw);
    struct clk_hw *         parent      = NULL;
    long                    best_rate   = -EINVAL;
    unsigned long           min_rate;
    int                     best_diff   = -1;
    int                     i;

    __PRINTK(YEL,   "Given : PID %u. required_rate = %lu.\n",
                    gck->id, req->rate);

    for (i = 0; i < clk_hw_get_num_parents(hw); i++) {
        u32 div;
        unsigned long parent_rate;

        parent = clk_hw_get_parent_by_index(hw, i);
        if (!parent)
        {
            __PRINTK(RED, "ERROR : No parent with index %i.\n", i);
            continue;
        }
        
        parent_rate = clk_hw_get_rate(parent);
#if DEBUG_GEN_CLK_SEVERE
        __PRINTK(MAG,   "Looking for parent %s|%i|%lu.\n", 
                        __clk_get_name((parent)->clk), i, parent_rate);
#endif
        min_rate = DIV_ROUND_CLOSEST(parent_rate, GENERATED_MAX_DIV + 1);
        if (!parent_rate || (gck->range.max && min_rate > gck->range.max))
        {
            __PRINTK(CYN,   "Parent %s|%i|%lu is not fit. min_rate = %lu (GCKDIV=0xff).\n", 
                            __clk_get_name((parent)->clk), i, parent_rate, min_rate);
            continue;
        }

        div = DIV_ROUND_CLOSEST(parent_rate, req->rate);
		clk_generated_best_diff(
                                req, 
                                parent, 
                                parent_rate, 
                                div,
                                &best_diff, 
                                &best_rate
                                );

		if (!best_diff) break;
    } //end for


    __PRINTK(CYN,   "best_rate = %ld, parent %s|%lu.\n",
                    best_rate,
                    __clk_get_name((req->best_parent_hw)->clk), req->best_parent_rate);

    if (best_rate < 0) { return best_rate; }
    req->rate = best_rate;
    return 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

/* No modification of hardware as we have the flag CLK_SET_PARENT_GATE set */
static int                      clk_generated_set_parent    (
                                                            struct clk_hw * hw,
                                                            u8              index
                                                            )
{
    struct clk_generated * gck = to_clk_generated(hw);
    if (index >= clk_hw_get_num_parents(hw))
    {
        __PRINTK(RED,   "ERROR : Asked for parent index %u. Number of parents %i.\n",
                        index, clk_hw_get_num_parents(hw));
        return -EINVAL;
    }
    gck->clk_src = index;
    __PRINTK(CYN, "Clock source is set to : %i.\n", index);
    return 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

static u8                       clk_generated_get_parent(struct clk_hw * hw)
{
    struct clk_generated * gck = to_clk_generated(hw);
    return gck->clk_src;
}
/*--- End of the function ------------------------------------------------------------------------*/

/* No modification of hardware as we have the flag CLK_SET_RATE_GATE set */
static int                      clk_generated_set_rate  (
                                                        struct clk_hw * hw,
                                                        unsigned long   required_rate,
                                                        unsigned long   parent_rate
                                                        )
{
    struct clk_generated *  gck = to_clk_generated(hw);
    u32                     div;
    __PRINTK(YEL,   "Given : PID %u. parent_rate = %lu. required_rate = %lu.\n",
                    gck->id, parent_rate, required_rate);
    if (!required_rate) { return -EINVAL; }

    if (gck->range.max 
        && required_rate > gck->range.max)
    {
        __PRINTK(RED,   "ERROR : Required clock(%lu) out of range(%lu).\n",
                        required_rate, gck->range.max);
        return -EINVAL;
    }

    div = DIV_ROUND_CLOSEST(parent_rate, required_rate);
    if (div > GENERATED_MAX_DIV + 1 || !div) {
        __PRINTK(RED,   "ERROR : Calculated GCKDIV(%u) out of range(%u).\n",
                        div, GENERATED_MAX_DIV);
        return -EINVAL;
    }
    gck->gck_div = div - 1;
    __PRINTK(GRN,   "PID %u. Calculated GCKDIV = %u.\n",
                    gck->id, gck->gck_div);
    return 0;
}

static const struct clk_ops     generated_ops = {
    .enable         = clk_generated_enable,
    .disable        = clk_generated_disable,
    .is_enabled     = clk_generated_is_enabled,
    .recalc_rate    = clk_generated_recalc_rate,
    .determine_rate = clk_generated_determine_rate,
    .get_parent     = clk_generated_get_parent,
    .set_parent     = clk_generated_set_parent,
    .set_rate       = clk_generated_set_rate,
};
/*--- End of the function ------------------------------------------------------------------------*/

/*!
 * clk_generated_startup.
 * Initialize a given clock to its default parent and divisor parameter.
 * @gck: Generated clock to set the startup parameters for.
 * Take parameters from the hardware
 * and update local clock configuration accordingly.
 */
static void                     clk_generated_startup   (struct clk_generated * gck)
{
    u32 tmp;
    unsigned long flags;
#if DEBUG_GEN_CLK_SEVERE
    __PRINTK(CYN, "Starting clock for PID : %u.\n", gck->id);
#endif
    spin_lock_irqsave(gck->lock, flags);
        regmap_write    (
                        gck->regmap,
                        AT91_PMC_PCR,
                        (gck->id & AT91_PMC_PCR_PID_MASK)
                        );
        regmap_read     (gck->regmap,
                        AT91_PMC_PCR,
                        &tmp
                        );
    spin_unlock_irqrestore(gck->lock, flags);

    gck->clk_src = (tmp & AT91_PMC_PCR_GCKCSS_MASK) >> AT91_PMC_PCR_GCKCSS_OFFSET;
    gck->gck_div = (tmp & AT91_PMC_PCR_GCKDIV_MASK) >> AT91_PMC_PCR_GCKDIV_OFFSET;

}
/*--- End of the function ------------------------------------------------------------------------*/

static struct clk_hw * __init
at91_clk_register_generated(
                            struct regmap *             regmap,
                            spinlock_t *                lock,
                            const char *                name,
                            const char **               parent_names,
                            u8                          num_parents,
                            u8                          id,
                            const struct clk_range *    range
                            )
{
    struct clk_generated *  gck;
    struct clk_init_data    init;
    struct clk_hw *         hw;
    int                     ret;

    gck = kzalloc(sizeof(*gck), GFP_KERNEL);
    if (!gck) { return ERR_PTR(-ENOMEM); }

    init.name           = name;
    init.ops            = &generated_ops;
    init.parent_names   = parent_names;
    init.num_parents    = num_parents;
    init.flags = CLK_SET_RATE_GATE | CLK_SET_PARENT_GATE;

    gck->id         = id;
    gck->hw.init    = &init;
    gck->regmap     = regmap;
    gck->lock       = lock;
    gck->range      = *range;

    hw = &gck->hw;
    ret = clk_hw_register(NULL, &gck->hw);
    if (ret) {
        kfree(gck);
        hw = ERR_PTR(ret);
    } else {
        clk_generated_startup(gck);
    }
    return hw;
}
/*--- End of the function ------------------------------------------------------------------------*/

static void __init              of_sama5d2_clk_generated_setup(struct device_node *np)
{
    int                     num;
    u32                     id;
    const char *            name;
    struct clk_hw *         hw;
    unsigned int            num_parents;
    const char *            parent_names[GENERATED_SOURCE_MAX];
    struct device_node *    gck_np;
    struct clk_range        range = CLK_RANGE(0, 0);
    struct regmap *         regmap;
//    u32                     clk_src = 1;

    __PRINTK(MAG, "+++ Initializing generated clock +++\n");

    num_parents = of_clk_get_parent_count(np);
    if (num_parents == 0 || num_parents > GENERATED_SOURCE_MAX){
        return;
    }

    /* Fill @parent_names with names of @np's parents and return number of parents*/
    of_clk_parent_fill(np, parent_names, num_parents);

    num = of_get_child_count(np);
    if (!num || num > PERIPHERAL_MAX)
        return;

    regmap = syscon_node_to_regmap(of_get_parent(np));
    if (IS_ERR(regmap)) { return; }

    for_each_child_of_node(np, gck_np) {
        if (of_property_read_u32(gck_np, "reg", &id))
            continue;



        if (id < PERIPHERAL_ID_MIN || id >= PERIPHERAL_MAX)
            continue;

        if (of_property_read_string (
                                    np,
                                    "clock-output-names",
                                    &name
                                    ))
        {
            name = gck_np->name;
        }
        range.max = 0; range.min = 0;
        of_at91_get_clk_range(
                            gck_np,
                            "atmel,clk-output-range",
                            &range
                            );
#if DEBUG_GEN_CLK
        __PRINTK(CYN,   "Registering clock : \n"
                        "%49s name = %s. PID = %u. num_parents = %u. max = %lu\n",
                        "", name, id, num_parents, range.max);
#endif
        hw = at91_clk_register_generated(
                                        regmap,
                                        &pmc_pcr_lock,
                                        name,
                                        parent_names,
                                        num_parents,
                                        id,
                                        &range
                                        );
        if (IS_ERR(hw)){
        __PRINTK(RED,   "Can't register clock : \n"
                        "%49s name = %s. num_parents = %u. id = %iu.\n",
                        "", name, num_parents, id);
            continue;
        }

        of_clk_add_hw_provider(gck_np, of_clk_hw_simple_get, hw);
    } //end for_each_child_of_node
}
/*--- End of the function ------------------------------------------------------------------------*/

CLK_OF_DECLARE  (
                of_sama5d2_clk_generated_setup,
                "atmel,sama5d2-clk-generated",
                of_sama5d2_clk_generated_setup
                );
