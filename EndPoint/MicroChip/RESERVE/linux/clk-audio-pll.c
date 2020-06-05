/* microElk */
#include <linux/clk-provider.h>
#include <linux/clkdev.h>
#include <linux/clk/at91_pmc.h>
#include <linux/of.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>

#include "pmc.h"

/*
 * DOC: Fractional PLL clock for audio
 *
 * Traits of this clock:
 * prepare - clk_prepare puts audio PLL in reset state
 * enable - clk_enable writes nd, fracr parameters and enables PLL
 * rate - rate is adjustable.
 *        clk->rate = parent->rate * ((nd + 1) + (fracr / 2^22))
 * parent - fixed parent.  No clk_set_parent support
 */

#define AUDIO_PLL_DIV_FRAC  (1 << 22)
#define AUDIO_PLL_ND_MAX    (AT91_PMC_AUDIO_PLL_ND_MASK >> AT91_PMC_AUDIO_PLL_ND_OFFSET)

struct clk_audio_frac {
    struct clk_hw hw;
    struct regmap *regmap;
    spinlock_t *lock;
    u32 fracr;
    u8 nd;
};

#define to_clk_audio_frac(hw) container_of(hw, struct clk_audio_frac, hw)

/* make sure that pll is in reset state beforehand */
static int              clk_apll_prepare(struct clk_hw *hw)
{
    struct clk_audio_frac *fck = to_clk_audio_frac(hw);
    __PRINTK(YEL""BOLD, "Reset audio PLL.\n");
    regmap_update_bits(
                        fck->regmap,
                        AT91_PMC_AUDIO_PLL0,
                        AT91_PMC_AUDIO_PLL_RESETN,
                        0
                        );
    return 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

static void             clk_apll_unprepare(struct clk_hw *hw)
{
    clk_apll_prepare(hw);
}
/*--- End of the function ------------------------------------------------------------------------*/

static int              clk_apll_enable(struct clk_hw *hw)
{
    struct clk_audio_frac *fck = to_clk_audio_frac(hw);
    unsigned long flags;
    __PRINTK(YEL""BOLD, "Enable audio PLL with ND = %i and FRACR = %u.\n",
                        fck->nd, fck->fracr);
    spin_lock_irqsave(fck->lock, flags);
        regmap_update_bits  (
                            fck->regmap,
                            AT91_PMC_AUDIO_PLL0,
                            AT91_PMC_AUDIO_PLL_RESETN,
                            AT91_PMC_AUDIO_PLL_RESETN
                            );

        regmap_update_bits  (
                            fck->regmap,
                            AT91_PMC_AUDIO_PLL1,
                            AT91_PMC_AUDIO_PLL_FRACR_MASK,
                            fck->fracr);
        /* reset and enable have to be done in 2 separated writes for AT91_PMC_AUDIO_PLL0 */
#if (1)
        regmap_update_bits  (fck->regmap,
                            AT91_PMC_AUDIO_PLL0,
                            AT91_PMC_AUDIO_PLL_PLLEN | AT91_PMC_AUDIO_PLL_ND_MASK,
                            AT91_PMC_AUDIO_PLL_PLLEN | AT91_PMC_AUDIO_PLL_ND(fck->nd)
                            );
#else
        regmap_update_bits  (fck->regmap,
                            AT91_PMC_AUDIO_PLL0,
                            AT91_PMC_AUDIO_PLL_ND_MASK,
                            AT91_PMC_AUDIO_PLL_ND(fck->nd)
                            );
        regmap_update_bits  (fck->regmap,
                            AT91_PMC_AUDIO_PLL0,
                            AT91_PMC_AUDIO_PLL_PLLEN,
                            AT91_PMC_AUDIO_PLL_PLLEN
                            );
#endif
    spin_unlock_irqrestore(fck->lock, flags);
    return 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

static void             clk_apll_disable(struct clk_hw *hw)
{
    struct clk_audio_frac * fck     = to_clk_audio_frac(hw);
    unsigned long           flags;
    __PRINTK(RED, "Disable audio PLL.\n");
    spin_lock_irqsave(fck->lock, flags);
        regmap_update_bits(
                            fck->regmap,
                            AT91_PMC_AUDIO_PLL0,
                            AT91_PMC_AUDIO_PLL_PLLEN,
                            0);
        /* do it in 2 separated writes */
        regmap_update_bits(
                            fck->regmap,
                            AT91_PMC_AUDIO_PLL0,
                            AT91_PMC_AUDIO_PLL_RESETN,
                            0);
    spin_unlock_irqrestore(fck->lock, flags);
}
/*--- End of the function ------------------------------------------------------------------------*/

static unsigned long    calc_f_AUDIOCORECLK (
                                            unsigned long parent_rate,
                                            unsigned long ND,
                                            unsigned long FRACR
                                            )
{
    unsigned long       f_AUDIOCORECLK;
    unsigned long long  fr = (unsigned long long)parent_rate * (unsigned long long)FRACR;
#if DEBUG_ACORECLK_SEVERE
    __PRINTK(YEL,   "Given : parent_rate = %lu. ND = %lu. FRACR = %lu.\n",
                    parent_rate, ND, FRACR);
#endif
                        fr = DIV_ROUND_CLOSEST_ULL(fr, AUDIO_PLL_DIV_FRAC);
#if DEBUG_ACORECLK_SEVERE
    __PRINTK(CYN,   "fr(%llu) = (parent_rate(%lu) * FRACR(%lu))/(1>>22).\n",
                    fr, parent_rate, FRACR);
#endif
    f_AUDIOCORECLK = parent_rate * (ND + 1) + fr;
#if DEBUG_ACORECLK_SEVERE
    __PRINTK(CYN,   "New audio PLL core frequency : \n"
                    "%49s f_AUDIOCORECLK(%lu) = parent_rate(%lu) * (ND(%lu) + 1) + fr(%llu).\n",
                    "", f_AUDIOCORECLK, parent_rate, ND, fr);
#endif
    return f_AUDIOCORECLK;
}
/*--- End of the function ------------------------------------------------------------------------*/

static unsigned long    clk_apll_recalc_rate    (
                                                struct clk_hw * hw,
                                                unsigned long   parent_rate
                                                )
{
    struct clk_audio_frac * fck = to_clk_audio_frac(hw);
    unsigned long           f_AUDIOCORECLK;
#if DEBUG_ACORECLK_SEVERE
    __PRINTK(YEL,   "Given : parent_rate = %lu.\n", parent_rate);
#endif
    f_AUDIOCORECLK = calc_f_AUDIOCORECLK(parent_rate, fck->nd, fck->fracr);
    __PRINTK(GRN""BOLD, "f_AUDIOCORECLK = %lu. ND = %u. FRACR = %lu\n",
                        f_AUDIOCORECLK, fck->nd, (unsigned long)fck->fracr);
    return f_AUDIOCORECLK;
}
/*--- End of the function ------------------------------------------------------------------------*/

/*! f_AUDIOCORECLK = f_ref(ND + 1 + FRACR/2^22)
 * Two unknown ND and FRACR.
 * ND proposed as :
 * ND = f_AUDIOCORECLK/f_ref - 1 ,
 * so :
 * f_AUDIOCORECLK = (ND + 1)*f_ref + f_AUDIOCORECLK%f_ref,
 * and :
 * f_AUDIOCORECLK = f_ref*{ ND + 1 + (f_AUDIOCORECLK%f_ref)/f_ref },
 * so :
 * FRACR/2^22 = (f_AUDIOCORECLK%f_ref)/f_ref
 * FRACR = {(f_AUDIOCORECLK%f_ref)/f_ref} * 2^22
 * -------------------------------------------------------------------------------------------------
 * f_AUDIOCORECLK = f_ref(ND + 1 + FRACR/2^22),
 * FRACR = (f_AUDIOCORECLK/f_ref - ND - 1)2^22,
 * if :
 * ND = f_AUDIOCORECLK/f_ref - 1,
 * FRACR = {(f_AUDIOCORECLK%f_ref)/f_ref} * 2^22
 */
static int              calc_FRACR_and_ND   (
                                            unsigned long required_rate,
                                            unsigned long parent_rate,
                                            unsigned long * ND,     //ret val 1
                                            unsigned long * FRACR   //ret val 2
                                            )
{
    unsigned long long tmp;
    unsigned long long rem;
#if DEBUG_ACORECLK_SEVERE
    __PRINTK(YEL,   "Given : required_rate = %lu. parent_rate = %lu.\n",
                    required_rate, parent_rate);
#endif
    if (!required_rate) { return -EINVAL; }

    tmp = required_rate;
    /*!
        # define do_div(n, base) ({						        \
            unsigned int __base = (base);					    \
            unsigned int __rem;						            \
            __rem = ((unsigned long long)(n)) % __base;			\
            (n) = ((unsigned long long)(n)) / __base;			\
            __rem;								                \
        })
     */
    rem = do_div(tmp, parent_rate);                 /// -# f_AUDIOCORECLK/f_ref = ND + 1 + FRACR/2^22
    if (tmp == 0 || (tmp - 1) > AUDIO_PLL_ND_MAX)
    {
        __PRINTK(RED,   "ERROR : required_rate(%lu)/parent_rate(%lu) = %llu.\n"
                        "%49s AUDIO_PLL_ND_MAX = %d.\n",
                        required_rate, parent_rate, tmp,
                        "", AUDIO_PLL_ND_MAX);
        return -EINVAL;
    } //end if (tmp == 0 || (tmp - 1) > AUDIO_PLL_ND_MAX)
#if DEBUG_ACORECLK_SEVERE
    else {
        __PRINTK(CYN,   "required_rate(%lu)/parent_rate(%lu) = (%llu).\n"
                        "%49s reminder = %llu\n",
                        required_rate, parent_rate, tmp,
                        "", rem);
    } //end if (tmp == 0 || (tmp - 1) > AUDIO_PLL_ND_MAX)
#endif
    *ND = tmp - 1;                                  ///ND = f_AUDIOCORECLK/f_ref - 1
#if DEBUG_ACORECLK_SEVERE
    __PRINTK(GRN,   "New  ND(%lu) = required_rate(%lu)/parent_rate(%lu) - 1.\n",
                    *ND, required_rate, parent_rate);
#endif
    tmp = rem * AUDIO_PLL_DIV_FRAC /* 1<<22 */;
    /* Divide positive or negative dividend
     * by positive or negative divisor
     * and round to closest integer.
     */
    tmp = DIV_ROUND_CLOSEST_ULL(tmp, parent_rate);  ///FRACR = (f_AUDIOCORECLK%f_ref) * (1<<22) / f_ref
    if (tmp > AT91_PMC_AUDIO_PLL_FRACR_MASK){
        __PRINTK(RED,   "ERROR : new  FRACR(%llu) > %d.\n",
                        tmp, AT91_PMC_AUDIO_PLL_FRACR_MASK);
        return -EINVAL;
    }
    /* we can cast here as we verified the bounds just above */
    *FRACR = (unsigned long)tmp;
#if DEBUG_ACORECLK_SEVERE
    __PRINTK(GRN, "New  FRACR = %lu. New ND = %lu\n", *FRACR, *ND);
#endif
    return 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

static long             clk_apll_round_rate     (
                                                struct clk_hw * hw,
                                                unsigned long   required_rate,
                                                unsigned long * parent_rate
                                                )
{
    long            f_AUDIOCORECLK = -EINVAL;
    unsigned long   FRACR;
    unsigned long   ND;
    int             ret;
#if DEBUG_ACORECLK_SEVERE
    __PRINTK(YEL,   "Given : required_rate = %lu. parent_rate = %lu\n",
                    required_rate, *parent_rate);
#endif
    ret = calc_FRACR_and_ND(
                                    required_rate,
                                    *parent_rate,
                                    &ND,            //Return value
                                    &FRACR          //Return value
                                    );
    if (ret)
    {
        __PRINTK(RED,"ERROR: Can't calculate FRACR and ND.\n");
        return ret;
    }

    f_AUDIOCORECLK = calc_f_AUDIOCORECLK(*parent_rate, ND, FRACR);

    __PRINTK(GRN""BOLD, "f_AUDIOCORECLK = %lu. ND = %lu. FRACR = %lu.\n",
                        f_AUDIOCORECLK, ND, FRACR);

    return f_AUDIOCORECLK;
}
/*--- End of the function ------------------------------------------------------------------------*/

static int              clk_apll_set_rate   (
                                            struct clk_hw * hw,
                                            unsigned long   required_rate,
                                            unsigned long   parent_rate
                                            )
{
    struct clk_audio_frac *     fck = to_clk_audio_frac(hw);
    unsigned long               FRACR;
    unsigned long               ND;
    int ret;

    __PRINTK(YEL,   "Set new f_AUDIOCORECLK = %lu. parent_rate : %lu.\n",
                    required_rate, parent_rate);

    ret = calc_FRACR_and_ND (
                            required_rate,
                            parent_rate,
                            &ND,
                            &FRACR
                            );
    if (ret)
    {
        __PRINTK(CYN,   "ERROR: Can't calculate new FRACR and ND for required frequency %lu.\n",
                        required_rate);
        return ret;
    }
    fck->nd     = ND;
    fck->fracr  = FRACR;
    __PRINTK(GRN""BOLD,   "New  FRACR = %lu. New ND = %lu.\n",
                    FRACR, ND);
    return 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

static const struct clk_ops audio_pll_ops = {
    .prepare        = clk_apll_prepare,
    .unprepare      = clk_apll_unprepare,
    .enable         = clk_apll_enable,
    .disable        = clk_apll_disable,
    .recalc_rate    = clk_apll_recalc_rate,
    .round_rate     = clk_apll_round_rate,
    .set_rate       = clk_apll_set_rate,
};
/*--- End of the function ------------------------------------------------------------------------*/

static void __init      of_sama5d2_clk_apll_setup(struct device_node *np)
{
    struct clk_audio_frac * fck;
    struct clk_init_data    init;
    struct regmap *         regmap;
    const char *            parent_name;
    const char *            name        = np->name;
    int ret;

    __PRINTK(MAG, "+++ Initializing audio PLL +++\n");
    parent_name = of_clk_get_parent_name(np, 0);

    of_property_read_string(np, "clock-output-names", &name);

    regmap = syscon_node_to_regmap(of_get_parent(np));
    if (IS_ERR(regmap))
        return;

    fck = kzalloc(sizeof(*fck), GFP_KERNEL);
    if (!fck)
        return;

    init.name           = name;
    init.ops            = &audio_pll_ops;
    init.parent_names   = (parent_name ? &parent_name : NULL);
    init.num_parents    = 1;
    init.flags          = CLK_SET_RATE_GATE;

    fck->hw.init = &init;
    fck->regmap = regmap;
    fck->lock = &pmc_pcr_lock;

    ret = clk_hw_register(NULL, &fck->hw);
    if (ret)    { kfree(fck); }
    else        { of_clk_add_hw_provider(np, of_clk_hw_simple_get, &fck->hw); }
    return;
}
CLK_OF_DECLARE( of_sama5d2_clk_apll_setup,
                "atmel,sama5d2-clk-audio-pll-frac",
                of_sama5d2_clk_apll_setup);
