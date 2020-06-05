#ifndef __MAX9867_I2C_REGS_H__
#define __MAX9867_I2C_REGS_H__


/* MAX9867 register space */
#define MAX9867_STATUS_REG              0x00 /* RO */
#define MAX9867_JACK_SENSE_REG          0x01 /* RO */
#define MAX9867_AUX_HIGH_REG            0x02 /* RO */
#define MAX9867_AUX_LOW_REG             0x03 /* RO */
#define MAX9867_IRQ_EN_REG              0x04

#define MAX9867_SYSCLK_REG              0x05
#define     MAX9867_SYSCLK_FREQ_MASK        0b1111
#define     MAX9867_SYSCLK_PSCLK_SHIFT      4
#define     MAX9867_SYSCLK_PSCLK_MASK       (0b11 << MAX9867_SYSCLK_PSCLK_SHIFT)
#define         MAX9867_SYSCLK_PSCLK_10_20      0b01 << MAX9867_SYSCLK_PSCLK_SHIFT
#define         MAX9867_SYSCLK_PSCLK_20_40      0b10 << MAX9867_SYSCLK_PSCLK_SHIFT
#define         MAX9867_SYSCLK_PSCLK_40_60      0b11 << MAX9867_SYSCLK_PSCLK_SHIFT

#define MAX9867_ACLK_CTLH_REG           0x06
#define     MAX9867_ACLK_CTLH_NI_MASK       0b01111111
#define         MAX9867_ACLK_CTLH_PLL       (1<<7)

#define MAX9867_ACLK_CTLL_REG           0x07
#if (CDC_MASTER)
    #define     MAX9867_ACLK_CTLL_NI_MASK       0xff
#else
    #define     MAX9867_ACLK_CTLL_NI_MASK       0b11111110
    #define     MAX9867_ACLK_CTLL_NI0_MASK      0b1
#endif
#define         MAX9867_ACLK_CTLL_RLC       0b1

#define MAX9867_IFC_M1_REG              0x08
#define     MAX9867_IFC_M1_TDM              (1 << 2)
#define     MAX9867_IFC_M1_HIZOFF           (1 << 3)
#define     MAX9867_IFC_M1_DLY              (1 << 4)
#define     MAX9867_IFC_M1_BCI              (1 << 5)
#define     MAX9867_IFC_M1_WCI              (1 << 6)
#define     MAX9867_IFC_M1_MAS              (1 << 7)

#define MAX9867_IFC_M2_REG              0x09
#define     MAX9867_IFC_M2_BSEL_MASK        0b111
#define         MAX9867_IFC_M2_BSEL_64xLRCLK         0x01
#define         MAX9867_IFC_M2_BSEL_48xLRCLK         0x02
#define         MAX9867_IFC_M2_BSEL_PCLK_2      0x04
#define         MAX9867_IFC_M2_BSEL_PCLK_4      0x05
#define         MAX9867_IFC_M2_BSEL_PCLK_8      0x06
#define         MAX9867_IFC_M2_BSEL_PCLK_16     0x07
#define     MAX9867_IFC_M2_DMONO            (1 << 3)
#define     MAX9867_IFC_M2_LVOLFIX          (1 << 4)

#define MAX9867_CODEC_FLTR_REG          0x0a
    #define MAX9867_CODEC_FLTR_DVFLT_MASK   0b111
    #define MAX9867_CODEC_FLTR_DVFLT_VAL(x) (x & MAX9867_CODEC_FLTR_DVFLT_MASK)
    #define MAX9867_CODEC_FLTR_AVFLT_SHIFT  4
    #define MAX9867_CODEC_FLTR_AVFLT_MASK   (0b111 << MAX9867_CODEC_FLTR_AVFLT_SHIFT)
    #define MAX9867_CODEC_FLTR_AVFLT_VAL(x) ( (x << MAX9867_CODEC_FLTR_AVFLT_SHIFT) & MAX9867_CODEC_FLTR_AVFLT_MASK )
    #define MAX9867_CODEC_FLTR_MODE         (1<<7)

#define MAX9867_SIDE_TONE_REG           0x0b
#define MAX9867_DAC_LVL_REG             0x0c
    #define MAX9867_DAC_LVL_DACA_MASK       0b1111
    #define MAX9867_DAC_LVL_DACA_VAL(x)     (x & MAX9867_DAC_LVL_DACA_MASK)
    #define MAX9867_DAC_LVL_DACG_SHIFT      4
    #define MAX9867_DAC_LVL_DACG_MASK       (0b11 << MAX9867_DAC_LVL_DACG_SHIFT)
    #define MAX9867_DAC_LVL_DACG_VAL(x)     ((x << MAX9867_DAC_LVL_DACG_SHIFT) & MAX9867_DAC_LVL_DACG_MASK)
    #define MAX9867_DAC_LVL_DACM            (1 << 6)
    
