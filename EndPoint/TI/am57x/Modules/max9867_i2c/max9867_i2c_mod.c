#include "max9867_i2c_mod.h"

static const struct reg_default max9867_reg_defs[] = {
    /* MAX9867_STATUS_REG */
    /* MAX9867_JACK_SENSE_REG */
    /* MAX9867_AUX_HIGH_REG*/
    /* MAX9867_AUX_LOW_REG*/    
    { .reg = MAX9867_IRQ_EN_REG,           .def = 0x00 },
    { .reg = MAX9867_SYSCLK_REG,           .def = 0x00 },
    { .reg = MAX9867_ACLK_CTLH_REG,        .def = 0x00 },
    { .reg = MAX9867_ACLK_CTLL_REG,        .def = 0x00 },
    { .reg = MAX9867_IFC_M1_REG,           .def = 0x00 },
    { .reg = MAX9867_IFC_M2_REG,           .def = 0x00 },
    { .reg = MAX9867_CODEC_FLTR_REG,       .def = 0x00 },
    { .reg = MAX9867_SIDE_TONE_REG,        .def = 0x00 },
    { .reg = MAX9867_DAC_LVL_REG,          .def = 0x00 },
    { .reg = MAX9867_ADC_LVL_REG,          .def = 0x00 },
    { .reg = MAX9867_LEFT_LN_LVL_REG,      .def = 0x00 },
    { .reg = MAX9867_RIGHT_LN_LVL_REG,     .def = 0x00 },
    { .reg = MAX9867_LEFT_VOL_REG,         .def = 0x00 },
    { .reg = MAX9867_RIGHT_VOL_REG,        .def = 0x00 },
    { .reg = MAX9867_LEFT_MIC_GAIN_REG,    .def = 0x00 },
    { .reg = MAX9867_RIGHT_MIC_GAIN_REG,   .def = 0x00 },
    { .reg = MAX9867_ADC_IN_CFG_REG,       .def = 0x00 },
    { .reg = MAX9867_MIC_CFG_REG,          .def = 0x00 },
    { .reg = MAX9867_MODE_CFG_REG,         .def = 0x00 },
    { .reg = MAX9867_PWR_MAN_SHDN_REG,     .def = 0x00 },
    { .reg = MAX9867_REV_REG,              .def = 0x42 },
};

#if (0)
/* bool (*volatile_reg)(struct device *dev, unsigned int reg); */
static bool max9867_volatile_reg    (
                                    struct device * dev, 
                                    unsigned int    reg
                                    )
{
    switch (reg) 
    {
        case MAX9867_STATUS_REG     :
        case MAX9867_JACK_SENSE_REG :
        case MAX9867_AUX_HIGH_REG   :
        case MAX9867_AUX_LOW_REG    :
            return true;
        default:
            return false;
    } /* end of switch */
}
/*--- End of the function ------------------------------------------------------------------------*/
#endif


static const struct regmap_range max9867_readable_ranges[] = 
{
    regmap_reg_range( MAX9867_STATUS_REG, MAX9867_PWR_MAN_SHDN_REG ),
};
static const struct regmap_access_table max9867_readable_table = 
{
    .yes_ranges     =             max9867_readable_ranges,
    .n_yes_ranges   = ARRAY_SIZE( max9867_readable_ranges ),
};


static const struct regmap_range max9867_writable_ranges[] = 
{
    regmap_reg_range( MAX9867_IRQ_EN_REG, MAX9867_PWR_MAN_SHDN_REG ),
};
static const struct regmap_access_table max9867_writable_table = 
{
    .yes_ranges     =             max9867_writable_ranges,
    .n_yes_ranges   = ARRAY_SIZE( max9867_writable_ranges ),
};


static const struct regmap_range max9867_volatile_ranges[] = 
{
    regmap_reg_range(MAX9867_STATUS_REG, MAX9867_AUX_LOW_REG),
};
static const struct regmap_access_table max9867_volatile_table = 
{
    .yes_ranges     =             max9867_volatile_ranges,
    .n_yes_ranges   = ARRAY_SIZE( max9867_volatile_ranges ),
};


static const struct regmap_config max9867_i2c_regmap_config = 
{
    .reg_bits           = 8,                            ///<Number of bits in a register address.
    .val_bits           = 8,                            ///<Number of bits in a register value.
    .max_register       = MAX9867_REV_REG,              ///<Optional, specifies the maximum valid register address.
    .reg_defaults       = max9867_reg_defs,             ///<Power on reset values for registers.
    .num_reg_defaults   = ARRAY_SIZE(max9867_reg_defs), ///<Number of elements in reg_defaults.
    .rd_table           = &max9867_readable_table,      ///<Points to a struct regmap_access_table specifying valid ranges for read access.
    .wr_table           = &max9867_writable_table,      ///<Points to a struct regmap_access_table specifying valid ranges for write access.
#if (0)
    /* Optional callback returning true if the register value can't be  cached. 
     * If this field is NULL but volatile_table (see below) is not, 
     * the check is performed on such table*/
    .volatile_reg       = max9867_volatile_reg,   
#else
    .volatile_table     = &max9867_volatile_table,
#endif
    .cache_type         = REGCACHE_RBTREE,
};

