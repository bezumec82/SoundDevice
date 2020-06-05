#ifndef __CLK_COMMON_H_
#define __CLK_COMMON_H_

/*! @file clk-common.h */

#include "driver.h"

#define SYS_CLOCK_CHECK_INTERVAL_MSEC   2000
///Converts msec to jiffies
#define SYS_CLOCK_CHECK_INTERVAL        (SYS_CLOCK_CHECK_INTERVAL_MSEC * HZ / 1000)

/*! @name Structures prototypes
 * @{ --------------------------------------------------------------------------------------------*/
struct s_sysclk_config
{
    long    normal_clock;
    long    normal_clock_low;       ///< Used to check accepted range of #curr_clock.
    long    normal_clock_high;      ///< Used to check accepted range of #curr_clock.
    /// Used to control sys clock jitter. Compares with #sys_clock_change_threshold.
    long    clock_fail_threshold;
    long    clock_normal_threshold; 
    long    tick_inc;
    long    ticks_per_ms;
};

/*! Each such structure represents source of system clock. */
struct sysclk_source
{
    struct list_head        node;       ///< Used in #source_list.
    int                     id;         ///< Used for searching.
    unsigned long           tick_count; 
    struct s_sysclk_config  cfg;        ///< Clock configuration.
};

/*! Notification respondents list structure */
struct sysclk_state_notify {
    struct list_head node;
    sysclk_notify_cb_t cb;  ///< Callback function. Prototype #sysclk_notify_cb_t provided in hal.h.
    void * ctx;             ///< Parameter for notify callback function.
};
/*! @} -------------------------------------------------------------------------------------------*/

/*! @name System clock functions
 * @{ */
int get_sysclk_state(u_long * freq, u_long * freq_frac);
int get_sysclk_change(void);
void notify_cleanup_list(void);
struct sysclk_state_notify * notify_lookup(sysclk_notify_cb_t cb, void * ctx);
int notify_remove_sysclk_change(sysclk_notify_cb_t cb, void * ctx);
int notify_add_sysclk_change(sysclk_notify_cb_t cb, void * ctx);
void sys_clock_state_change(void);
void sys_clock_print(struct sysclk_source   * source);
void sys_clock_fail(struct sysclk_source    * source);
void sys_clock_normal(struct sysclk_source  * source);
void sys_clock_init_common(void);

/*! @name Initialization functions
 * @{ */
int sys_clock_init(void *context);
void sys_clock_deinit(void *context);
/*! @} -------------------------------------------------------------------------------------------*/

/*! @addtogroup Exported Exported to kernel functions
 * @{ */
 u32 ts_timer(void);
int sysclk_source_tick(int id, unsigned long tick_count);
int sysclk_source_add(struct s_sysclk_config    * cfg);
int sysclk_source_remove(int id);
/*! @} -------------------------------------------------------------------------------------------*/

/*! @} -------------------------------------------------------------------------------------------*/

/*! @name Notification functions
 * @brief Notification mechanism from sys clock.
 * @details If sys clock fails, notification to all recipients is sent.
 * @{ */
void notify_cleanup_list(void);
struct sysclk_state_notify * find_notify_node (sysclk_notify_cb_t cb, void * ctx);
int delete_notify_node(sysclk_notify_cb_t cb, void * ctx);
int add_notify_node(sysclk_notify_cb_t cb, void * ctx);
void notify_all(void);
void init_notify (void);
/*! @} -------------------------------------------------------------------------------------------*/



#endif /* __CLK_COMMON_H_ */

/*--- End of the file ----------------------------------------------------------------------------*/