#define MAX9867_ADC_LVL_REG             0x0d

#define MAX9867_LEFT_LN_LVL_REG         0x0e
    #define MAX9867_LEFT_LN_LVL_LIGL_MASK       0b1111
    #define MAX9867_LEFT_LN_LVL_LIGL_VAL(x)     (x & MAX9867_LEFT_LN_LVL_LIGL_MASK)
    #define MAX9867_LEFT_LN_LVL_LILM            (1 << 6)

#define MAX9867_RIGHT_LN_LVL_REG        0x0f
    #define MAX9867_RIGHT_LN_LVL_LIGR_MASK      0b1111
    #define MAX9867_RIGHT_LN_LVL_LIGR_VAL(x)    (x & MAX9867_RIGHT_LN_LVL_LIGR_MASK)
    #define MAX9867_RIGHT_LN_LVL_LIRM           (1 << 6)

#define MAX9867_LEFT_VOL_REG            0x10
    #define MAX9867_LEFT_VOL_VOLL_MASK     0b111111
    #define MAX9867_LEFT_VOL_VOLL_VAL(x)   (x & MAX9867_LEFT_VOL_VOLL_MASK)
    #define MAX9867_LEFT_VOL_VOLM           1 << 6
#define MAX9867_RIGHT_VOL_REG           0x11
    #define MAX9867_RIGHT_VOL_VOLR_MASK    0b111111
    #define MAX9867_RIGHT_VOL_VOLR_VAL(x)  (x & MAX9867_RIGHT_VOL_VOLR_MASK)
    #define MAX9867_RIGHT_VOL_VOLM         1 << 6
    
#define MAX9867_LEFT_MIC_GAIN_REG       0x12
#define MAX9867_RIGHT_MIC_GAIN_REG      0x13

#define MAX9867_ADC_IN_CFG_REG          0x14
    #define MAX9867_ADC_IN_CFG_AUXEN        (1 << 0)
    #define MAX9867_ADC_IN_CFG_AUCAL        (1 << 1)
    #define MAX9867_ADC_IN_CFG_AUXGAIN      (1 << 2)
    #define MAX9867_ADC_IN_CFG_AUXCAP       (1 << 3)
    #define MAX9867_ADC_IN_CFG_MXINR_SHIFT  4
    #define MAX9867_ADC_IN_CFG_MXINR_MASK   (0b11 << MAX9867_ADC_IN_CFG_MXINR_SHIFT)
    #define MAX9867_ADC_IN_CFG_MXINR_VAL(x) ( (x << MAX9867_ADC_IN_CFG_MXINR_SHIFT) & MAX9867_ADC_IN_CFG_MXINR_MASK )
    
    #define MAX9867_ADC_IN_CFG_MXINL_SHIFT  6
    #define MAX9867_ADC_IN_CFG_MXINL_MASK   (0b11 << MAX9867_ADC_IN_CFG_MXINL_SHIFT)
    #define MAX9867_ADC_IN_CFG_MXINL_VAL(x) ( (x << MAX9867_ADC_IN_CFG_MXINL_SHIFT) & MAX9867_ADC_IN_CFG_MXINL_MASK )

#define MAX9867_MIC_CFG_REG             0x15

#define MAX9867_MODE_CFG_REG            0x16
#define     MAX9867_MODE_CFG_HPMODE_MASK    0b111
#define     MAX9867_MODE_CFG_HPMODE_VAL(x)  (x & MAX9867_MODE_CFG_HPMODE_MASK)
#define     MAX9867_MODE_CFG_JDETEN         (1 << 3)
                                          /*(1 << 4)*/
#define     MAX9867_MODE_CFG_ZDEN           (1 << 5)
#define     MAX9867_MODE_CFG_VSEN           (1 << 6)
#define     MAX9867_MODE_CFG_DSLEW          (1 << 7)

#define MAX9867_PWR_MAN_SHDN_REG        0x17
#define     MAX9867_PWR_MAN_ADREN           (1 << 0)
#define     MAX9867_PWR_MAN_ADLEN           (1 << 1)   
#define     MAX9867_PWR_MAN_DAREN           (1 << 2)
#define     MAX9867_PWR_MAN_DALEN           (1 << 3)
                                          /*(1 << 4)*/
#define     MAX9867_PWR_MAN_LNREN           (1 << 5)
#define     MAX9867_PWR_MAN_LNLEN           (1 << 6)
#define     MAX9867_PWR_MAN_SHDN_BIT        (1 << 7)
#define MAX9867_REV_REG                 0xff
#define MAX9867_REV_VAL                 0x42

#endif /* __MAX9867_I2C_REGS_H__ */
