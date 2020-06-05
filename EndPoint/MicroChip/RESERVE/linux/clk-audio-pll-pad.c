/* microElk */
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/clkdev.h>
#include <linux/clk/at91_pmc.h>
#include <linux/of.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>

#include "pmc.h"
/*------------------------------------------------------------------------------------------------*/
/*
 * DOC: PAD output for fractional PLL clock for audio
 *
 * Traits of this clock:
 * enable   - clk_enable writes QDAUDIO (which is ext_div and (div2,div3)), and enables PAD output
 * rate     - rate is adjustable. clk->rate = parent->rate / (ext_div * (div2,div3))
 * parent   - fixed parent. No clk_set_parent support
 */

#define AUDIO_PLL_FOUT_MIN          620000000
#define AUDIO_PLL_FOUT_MAX          700000000
#if (0)
# define AUDIO_PLL_REFERENCE_FOUT   660000000
#else
# define AUDIO_PLL_REFERENCE_FOUT   54 * 12288 * 1000
#endif

#define     AT91_PMC_AUDIO_PLL_DIV_OFFSET               24
#define     AT91_PMC_AUDIO_PLL_DIV_SIZE                 2
#define     AT91_PMC_AUDIO_PLL_DIV_MAX                  0x3
#define     AT91_PMC_AUDIO_PLL_DIV_MASK                 (0x3 << AT91_PMC_AUDIO_PLL_DIV_OFFSET)
#define     AT91_PMC_AUDIO_PLL_DIV(val)                 ((val) << AT91_PMC_AUDIO_PLL_DIV_OFFSET)

#define     AT91_PMC_AUDIO_PLL_QDAUDIO_OFFSET           26
#define     AT91_PMC_AUDIO_PLL_QDAUDIO_SIZE             5
#define     AT91_PMC_AUDIO_PLL_QDAUDIO_MAX              0x1f
#define     AT91_PMC_AUDIO_PLL_QDAUDIO_MASK             (0x1f << AT91_PMC_AUDIO_PLL_QDAUDIO_OFFSET)
#define     AT91_PMC_AUDIO_PLL_QDAUDIO(val)             ((val) << AT91_PMC_AUDIO_PLL_QDAUDIO_OFFSET)

#define     AT91_PMC_AUDIO_PLL_DIV_QDAUDIO_OFFSET       24
#define     AT91_PMC_AUDIO_PLL_DIV_QDAUDIO_SIZE         7
#define     AT91_PMC_AUDIO_PLL_DIV_QDAUDIO_MAX          0x7f
#define     AT91_PMC_AUDIO_PLL_DIV_QDAUDIO_MASK         (0x7f << AT91_PMC_AUDIO_PLL_DIV_QDAUDIO_OFFSET)
#define     AT91_PMC_AUDIO_PLL_DIV_QDAUDIO(val)         ((val) << AT91_PMC_AUDIO_PLL_DIV_QDAUDIO_OFFSET)

#define     AT91_PMC_AUDIO_PLL_DIVxQDAUDIO_MAX          AT91_PMC_AUDIO_PLL_QDAUDIO_MAX * AT91_PMC_AUDIO_PLL_DIV_MAX

#define to_DIV_QDAUDIO(DIV, QDAUDIO)        ( (DIV) | ((QDAUDIO) << AT91_PMC_AUDIO_PLL_DIV_SIZE) )

#define DIV_QDAUDIO_to_QDAUDIO(DIV_QDAUDIO) (                                                       \
                                            ((DIV_QDAUDIO) >> AT91_PMC_AUDIO_PLL_DIV_SIZE)          \
                                            & (AT91_PMC_AUDIO_PLL_QDAUDIO_MAX))

#define DIV_QDAUDIO_to_DIV(DIV_QDAUDIO)     (DIV_QDAUDIO & AT91_PMC_AUDIO_PLL_DIV_MAX)
/*------------------------------------------------------------------------------------------------*/

struct clk_audio_pad {
    struct clk_hw   hw;
    struct regmap * regmap;
    spinlock_t *    lock;
    u8              DIV_QDAUDIO;
};

#define to_clk_audio_pad(hw) container_of(hw, struct clk_audio_pad, hw)

