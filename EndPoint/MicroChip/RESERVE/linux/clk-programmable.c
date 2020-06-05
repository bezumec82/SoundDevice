/*
 *  Copyright (C) 2013 Boris BREZILLON <b.brezillon@overkiz.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include <linux/clk-provider.h>
#include <linux/clkdev.h>
#include <linux/clk/at91_pmc.h>
#include <linux/of.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>

#include "pmc.h"

#define PROG_SOURCE_MAX         6
#define PROG_ID_MAX             5

#define PROG_STATUS_MASK(id)    (1 << ((id) + 8))
#define PROG_PRES_MASK          0xff
#define PROG_PRES(layout, pckr) ((pckr >> layout->presc_off) & PROG_PRES_MASK)
#define PROG_MAX_RM9200_CSS     3
#define AT91_PMC_PCK0_OFF       8

struct clk_prg_layout {
    u8 presc_off;
    u8 css_mask;
    u8 have_slck_mck;
};

struct clk_prg {
    struct clk_hw hw;
    struct regmap *regmap;
    u8 id;
    const struct clk_prg_layout *layout;
};

#define to_clk_prg(hw) container_of(hw, struct clk_prg, hw)


static inline bool      clk_system_ready(struct regmap * regmap, int id)
{
	unsigned int status;
	regmap_read (
                regmap, 
                AT91_PMC_SR, 
                &status
                );
	return status & (1 << (id + AT91_PMC_PCK0_OFF)) ? 1 : 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

static int              clk_prg_enable(struct clk_hw * hw)
{
    struct clk_prg *  prog = to_clk_prg(hw);
    __PRINTK(YEL""BOLD,   "Enable programmable clock ID %i.\n", prog->id);
    
    __PRINTK(MAG,   "Writing 0x%x to the offset 0x%x.\n",
                    1 << (prog->id + AT91_PMC_PCK0_OFF), AT91_PMC_SCER);
                    
	regmap_write    (
                    prog->regmap,
                    AT91_PMC_SCER,
                    1 << (prog->id + AT91_PMC_PCK0_OFF)
                    );
    while (!clk_system_ready(prog->regmap, prog->id))
    {
		cpu_relax();
    }
    __PRINTK(GRN""BOLD,"Programmable clock ID %i enabled.\n", prog->id);
	return 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

static unsigned long    clk_prg_recalc_rate(
                                                    struct clk_hw * hw,
                                                    unsigned long   parent_rate
                                                    )
{
    struct clk_prg *   prog = to_clk_prg(hw);
    unsigned int                pckr;
    regmap_read(prog->regmap, AT91_PMC_PCKR(prog->id), &pckr);

    return parent_rate >> PROG_PRES(prog->layout, pckr);
}
/*--- End of the function ------------------------------------------------------------------------*/

