/*! @file module.c
 * @brief Entry point of the module.
 * 'module_init' and 'module_exit' are defined here.
 */

/*! @cond DO_NOT_SHOW ----------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
/*! @endcond -------------------------------------------------------------------------------------*/

#include "driver.h"

/*! @name Common module data
 * @{ */ 
#include VERSION_INCLUDE

MODULE_AUTHOR("Denis V.Sorokovik, Draky Void, q2l, microElk");
MODULE_DESCRIPTION("LEMZGW hardware abstraction layer (hal)");
MODULE_LICENSE("GPL");

#ifdef KBUILD_MODNAME
#undef KBUILD_MODNAME
#endif
#define KBUILD_MODNAME "hal"
/*! @} -------------------------------------------------------------------------------------------*/


/*! @name Initialization function prototypes
 * @details This functions become constructors and destructors in structure #init_struct.
 * @{ */
int sys_clock_init(void *);     ///< In file clk-common.c.
void sys_clock_deinit(void *);  ///< In file clk-common.c.

int msec_sched_init(void *);    ///< In file msec-sched.c.
void msec_sched_deinit(void *); ///< In file msec-sched.c.

int dsp_init(void *);
void dsp_deinit(void *);

int hal_ctl_init(void *);
void hal_ctl_deinit(void *);


int tdm_init(void *);           ///< In file timeslots.c.
void tdm_deinit(void *);        ///< In file timeslots.c.

int mux_init(void *);           ///< In file vmux.c
void mux_deinit(void *);        ///< In file vmux.c
/*! @} -------------------------------------------------------------------------------------------*/


/*! @name Global variables
 * @{ */
static char s_hal_version[256]  = "";           /*!< Used in #hal_version as ret. val. */
char * init[MAX_ENABLE_STRS]    \
    = {[0 ... MAX_ENABLE_STRS - 1] = NULL };    /*!< */
struct hal_context hal_context;                 /*!< Defined in driver.h */
/*! @} -------------------------------------------------------------------------------------------*/

/*! Function that returns versions of module.
 * Used in 'mod_init' just to print out versions.
 * #HAL_DEVNAME - defined as "hal" in file 'driver.h'.
 * KERN_INFO    - log level.
 * #HAL_DESCR   - defined in driver.h.
 * 
 * @return Filled global variable #s_hal_version.
 * 
 */
const char * hal_version( void )
{
    if( !s_hal_version[0] )
    {
        snprintf(   s_hal_version, sizeof(s_hal_version), 
                    "%s%s: hardware abstraction layer (%s)\n"
                    "%s: %s\n", 
                    KERN_INFO, HAL_DEVNAME, HAL_DESCR, 
                    HAL_DEVNAME, version_string()
                    );
    }
    return s_hal_version;
}

/*! @cond DO_NOT_SHOW 
 * This snippet creates array of 'init_struct'
 * taking constructors and destructors from above.
 */
INIT_PROLOGUE(hal)
    INIT_PART_DEFAULT(sys_clock)
    INIT_PART_DEFAULT(msec_sched)
    INIT_PART_DEFAULT(dsp)
    INIT_PART_DEFAULT(hal_ctl)
    INIT_PART_DEFAULT(tdm)
    INIT_PART_DEFAULT(mux)
INIT_EPILOGUE;
/*! @endcond */

/*! @name Initialization facilities
 * @{ */
/*! Module initialization function.
 * @return Propagate error from 'init_startup'.
 */
static int mod_init(void)
{
    /* Variables initialization */
    static int version_printed = 0;
    /* Code */
    __PRINTK(GRN, "Initializing HAL module.\n");
    
    if (version_printed++ == 0) {
        printk(hal_version());
    } //end if (version_printed++ == 0)
    
    return init_startup(
                        &hal_context,
                        INIT_STRUCT_NAME(hal),
                        INIT_NUM_ITEMS(hal),
                        init,
                        MAX_ENABLE_STRS
                        );
}
/*--- End of function ----------------------------------------------------------------------------*/

/*! Module exit function. */
static void mod_cleanup(void)
{
    __PRINTK(RED, "Removing HAL module.\n");
    init_shutdown(&hal_context, INIT_STRUCT_NAME(hal), INIT_NUM_ITEMS(hal));
#ifdef MEMORY_DEBUG
    memcheck();
#endif
}
/*! @} -------------------------------------------------------------------------------------------*/

/*! Not used in kernel version more than 2.6.0 */
void module_inc_use_count(void)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
    MOD_INC_USE_COUNT;
#endif
}
/*--- End of function ----------------------------------------------------------------------------*/

/*! Not used in kernel version more than 2.6.0 */
void module_dec_use_count(void)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
    MOD_DEC_USE_COUNT;
#endif
}
/*--- End of function ----------------------------------------------------------------------------*/


module_init(mod_init)
module_exit(mod_cleanup)
/*--- End of the file ----------------------------------------------------------------------------*/
