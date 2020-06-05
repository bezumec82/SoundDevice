#ifndef __GPIO_SPPLMNTL__
#define __GPIO_SPPLMNTL__

#include "main.h"
/*----------------------------------------------------------------------------*/
#define GPIO_DIRECTION_OUT              0
#define GPIO_DIRECTION_IN               1

#define CRATE_ADDR_LEN_BITS             6
#define CRATE_ADDR_OFFSET               2

#define BUFFER_MAX_SIZE                 512
/*----------------------------------------------------------------------------*/

/* Functions prototypes */
int Get_GPIO_val(uint32_t gpio_num);
int Set_GPIO_val(uint32_t gpio_num, uint32_t value);
/*----------------------------------------------------------------------------*/

#endif