static int              clk_apll_pad_enable(struct clk_hw *hw)
{
    struct clk_audio_pad * apad_ck = to_clk_audio_pad(hw);
    unsigned long flags;

    __PRINTK(YEL""BOLD, "Enable audio PLL PAD with :\n"
                        "%49s DIV = %u. QDAUDIO = %u. DIV_QDAUDIO = %u.\n",
                        "", DIV_QDAUDIO_to_DIV(apad_ck->DIV_QDAUDIO),
                        DIV_QDAUDIO_to_QDAUDIO(apad_ck->DIV_QDAUDIO),
                        apad_ck->DIV_QDAUDIO);

    spin_lock_irqsave(apad_ck->lock, flags);
#if (1)
        /// Write DIV and QDAUTIO together.
        regmap_update_bits  (
                            apad_ck->regmap,                                        //map
                            AT91_PMC_AUDIO_PLL1,                                    //reg
                            AT91_PMC_AUDIO_PLL_DIV_QDAUDIO_MASK,                    //mask
                            AT91_PMC_AUDIO_PLL_DIV_QDAUDIO(apad_ck->DIV_QDAUDIO)    //val
                            );
#else
        regmap_update_bits  (
                            apad_ck->regmap,
                            AT91_PMC_AUDIO_PLL1,
                            AT91_PMC_AUDIO_PLL_DIV_MASK,
                            AT91_PMC_AUDIO_PLL_DIV(DIV_QDAUDIO_to_DIV(apad_ck->DIV_QDAUDIO))
                            );
        regmap_update_bits  (
                            apad_ck->regmap,
                            AT91_PMC_AUDIO_PLL1,
                            AT91_PMC_AUDIO_PLL_QDAUDIO_MASK,
                            AT91_PMC_AUDIO_PLL_QDAUDIO(DIV_QDAUDIO_to_QDAUDIO(apad_ck->DIV_QDAUDIO))
                            );
#endif
        regmap_update_bits  (
                            apad_ck->regmap,
                            AT91_PMC_AUDIO_PLL0,
                            AT91_PMC_AUDIO_PLL_PADEN,
                            AT91_PMC_AUDIO_PLL_PADEN
                            );
    spin_unlock_irqrestore(apad_ck->lock, flags);
    return 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

static void             clk_apll_pad_disable(struct clk_hw * hw)
{
    struct clk_audio_pad * apad_ck = to_clk_audio_pad(hw);
    __PRINTK(RED, "Disable audio PLL PAD.\n");
    regmap_update_bits  (
                        apad_ck->regmap,
                        AT91_PMC_AUDIO_PLL0,
                        AT91_PMC_AUDIO_PLL_PADEN,
                        0
                        );
}
/*--- End of the function ------------------------------------------------------------------------*/

static unsigned long    clk_apll_pad_recalc_rate  (
                                                        struct clk_hw * hw,
                                                        unsigned long   parent_rate
                                                        )
{
    struct clk_audio_pad *  apad_ck         = to_clk_audio_pad(hw);
    unsigned long           f_AUDIOPINCLK   = 0;

    u8                      DIV         = DIV_QDAUDIO_to_DIV    (apad_ck->DIV_QDAUDIO);
    u8                      QDAUDIO     = DIV_QDAUDIO_to_QDAUDIO(apad_ck->DIV_QDAUDIO);
#if DEBUG_APINCLK_SEVERE
    __PRINTK(YEL,   "Given : \n"
                    "%49s parent_rate = %lu. \n"
                    "%49s DIV = %u. QDAUDIO = %u. apad_ck->DIV_QDAUDIO = %u.\n",
                    "", parent_rate,
                    "", DIV, QDAUDIO, apad_ck->DIV_QDAUDIO);
#endif
    if (DIV && QDAUDIO) {
        f_AUDIOPINCLK = parent_rate / (DIV * QDAUDIO);
    }
    __PRINTK(GRN""BOLD, "f_AUDIOPINCLK = %lu. DIV = %u. QDAUDIO = %u.\n" ,
                        f_AUDIOPINCLK, DIV, QDAUDIO);
    return f_AUDIOPINCLK;
}
/*--- End of the function ------------------------------------------------------------------------*/

static int              clk_apll_compute_DIV_QDAUDIO (
                                                    unsigned long   parent_rate,
                                                    unsigned long   required_rate,
                                                    unsigned long * DIVxQDAUDIO,
                                                    u8 *            DIV,
                                                    u8 *            QDAUDIO
                                                    )
{
    unsigned long tmp_DIVxQDAUDIO   = 0;
    unsigned long tmp_DIV           = 0;
    unsigned long tmp_QDAUDIO       = 0;
#if DEBUG_APINCLK_SEVERE
    __PRINTK(YEL,   "Given : parent_rate = %lu. required_rate = %lu.\n",
                    parent_rate, required_rate);
#endif
    if (!required_rate)
    {
        __PRINTK(RED, "required_rate = %lu.\n", required_rate);
        return -EINVAL;
    }
    ///f_AUDIOPINCLK = f_AUDIOCORECLK/(DIV * QDAUDIO)
    ///DIV * QDAUDIO = f_AUDIOCORECLK/f_AUDIOPINCLK
    tmp_DIVxQDAUDIO = parent_rate / required_rate;
    if (!tmp_DIVxQDAUDIO ||
    (tmp_DIVxQDAUDIO > AT91_PMC_AUDIO_PLL_DIVxQDAUDIO_MAX))
    {
        __PRINTK(RED,   "ERROR : Calculated DIV*QDAUDIO = %lu.\n"
                        "%49s AT91_PMC_AUDIO_PLL_DIVxQDAUDIO_MAX = %d.\n",
                        tmp_DIVxQDAUDIO,
                        "", AT91_PMC_AUDIO_PLL_DIVxQDAUDIO_MAX);
        return -EINVAL;
    }

    if (tmp_DIVxQDAUDIO <= AT91_PMC_AUDIO_PLL_QDAUDIO_MAX)
    {

        tmp_DIV = 1;
        tmp_QDAUDIO = tmp_DIVxQDAUDIO;
#if DEBUG_APINCLK_SEVERE
        __PRINTK(CYN,   "DIV*QDAUDIO(%lu) < QDAUDIO_MAX(%u).\n"
                        "%49s DIV = %lu. QDAUDIO = %lu.\n",
                        tmp_DIVxQDAUDIO, AT91_PMC_AUDIO_PLL_QDAUDIO_MAX,
                        "", tmp_DIV, tmp_QDAUDIO);
#endif
    } else { //if (tmp_DIVxQDAUDIO <= AT91_PMC_AUDIO_PLL_QDAUDIO_MAX)

        if ((tmp_DIVxQDAUDIO % 3) == 0)
        {
            tmp_DIV     = 3;
            tmp_QDAUDIO = tmp_DIVxQDAUDIO / tmp_DIV;
#if DEBUG_APINCLK_SEVERE
            __PRINTK(CYN,   "(DIV*QDAUDIO(%lu))%%3 == 0.\n"
                            "%49s DIV = %lu. QDAUDIO = %lu.\n",
                            tmp_DIVxQDAUDIO,
                            "", tmp_DIV, tmp_QDAUDIO);
#endif
        }
        else
        {
            tmp_DIV     = 2;
            tmp_QDAUDIO = tmp_DIVxQDAUDIO >> 1;
#if DEBUG_APINCLK_SEVERE
            __PRINTK(CYN,   "DIV = %lu. QDAUDIO = %lu\n",
                            tmp_DIV, tmp_QDAUDIO);
#endif
        } //end if ((tmp_DIVxQDAUDIO % 3) == 0)
    } //end if (tmp_DIVxQDAUDIO <= AT91_PMC_AUDIO_PLL_QDAUDIO_MAX)

    /* if we were given variable to store, we can provide them */

    if (DIVxQDAUDIO) { *DIVxQDAUDIO = tmp_DIV * tmp_QDAUDIO; }
    if (DIV && QDAUDIO) {
        /* we can cast here as we verified the bounds just above */
        *DIV        = (u8)tmp_DIV;
        *QDAUDIO    = (u8)tmp_QDAUDIO;
    }
    return 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

static long             clk_apll_pad_round_rate     (
                                                    struct clk_hw * hw,
                                                    unsigned long   required_rate,
                                                    unsigned long * parent_rate
                                                    )
{
    struct clk_hw * pclk                = clk_hw_get_parent(hw);
    long            best_rate           = -EINVAL;
    unsigned long   best_parent_rate    = 0;
    unsigned long   tmp_DIVxQDAUDIO;
#if DEBUG_APINCLK_SEVERE
    __PRINTK(YEL,   "Given : parent_rate = %lu. required_rate = %lu.\n",
                    *parent_rate, required_rate);
#endif
    ///You're calculating your divisor based on the AUDIO_PLL_REFERENCE_FOUT value.
    if (clk_apll_compute_DIV_QDAUDIO (
                                    /* *parent_rate, */
                                    AUDIO_PLL_REFERENCE_FOUT,
                                    required_rate,
                                    &tmp_DIVxQDAUDIO,
                                    NULL,
                                    NULL)
                                    )
    {
        __PRINTK(RED, "ERROR : Can't compute DIV*QDAUDIO.\n");
        return -EINVAL;
    }
#if DEBUG_APINCLK_SEVERE
    else {
        __PRINTK(GRN,   "DIV*QDAUDIO = %lu. Asking for the parent_rate = %lu.\n",
                        tmp_DIVxQDAUDIO, required_rate * tmp_DIVxQDAUDIO);
    }
#endif
    ///And then asking your parent to adapt its rate based on the returned divisor.
    best_parent_rate = clk_hw_round_rate(pclk, required_rate * tmp_DIVxQDAUDIO);
    best_rate = best_parent_rate / tmp_DIVxQDAUDIO;
#if DEBUG_APINCLK_SEVERE
    __PRINTK(GRN""BOLD, "best_rate = %ld. parent_rate = best_parent_rate = %lu.\n",
                        best_rate, best_parent_rate);
#endif
    *parent_rate = best_parent_rate;
    return best_rate;
}
/*--- End of the function ------------------------------------------------------------------------*/

static int              clk_apll_pad_set_rate  (
                                                    struct clk_hw * hw,
                                                    unsigned long   required_rate,
                                                    unsigned long   parent_rate
                                                    )
{
    struct clk_audio_pad *  apad_ck         = to_clk_audio_pad(hw);
    u8                      tmp_DIV;
    u8                      tmp_QDAUDIO;
    int                     ret;

    __PRINTK(YEL,   "Set new f_AUDIOPINCLK = %lu. required_rate = %lu.\n",
                    required_rate, parent_rate);
    ret = clk_apll_compute_DIV_QDAUDIO   (
                                        parent_rate,
                                        required_rate,
                                        NULL,
                                        &tmp_DIV,
                                        &tmp_QDAUDIO
                                        );
    if (!ret) {
        apad_ck->DIV_QDAUDIO = to_DIV_QDAUDIO(tmp_DIV, tmp_QDAUDIO);
#if DEBUG_APINCLK_SEVERE
        __PRINTK(GRN, "DIV_QDAUDIO = %u\n", apad_ck->DIV_QDAUDIO);
#endif
    } else {
        __PRINTK(RED, "Can't compute DIV_QDAUDIO. ret = %i\n", ret);
    }
    return ret;
}
/*--- End of the function ------------------------------------------------------------------------*/

static const struct     clk_ops audio_pll_pad_ops = {
    .enable         = clk_apll_pad_enable,
    .disable        = clk_apll_pad_disable,
    .recalc_rate    = clk_apll_pad_recalc_rate,
    .round_rate     = clk_apll_pad_round_rate,
    .set_rate       = clk_apll_pad_set_rate,
};

static void __init      of_sama5d2_clk_apll_pad_setup(struct device_node * np)
{
    struct clk_audio_pad *  apad_ck;
    struct clk_init_data    init;
    struct regmap *         regmap;
    const char *            parent_name;
    const char *            name = np->name;
    int                     ret;
    __PRINTK(MAG, "+++ Initializing audio PLL PAD +++\n");
    parent_name = of_clk_get_parent_name(np, 0);

    of_property_read_string(np, "clock-output-names", &name);

    regmap = syscon_node_to_regmap(of_get_parent(np));
    if (IS_ERR(regmap))
        return;

    apad_ck = kzalloc(sizeof(*apad_ck), GFP_KERNEL);
    if (!apad_ck)
        return;

    init.name           = name;
    init.ops            = &audio_pll_pad_ops;
    init.parent_names   = (parent_name ? &parent_name : NULL);
    init.num_parents    = 1;
    init.flags          = CLK_SET_RATE_GATE | CLK_SET_PARENT_GATE | CLK_SET_RATE_PARENT;

    apad_ck->hw.init    = &init;
    apad_ck->regmap     = regmap;
    apad_ck->lock       = &pmc_pcr_lock;

    ret = clk_hw_register(NULL, &apad_ck->hw);
    if (ret)
        kfree(apad_ck);
    else
        of_clk_add_hw_provider(np, of_clk_hw_simple_get, &apad_ck->hw);

    return;
}
/*--- End of the function ------------------------------------------------------------------------*/

CLK_OF_DECLARE(of_sama5d2_clk_apll_pad_setup,
           "atmel,sama5d2-clk-audio-pll-pad",
           of_sama5d2_clk_apll_pad_setup);