int max9867_cfg (struct max9867_priv * max9867)
{
    uint16_t ni_hi;
    uint16_t ni_low;
    __PRINTK(YEL, "Configuring max9867 codec.\n");
    regmap_write  (
                        max9867->regmap,            /* map  */
                        MAX9867_PWR_MAN_SHDN_REG,   /* reg  */
                        0x00                        /* Power off  */
                        );

    /* FREQ = 0x00      Normal or PLL mode.
     * PSCLK = 0b01     Select if MCLK is between 10MHz and 20MHz.
     *                  12.288 MHz provided */
    regmap_update_bits  (
                        max9867->regmap,            /* map */
                        MAX9867_SYSCLK_REG,         /* reg */
                        ( MAX9867_SYSCLK_FREQ_MASK | MAX9867_SYSCLK_PSCLK_MASK ),  /* mask */
                        ( 0                        | MAX9867_SYSCLK_PSCLK_10_20 )  /* 10...20MHz range  */
                        );
    /* PLL = 0          Valid for slave and master mode. The frequency of LRCLK is set by the NI divider bits. 
     *                  In master mode, the MAX9867 generates LRCLK using the specified divide ratio.
     *                  When PLL = 0, the frequency of LRCLK is determined by NI. */
    ni_hi   = (NI_VALUE >> 8) & MAX9867_ACLK_CTLH_NI_MASK;
    ni_low  = 0x00FF & NI_VALUE;
    regmap_update_bits  (
                        max9867->regmap,                                       /* map  */
                        MAX9867_ACLK_CTLH_REG,                                 /* reg  */
                        ( MAX9867_ACLK_CTLH_NI_MASK | MAX9867_ACLK_CTLH_PLL ), /* mask */
                        ( ni_hi                     | 0                     )  /* val  */
                        );
    regmap_update_bits  (
                        max9867->regmap,              /* map  */
                        MAX9867_ACLK_CTLL_REG,        /* reg  */
                        MAX9867_ACLK_CTLL_NI_MASK,    /* mask */
                        ni_low                        /* val  */
                        );    
    /* MAS = 1          The MAX9867 operates in master mode with LRCLK and BCLK configured as outputs.
     * WCI = 0          Left-channel data is input and output while LRCLK is low.
     * BCI = 0          SDIN is latched into the part on the rising edge of BCLK.
     *                  SDOUT transitions after the rising edge of BCLK as determined by SDODLY. 
     *                  LRCLK changes state immediately after the rising edge of BCLK.
     * DLY = 1          SDIN/SDOUT data is assumed to be delayed one BCLK cycle 
     *                  so that it is latched on the 2nd BCLK edge following an LRCLK edge (I2S-compatible mode).
     * HIZOFF = 1       SDOUT is set either high or low after all data bits have been transferred out of the MAX9867.
     * TDM = 0          LRCLK signal polarity indicates left and right audio. */
    regmap_update_bits  (
                        max9867->regmap,                                                                                                                    /* map  */
                        MAX9867_IFC_M1_REG,                                                                                                                 /* reg  */ 
                        ( MAX9867_IFC_M1_MAS | MAX9867_IFC_M1_WCI | MAX9867_IFC_M1_BCI | MAX9867_IFC_M1_DLY | MAX9867_IFC_M1_HIZOFF | MAX9867_IFC_M1_TDM ), /* mask */ 
                        ( MAX9867_IFC_M1_MAS | 0                  | 0                  | MAX9867_IFC_M1_DLY | MAX9867_IFC_M1_HIZOFF | 0                  )  /* val  */ 
                        );
    /* DMONO = 0        Stereo data input on SDIN is processed separately. 
     * LVOLFIX = 1      Line input to headphone output volume fixed at VOLL and VOLR bits. */
    regmap_update_bits  (
                        max9867->regmap,                                                                  /* map  */
                        MAX9867_IFC_M2_REG,                                                               /* reg  */ 
                        ( MAX9867_IFC_M2_BSEL_MASK     | MAX9867_IFC_M2_DMONO | MAX9867_IFC_M2_LVOLFIX ), /* mask */ 
                        ( MAX9867_IFC_M2_BSEL_64xLRCLK | 0                    | 0                      )  /* val  */ 
                        );    
#if (0)
    /* Accordingly to TOM */
    regmap_update_bits  (
                        max9867->regmap,                                                                                     /* map  */
                        MAX9867_CODEC_FLTR_REG,                                                                              /* reg  */
                        ( MAX9867_CODEC_FLTR_DVFLT_MASK     | MAX9867_CODEC_FLTR_AVFLT_MASK     | MAX9867_CODEC_FLTR_MODE ), /* mask */
                        ( MAX9867_CODEC_FLTR_DVFLT_VAL(0x1) | MAX9867_CODEC_FLTR_AVFLT_VAL(0X1) | 0                       )  /* val  */
                        );
#endif
    /* DAC Gain         
     * DACG             0b11    -> +18dB
     *                  0b10    -> +12dB
     *                  0b01    -> +6dB
     *                  0b00    -> 0dB
     * DAC Level
     * DACA             0xf     -> -15dB
     *                  0x0     -> 0dB
     * DACM = 0         No mute */
    regmap_update_bits  (
                        max9867->regmap,                                                                           /* map  */
                        MAX9867_DAC_LVL_REG,                                                                       /* reg  */
                        ( MAX9867_DAC_LVL_DACA_MASK     | MAX9867_DAC_LVL_DACG_MASK      | MAX9867_DAC_LVL_DACM ), /* mask */
                        ( MAX9867_DAC_LVL_DACA_VAL(0x0) | MAX9867_DAC_LVL_DACG_VAL(0b00) | 0                    )  /* val  */
                        );                      
    /* LILM/LIRM = 0    Line input is connected to the headphone amplifiers.
     * LILM/LIRM = 0    Line input is disconnected from the headphone amplifiers.
     * LIGL/LIGR        0x0 -> +24dB
     *                  0x4 -> +16dB
     *                  0x8 -> +8dB
     *                  0xC -> 0dB
     *                  0xF -> -6dB */
    regmap_update_bits  (
                        max9867->regmap,                                                  /* map  */
                        MAX9867_LEFT_LN_LVL_REG,                                          /* reg  */
                        ( MAX9867_LEFT_LN_LVL_LIGL_MASK     | MAX9867_LEFT_LN_LVL_LILM ), /* mask */
                        ( MAX9867_LEFT_LN_LVL_LIGL_VAL(0x8) | MAX9867_LEFT_LN_LVL_LILM )  /* val  */
                        );
    regmap_update_bits  (
                        max9867->regmap,                                                   /* map  */
                        MAX9867_RIGHT_LN_LVL_REG,                                          /* reg  */
                        ( MAX9867_RIGHT_LN_LVL_LIGR_MASK    | MAX9867_RIGHT_LN_LVL_LIRM ), /* mask */
                        ( MAX9867_RIGHT_LN_LVL_LIGR_VAL(0x8)| MAX9867_RIGHT_LN_LVL_LIRM )  /* val  */
                        );                                                                                    
    /* VOLLM/VOLRM = 0  Audio playback is unmuted.
     * Left/Right Playback Volume
     * VOLL/VOLR        0x0     -> +6dB
     *                  0x4     -> +4dB
     *                  0x9     -> 0dB
     *                  0x27    -> -84dB */
    
    regmap_update_bits  (
                        max9867->regmap,                                            /* map  */
                        MAX9867_LEFT_VOL_REG,                                       /* reg  */
                        ( MAX9867_LEFT_VOL_VOLL_MASK     | MAX9867_LEFT_VOL_VOLM ), /* mask */
                        ( MAX9867_LEFT_VOL_VOLL_VAL(0x4) | 0                     )  /* val  */
                        );                                                       
    regmap_update_bits  (
                        max9867->regmap,                                                /* map  */
                        MAX9867_RIGHT_VOL_REG,                                          /* reg  */
                        ( MAX9867_RIGHT_VOL_VOLR_MASK     | MAX9867_RIGHT_VOL_VOLM ),   /* mask */
                        ( MAX9867_RIGHT_VOL_VOLR_VAL(0x4) | 0                      )    /* val  */
                        );
    /* MXINL/MXINR = 0b10   Left/right line input */
    regmap_update_bits  (
                        max9867->regmap,                        /* map  */
                        MAX9867_ADC_IN_CFG_REG,                 /* reg  */
                        ( MAX9867_ADC_IN_CFG_MXINR_MASK      | MAX9867_ADC_IN_CFG_MXINL_MASK      ), /* mask */
                        ( MAX9867_ADC_IN_CFG_MXINR_VAL(0b10) | MAX9867_ADC_IN_CFG_MXINL_VAL(0b10) )  /* val  */
                        );
    /* HPMODE = 0b000   Stereo differential (clickless)
     *          0b001   Mono (left) differential (clickless)
     *          0b100   Stereo single-ended (clickless)
     *          0b110   Stereo single-ended (fast turn-on) */
    regmap_update_bits  (
                        max9867->regmap,                        /* map  */
                        MAX9867_MODE_CFG_REG,                   /* reg  */
                        ( MAX9867_MODE_CFG_HPMODE_MASK       | MAX9867_MODE_CFG_JDETEN  ), /* mask */
                        ( MAX9867_MODE_CFG_HPMODE_VAL(0b000) | 0                        )  /* val  */
                        );
    mdelay(100);
    regmap_update_bits  (
                        max9867->regmap,            /* map  */
                        MAX9867_PWR_MAN_SHDN_REG,   /* reg  */
                    #if (0)
                        ( MAX9867_PWR_MAN_SHDN_BIT | MAX9867_PWR_MAN_DAREN | MAX9867_PWR_MAN_DALEN ), /* mask */  
                        ( MAX9867_PWR_MAN_SHDN_BIT | MAX9867_PWR_MAN_DAREN | MAX9867_PWR_MAN_DALEN )  /* val  */
                    #else
                        0xff, /* mask */
                        0xff  /* val  */
                    #endif
                        );
    __PRINTK(GRN, "max9867 codec is configured.\n");
    return 0;
}

