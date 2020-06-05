/* microElk */
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/clkdev.h>
#include <linux/clk/at91_pmc.h>
#include <linux/of.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>
#include "pmc.h"

#define AUDIO_PLL_FOUT_MIN              620000000
#define AUDIO_PLL_FOUT_MAX              700000000
#if (0)
# define AUDIO_PLL_REFERENCE_FOUT    660000000
#else
# define AUDIO_PLL_REFERENCE_FOUT    54 * 12288 * 1000
#endif
#define AUDIO_PLL_QDPMC_MAX             (AT91_PMC_AUDIO_PLL_QDPMC_MASK >> AT91_PMC_AUDIO_PLL_QDPMC_OFFSET)

struct clk_audio_pmc {
    struct clk_hw hw;
    struct regmap *regmap;
    u8 QDPMC;
};

#define to_clk_audio_pmc(hw) container_of(hw, struct clk_audio_pmc, hw)

static int              clk_apll_pmc_enable(struct clk_hw * hw)
{
    struct clk_audio_pmc * apmc_ck = to_clk_audio_pmc(hw);
    __PRINTK(YEL""BOLD, "Enable audio PLL PMC with QDPMC = %i.\n", apmc_ck->QDPMC);
#if (1)
    regmap_update_bits      (
                            apmc_ck->regmap,
                            AT91_PMC_AUDIO_PLL0,
                            AT91_PMC_AUDIO_PLL_PMCEN | AT91_PMC_AUDIO_PLL_QDPMC_MASK,
                            AT91_PMC_AUDIO_PLL_PMCEN | AT91_PMC_AUDIO_PLL_QDPMC(apmc_ck->QDPMC)
                            );
#else
    regmap_update_bits      (
                            apmc_ck->regmap,
                            AT91_PMC_AUDIO_PLL0,
                            AT91_PMC_AUDIO_PLL_QDPMC_MASK,
                            AT91_PMC_AUDIO_PLL_QDPMC(apmc_ck->QDPMC)
                            );
    regmap_update_bits      (
                            apmc_ck->regmap,
                            AT91_PMC_AUDIO_PLL0,
                            AT91_PMC_AUDIO_PLL_PMCEN,
                            AT91_PMC_AUDIO_PLL_PMCEN
                            );
#endif
    return 0;
}

static void             clk_apll_pmc_disable(struct clk_hw *hw)
{
    struct clk_audio_pmc *apmc_ck = to_clk_audio_pmc(hw);
    __PRINTK(RED, "Disable audio PLL PMC.\n");
    regmap_update_bits      (
                            apmc_ck->regmap,
                            AT91_PMC_AUDIO_PLL0,
                            AT91_PMC_AUDIO_PLL_PMCEN,
                            0
                            );
}

static unsigned long    clk_apll_pmc_recalc_rate   (
                                                        struct clk_hw * hw,
                                                        unsigned long   parent_rate
                                                        )
{
    struct clk_audio_pmc *  apmc_ck   = to_clk_audio_pmc(hw);
    unsigned long           f_AUDIOPLLCLK = 0;
#if DEBUG_APMCCLK_SEVERE
    __PRINTK(YEL, "Given : new parent_rate = %lu.\n", parent_rate);
#endif
    f_AUDIOPLLCLK = parent_rate / (apmc_ck->QDPMC + 1);
    __PRINTK(GRN""BOLD, "f_AUDIOPLLCLK(%lu) = parent_rate(%lu)/(QDPMC(%u) + 1).\n",
                        f_AUDIOPLLCLK, parent_rate, apmc_ck->QDPMC);
    return f_AUDIOPLLCLK;
}
/*--- End of the function ------------------------------------------------------------------------*/

/*! f_AUDIOPLLCLK = f_AUDIOCORECLK/(QDPMC + 1)
 * QDPMC = f_AUDIOCORECLK/f_AUDIOPLLCLK - 1
 * look #clk_apll_pmc_set_rate for '-1'
 */