static int              clk_prg_determine_rate (
                                                        struct clk_hw *             hw,
                                                        struct clk_rate_request *   clk_request
                                                        )
{
    struct clk_prg *    prog        = to_clk_prg(hw);
    struct clk_hw *     parent;
    long                best_rate   = -EINVAL;
    unsigned long       parent_rate;
    unsigned long       tmp_rate;
    int                 prescaler;
    int                 i;


    __PRINTK(YEL,   "Given : ID %u. Required rate = %ld.\n", 
                    prog->id, clk_request->rate);

    for (i = 0; i < clk_hw_get_num_parents(hw); i++)
    {
        parent = clk_hw_get_parent_by_index(hw, i);
        if (!parent)
        {
            __PRINTK(RED, "ERROR : No parent with index %i.\n", i);
            continue;
        }

        parent_rate = clk_hw_get_rate(parent);
        __PRINTK(MAG,   "Looking for parent %s|%i|%lu.\n", 
                        __clk_get_name((parent)->clk), i, parent_rate);
        
        if (parent_rate < clk_request->rate){
            __PRINTK(CYN,   "Parent rate %lu is less than required %lu.\n", 
                            parent_rate, clk_request->rate);
            continue;
        }
        
        for (prescaler = 0; prescaler < PROG_PRES_MASK; prescaler++)
        {
            tmp_rate = parent_rate/(prescaler + 1); /* Divide by 2 on each cycle. */
            if (tmp_rate <= clk_request->rate)
            {
                break; /* Exit if close rate achieved. */
            }
        } //end for

        if (tmp_rate > clk_request->rate) /* This parent can't be used. */
        {
            __PRINTK(CYN,   "Parent %s|%i|%lu is not fit. PRESC > 0xff.\n", 
                            __clk_get_name((parent)->clk), i, parent_rate);
            continue;
        }

        if  (best_rate < 0 || /* Was initialized as -EINVAL */
            (clk_request->rate - tmp_rate) < (clk_request->rate - best_rate))
        {
            best_rate                       = tmp_rate;
            clk_request->best_parent_rate   = parent_rate;
            clk_request->best_parent_hw     = parent;
        }

        if (!best_rate)
        {
            break;
        }
    } //end for

    if (best_rate < 0)
    {
        __PRINTK(RED, "No parents are fit to achieve rate %lu.\n", clk_request->rate);
        return best_rate;
    }

    clk_request->rate = best_rate;
    __PRINTK(CYN,   "Choosen parent  %s|%ld. Rate will be : %lu.\n",
                    __clk_get_name((clk_request->best_parent_hw)->clk), parent_rate,
                    clk_request->rate
                    );
    return 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

static int              clk_prg_set_parent(struct clk_hw *hw, u8 index)
{
    struct clk_prg *               prog    = to_clk_prg(hw);
    const struct clk_prg_layout *  layout  = prog->layout;

    if (index > layout->css_mask)
    {
        __PRINTK(CYN, "ERROR : Parent index %i is more than mask.\n", index);
        return -EINVAL;
    }

    __PRINTK(MAG,   "Writing 0x%x to the offset 0x%x. Mask : 0x%x\n",
                    index, AT91_PMC_PCKR(prog->id), layout->css_mask);
    regmap_update_bits  (
                        prog->regmap,
                        AT91_PMC_PCKR(prog->id),
                        layout->css_mask,
                        index
                        );
    __PRINTK(CYN, "Clock source is set to : %i.\n", index);
    return 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

static u8               clk_prg_get_parent(struct clk_hw *hw)
{
    struct clk_prg *               prog    = to_clk_prg(hw);
    const struct clk_prg_layout *  layout  = prog->layout;
    unsigned int                            pckr;
    u8                                      ret;

    regmap_read(prog->regmap, AT91_PMC_PCKR(prog->id), &pckr);
    ret = pckr & layout->css_mask;
    if (layout->have_slck_mck && (pckr & AT91_PMC_CSSMCK_MCK) && !ret)
    {
        ret = PROG_MAX_RM9200_CSS + 1;
    }
    return ret;
}
/*--- End of the function ------------------------------------------------------------------------*/

static int              clk_prg_set_rate   (
                                                    struct clk_hw * hw,
                                                    unsigned long   required_rate,
                                                    unsigned long   parent_rate
                                                    )
{
    struct clk_prg *                        prog        = to_clk_prg(hw);
    const struct clk_prg_layout *           layout      = prog->layout;
    unsigned long                           div         = parent_rate / required_rate;
    unsigned int                            pckr;
    int                                     prescaler   = 0;

    __PRINTK(GRN,   "Given : parent_rate = %lu. required_rate = %lu.\n",
                    parent_rate, required_rate);

    if (!div)
    {
        __PRINTK(RED, "ERROR : Divider is 0.\n");
        return -EINVAL;
    }
    prescaler = div - 1;

    if (prescaler >= PROG_PRES_MASK)
    {
        __PRINTK(RED, "ERROR : PRES(%i) > %i.\n", prescaler, PROG_PRES_MASK);
        return -EINVAL;
    }

    __PRINTK(MAG,   "Writing 0x%x to the offset(0x%x)|bit pos.(0x%x).\n",
                    prescaler, AT91_PMC_PCKR(prog->id), layout->presc_off);
    regmap_update_bits  (
                        prog->regmap,
                        AT91_PMC_PCKR(prog->id),
                        PROG_PRES_MASK << layout->presc_off,
                        prescaler << layout->presc_off
                        );
    regmap_read (
                prog->regmap,
                AT91_PMC_PCKR(prog->id),
                &pckr
                );
    __PRINTK(CYN,   "PMC_PCK%i(0x%x) = 0x%x.\n", 
                    prog->id, AT91_PMC_PCKR(prog->id), pckr);
    return 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

static const struct     clk_ops programmable_ops = {
    .enable         = clk_prg_enable,
    .recalc_rate    = clk_prg_recalc_rate,
    .determine_rate = clk_prg_determine_rate,
    .get_parent     = clk_prg_get_parent,
    .set_parent     = clk_prg_set_parent,
    .set_rate       = clk_prg_set_rate,

};

static struct clk_hw * __init
at91_clk_register_programmable  (
                                struct regmap * regmap,
                                const char *    name,
                                const char **   parent_names,
                                u8              num_parents,
                                u8              id,
                                const struct clk_prg_layout *layout
                                )
{
    struct clk_prg *            prog;
    struct clk_hw *             hw;
    struct clk_init_data        init;
    int                         ret;

    if (id > PROG_ID_MAX)
    {
        __PRINTK(RED, "ERROR : Wrong clock ID : %i.\n", id);
        return ERR_PTR(-EINVAL);
    }
    prog = kzalloc(sizeof(*prog), GFP_KERNEL);
    if (!prog) { return ERR_PTR(-ENOMEM); }

    init.name           = name;
    init.ops            = &programmable_ops;
    init.parent_names   = parent_names;
    init.num_parents    = num_parents;
    init.flags          = CLK_SET_RATE_GATE | CLK_SET_PARENT_GATE;

    prog->id        = id;
    prog->layout    = layout;
    prog->hw.init   = &init;
    prog->regmap    = regmap;

    hw = &prog->hw;
    ret = clk_hw_register(NULL, &prog->hw);
    if (ret)
    {
        kfree(prog);
        hw = ERR_PTR(ret);
    } else {
        pmc_register_pck(id);
    }

    return hw;
}
/*--- End of the function ------------------------------------------------------------------------*/

static const struct clk_prg_layout     at91rm9200_programmable_layout  = {
    .presc_off = 2,
    .css_mask = 0x3,
    .have_slck_mck = 0,
};

static const struct clk_prg_layout     at91sam9g45_programmable_layout = {
    .presc_off = 2,
    .css_mask = 0x3,
    .have_slck_mck = 1,
};

/* SAMA5D27 HERE */
static const struct clk_prg_layout     at91sam9x5_programmable_layout  = {
    .presc_off = 4,
    .css_mask = 0x7,
    .have_slck_mck = 0,
};

static void __init  of_at91_clk_prog_setup(
                                            struct device_node *                    np,
                                            const struct clk_prg_layout *  layout
                                            )
{
    int                     num;
    u32                     id;
    struct clk_hw *         hw;
    unsigned int            num_parents;
    const char *            parent_names[PROG_SOURCE_MAX];
    const char *            name;
    struct device_node *    progclknp;
    struct regmap *         regmap;

    __PRINTK(MAG, "+++ Initializing programmable clock +++\n");

    num_parents = of_clk_get_parent_count(np);
    if (num_parents == 0 || num_parents > PROG_SOURCE_MAX)
        return;

    of_clk_parent_fill(np, parent_names, num_parents);

    num = of_get_child_count(np);
    if (!num || num > (PROG_ID_MAX + 1)){
        __PRINTK(RED, "ERROR : Provided %i clocks.\n", num);
        return;
    }

    regmap = syscon_node_to_regmap(of_get_parent(np));
    if (IS_ERR(regmap))
    {
        __PRINTK(RED, "ERROR : Can't get regmap.\n");
        return;
    }

    for_each_child_of_node(np, progclknp) {
        if (of_property_read_u32(progclknp, "reg", &id))
        {
        __PRINTK(RED, "ERROR : Prog ID didn't provided.\n");
            continue;
        }
        if (of_property_read_string(np, "clock-output-names", &name))
            name = progclknp->name;

        __PRINTK(CYN,   "Registering clock : \n"
                        "%49s name = %s. ID = %u. num_parents = %u. \n",
                        "", name, id, num_parents);
        hw = at91_clk_register_programmable (
                                            regmap,
                                            name,
                                            parent_names,
                                            num_parents,
                                            id,
                                            layout
                                            );
        if (IS_ERR(hw))
        {
            __PRINTK(RED,   "Can't gegister clock : \n"
                            "%49s name = %s. PID = %u. num_parents = %u. \n",
                            "", name, id, num_parents);
            continue;
        }

        of_clk_add_hw_provider(progclknp, of_clk_hw_simple_get, hw);
    } //end for_each_child_of_node
}
/*--- End of the function ------------------------------------------------------------------------*/


static void __init of_at91rm9200_clk_prog_setup(struct device_node *np)
{
    of_at91_clk_prog_setup(np, &at91rm9200_programmable_layout);
}
CLK_OF_DECLARE  (
                at91rm9200_clk_prog,
                "atmel,at91rm9200-clk-programmable",
                of_at91rm9200_clk_prog_setup
                );

static void __init of_at91sam9g45_clk_prog_setup(struct device_node *np)
{
    of_at91_clk_prog_setup(np, &at91sam9g45_programmable_layout);
}

CLK_OF_DECLARE  (
                at91sam9g45_clk_prog,
                "atmel,at91sam9g45-clk-programmable",
                of_at91sam9g45_clk_prog_setup);

static void __init of_at91sam9x5_clk_prog_setup(struct device_node *np)
{
    of_at91_clk_prog_setup(np, &at91sam9x5_programmable_layout);
}
CLK_OF_DECLARE  (
                at91sam9x5_clk_prog,
                "atmel,at91sam9x5-clk-programmable",
                of_at91sam9x5_clk_prog_setup);