static int max9867_i2c_probe(
                            struct i2c_client *             i2c,
                            const struct i2c_device_id *    id
                            )
{
    struct max9867_priv * max9867;
    int ret, reg;

    max9867 = devm_kzalloc(&i2c->dev, sizeof(struct max9867_priv), GFP_KERNEL);
    if (!max9867) return -ENOMEM;

    i2c_set_clientdata(i2c, max9867);
    max9867->regmap = devm_regmap_init_i2c(i2c, &max9867_i2c_regmap_config);
    if (IS_ERR(max9867->regmap)) 
    {
        ret = PTR_ERR(max9867->regmap);
        __PRINTERR("Failed to allocate regmap: %d\n", ret);
        return ret;
    }
    ret = regmap_read(max9867->regmap, MAX9867_REV_REG, &reg);
    if (ret < 0) {
        __PRINTERR("Failed to read: %d\n", ret);
        return ret;
    }
    if (reg == MAX9867_REV_VAL)
    {
        __PRINTK(MAG, "Device revision: %x\n", reg);
        max9867_cfg(max9867);
    }
    else
    {
        __PRINTERR("Wrong device revision : 0x%x\n", reg);
    }
    
    return ret;
}
/*--- End of the function ------------------------------------------------------------------------*/

/* void (*shutdown)(struct i2c_client *); */
void max9867_i2c_shutdown (struct i2c_client * i2c)
{
    struct max9867_priv * max9867 = (struct max9867_priv *)i2c_get_clientdata(i2c);
    __PRINTK(RED, "Shutdown max9867 codec.\n");
    regmap_update_bits  (
                    max9867->regmap,            /* mask */
                    MAX9867_PWR_MAN_SHDN_REG,   /* reg  */
                    0xff,                       /* mask */
                    0x00                        /* Power off  */
                    );
}
/*--- End of the function ------------------------------------------------------------------------*/