static int              clk_apll_compute_QDPMC     (
                                                        unsigned long   parent_rate,
                                                        unsigned long   required_rate,
                                                        unsigned long * new_QDPMC //return value
                                                        )
{
    unsigned long tmp_QDPMC;
#if DEBUG_APMCCLK_SEVERE
    __PRINTK(YEL,   "Given : parent_rate = %lu. required_rate = %lu.\n",
                    parent_rate, required_rate);
#endif
    if (!required_rate)
    {
        __PRINTK(RED, "ERROR : required_rate = %lu.\n", required_rate);
        return -EINVAL;
    }
    tmp_QDPMC = parent_rate / required_rate;
    if (!tmp_QDPMC || tmp_QDPMC > AUDIO_PLL_QDPMC_MAX)
    {
        __PRINTK(RED, "ERROR : new QDPMC = %lu.\n", tmp_QDPMC - 1);
        return -EINVAL;
    }
    *new_QDPMC = tmp_QDPMC;
#if DEBUG_APMCCLK_SEVERE
    __PRINTK(GRN""BOLD, "New QDPMC = %lu.\n", *new_QDPMC - 1);
#endif
    return 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

static long             clk_apll_pmc_round_rate    (
                                                        struct clk_hw * hw,
                                                        unsigned long   required_rate,
                                                        unsigned long * parent_rate
                                                        )
{
    struct clk_hw *     pclk                = clk_hw_get_parent(hw);
    long                best_rate           = -EINVAL;
    unsigned long       best_parent_rate    = 0;
    unsigned long       tmp_QDPMC;
#if DEBUG_APMCCLK_SEVERE
    __PRINTK(YEL,   "Given : required_rate = %lu. parent_rate = %lu.\n",
                    required_rate, *parent_rate);
#endif
    ///You're calculating your divisor based on the AUDIO_PLL_REFERENCE_FOUT value.
    if (clk_apll_compute_QDPMC (
                                    /* *parent_rate, */
                                    AUDIO_PLL_REFERENCE_FOUT,
                                    required_rate,
                                    &tmp_QDPMC))
    {
        __PRINTK(RED, "ERROR\n");
        return -EINVAL;
    }
    ///And then asking your parent to adapt its rate based on the returned divisor.
    best_parent_rate    = clk_hw_round_rate(pclk, required_rate * tmp_QDPMC);
    best_rate           = best_parent_rate / tmp_QDPMC;
#if DEBUG_APMCCLK_SEVERE
    __PRINTK(GRN,   "best_rate = %ld. best_parent_rate = %lu. qd=%lu.\n",
                    best_rate, best_parent_rate, tmp_QDPMC - 1);
#endif
    *parent_rate = best_parent_rate;
    return best_rate;
}
/*--- End of the function ------------------------------------------------------------------------*/

static int              clk_apll_pmc_set_rate      (
                                                        struct clk_hw * hw,
                                                        unsigned long   required_rate,
                                                        unsigned long   parent_rate
                                                        )
{


    struct clk_audio_pmc *  apmc_ck = to_clk_audio_pmc(hw);
    unsigned long           tmp_QDPMC;
    int                     ret;
    __PRINTK(YEL,   "Set new f_AUDIOPLLCLK = %lu. parent_rate = %lu.\n",
                    required_rate, parent_rate);
    ret = clk_apll_compute_QDPMC    (
                                    parent_rate,
                                    required_rate,
                                    &tmp_QDPMC
                                    );

    /// -1 implemented here, never before
    if (!ret)   { apmc_ck->QDPMC = tmp_QDPMC - 1; }
    else        { __PRINTK(RED, "Can't compute QDPMC. %i returned.\n", ret); }
    return ret;
}
/*--- End of the function ------------------------------------------------------------------------*/

static const struct clk_ops audio_pll_pmc_ops = {
    .enable         = clk_apll_pmc_enable,
    .disable        = clk_apll_pmc_disable,
    .recalc_rate    = clk_apll_pmc_recalc_rate,
    .round_rate     = clk_apll_pmc_round_rate,
    .set_rate       = clk_apll_pmc_set_rate,
};
/*--- End of the function ------------------------------------------------------------------------*/

static void __init of_sama5d2_clk_apll_pmc_setup(struct device_node *np)
{
    struct clk_audio_pmc *  apmc_ck;
    struct clk_init_data    init;
    struct regmap *         regmap;
    const char *            parent_name;
    const char *            name        = np->name;
    int                     ret;
    __PRINTK(MAG, "+++ Initializing audio PLL PMC +++\n");
    parent_name = of_clk_get_parent_name(np, 0);

    of_property_read_string(np, "clock-output-names", &name);

    regmap = syscon_node_to_regmap(of_get_parent(np));
    if (IS_ERR(regmap)) return;

    apmc_ck = kzalloc(sizeof(*apmc_ck), GFP_KERNEL);
    if (!apmc_ck)       return;

    init.name           = name;
    init.ops            = &audio_pll_pmc_ops;
    init.parent_names   = (parent_name ? &parent_name : NULL);
    init.num_parents    = 1;
    init.flags          = CLK_SET_RATE_GATE | CLK_SET_PARENT_GATE | CLK_SET_RATE_PARENT;

    apmc_ck->hw.init    = &init;
    apmc_ck->regmap     = regmap;

    ret = clk_hw_register(NULL, &apmc_ck->hw);
    if (ret)    { kfree(apmc_ck); }
    else        { of_clk_add_hw_provider(np, of_clk_hw_simple_get, &apmc_ck->hw); }
    return;
}
/*--- End of the function ------------------------------------------------------------------------*/

CLK_OF_DECLARE( of_sama5d2_clk_apll_pmc_setup,
                "atmel,sama5d2-clk-audio-pll-pmc",
                of_sama5d2_clk_apll_pmc_setup);
