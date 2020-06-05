#ifndef __MAX9867_I2C_MOD_H__
#define __MAX9867_I2C_MOD_H__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/regmap.h>


#define ENABLE      1
#define DISABLE     0
#define TRUE        1
#define FALSE       0

#define NI_VALUE    0x2000
#define FCLK_RATE   16000
#define CDC_MASTER  TRUE

#include "max9867_i2c_regs.h"

/*! @addtogroup Console_output
 * @{ */
#define NORM                "\x1B[0m"   ///normal
#define RED                 "\x1B[31m"  ///red
#define GRN                 "\x1B[32m"  ///green
#define YEL                 "\x1B[33m"  ///yellow
#define BLU                 "\x1B[34m"  ///blue
#define MAG                 "\x1B[35m"  ///magenta
#define CYN                 "\x1B[36m"  ///cyan
#define WHT                 "\x1B[37m"  ///white
#define BOLD                "\e[1m"     ///bold symbols
#define ITAL                "\e[3m"     ///italic text
#define BLINK               "\e[5m"     ///blink


/* Strip down path from filename */
#define __FILENAME__    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#ifdef DEBUG
    # define __PRINTK(color, text, ...) \
                        printk(color"%-15.15s %-15.15s #%-5i: "text""NORM, \
                        __FILENAME__, __FUNCTION__, __LINE__ , ##__VA_ARGS__);                       
                        
#else   /* #ifdef DEBUG */
    # define __PRINTK(color, text, ...)
#endif  /* #ifdef DEBUG */

#define __PRINTERR(text, ...)   __PRINTK(RED, "ERROR : "text""NORM, ##__VA_ARGS__);

#define __PRINTK_COND(condition, color, text, ... ) \
        if (condition) { __PRINTK(color, text""NORM, ##__VA_ARGS__); }
#define PRINTK_COND(condition, color, text, ... ) \
        if (condition) { printk(color""text""NORM, ##__VA_ARGS__); }




/* codec private data */
struct max9867_priv {
    struct regmap * regmap;
    unsigned int    sysclk;
    unsigned int    pclk;
    unsigned int    master;
};

#endif /* __MAX9867_I2C_MOD_H__ */