/* int (*remove)(struct i2c_client *); */
int max9867_i2c_remove (struct i2c_client * i2c)
{
    struct max9867_priv * max9867 = (struct max9867_priv *)i2c_get_clientdata(i2c);    
    __PRINTK(RED, "Shutdown max9867 codec.\n");
    regmap_update_bits  (
                    max9867->regmap,            /* mask */
                    MAX9867_PWR_MAN_SHDN_REG,   /* reg  */
                    0xff,                       /* mask */
                    0x00                        /* Power off  */
                    );
    __PRINTK(RED, "max9867 module removed.\n");
    return 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

static const struct i2c_device_id max9867_i2c_id[] = {
    { "max9867", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, max9867_i2c_id);

static const struct of_device_id max9867_of_match[] = {
    { .compatible = "maxim,max9867", },
    { }
};
MODULE_DEVICE_TABLE(of, max9867_of_match);

static struct i2c_driver max9867_i2c_driver = {
    .driver = {
        .name = "max9867",
        .of_match_table = of_match_ptr(max9867_of_match),
    },    
    .probe      = max9867_i2c_probe,
    .remove     = max9867_i2c_remove,
    .shutdown   = max9867_i2c_shutdown,
    .id_table   = max9867_i2c_id,
};

module_i2c_driver(max9867_i2c_driver);

MODULE_AUTHOR("Laskov Constanine <laskov82@gmail.com>");
MODULE_DESCRIPTION("MAX9867 driver");
MODULE_LICENSE("GPL");
