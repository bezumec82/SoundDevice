/*! @file clk-common.c
 * @brief System clock custom kernel implementation.
 * @details #sysclk_source_tick reschedules tasklet #source_tasklet
 * that is represented by function #source_tasklet_callback.
 * #source_tasklet_callback function should be called synchronously,
 * to call synchronously #msec_scheduler.
 * For this purpose TDM is good aim.
 * If #source_tasklet_callback wasn't called from any synchronous source,
 * #sys_clock_fail will be called.
 */

#include "driver.h"
#include "msec-sched.h"

/// This flag shows that tasklet callback #source_tasklet_callback was called.
int was_gpt_int = 0;
/// @bug Variable not used in code.
/// @code
int volatile sys_clock_check_first_time = 1;
/// @endcode

/// This flag is reseted in function #get_sysclk_change.
int volatile sys_clock_change = 1;

/// Sys clock state flag.
int volatile sys_clock_ok = 0;
/*! This flags shows that function #sys_clk_ctl_tmr_callback was called first time. */
int volatile sys_clock_first_time = 1;

/// Used in #sys_clk_ctl_tmr_callback
static u_long volatile  curr_tick, prev_tick;

static u_long volatile curr_clock;  ///<Filled in #sys_clk_ctl_tmr_callback.
static u_long volatile  curr_clock_frac;

static int sys_clock_change_threshold = 0;

static u_long volatile tick_high;
/*! Used to set ID for each new #sysclk_source in function #sysclk_source_add. */
static int                      sysclk_source_id = 1;
/// Tasklet to reschedule from #sysclk_source_tick.
static struct tasklet_struct    source_tasklet; 
/// Timer to control sysclock.
static struct timer_list        sys_clk_ctl_tmr; 


/*! @addtogroup Lists
 * @{ --------------------------------------------------------------------------------------------*/
/*! Used to store #sysclk_source structures. */
static struct list_head         source_list;
/*! Used to protect #source_list from concurrency. */
struct semaphore                source_list_lock;
/*! @} -------------------------------------------------------------------------------------------*/


/* Functions prototypes */
static void source_tasklet_callback(unsigned long data);
/*----------------------------------------------------------------------------------------------- */


/*! Returns information about sys-clock.
 * @param freq      : Return parameter. Current sys clock frequency.
 * @param freq_frac : Return parameter. Current sys clock fracture frequency. Not implemented.
 * @return          Value of #sys_clock_ok flag.
 */
int get_sysclk_state(u_long * freq, u_long * freq_frac)
{
    int status;

    status = sys_clock_ok;
    if (freq)
        *freq = curr_clock;
    if (freq_frac)
        *freq_frac = curr_clock_frac;

    return status;
}
/*--- End of function --------------------------------------------------------------------------- */

/// Resets #sys_clock_change flag.
/// @return Previous value of #sys_clock_change flag.
int get_sysclk_change(void)
{
    int prev = sys_clock_change;
    sys_clock_change = 0;
    return prev;
}
/*--- End of function --------------------------------------------------------------------------- */

/*! Removes all entries from #source_list and frees according #sysclk_source structures. */
void sysclk_source_cleanup_list(void)
{
    struct list_head * node;
    struct sysclk_source * source;

    down(&source_list_lock);
    for (   node = source_list.next;
            node != &source_list;
            node = source_list.next
            ) {
        list_del(node);
        source = list_entry(node, struct sysclk_source, node);
        kfree(source);
    }
    /// @bug Taken one semaphore, but returned another.
    /// @code
//    up(&notify_list_lock); 
    up(&source_list_lock);
    /// @endcode
}
/*--- End of function --------------------------------------------------------------------------- */

/*! This function finds #sysclk_source structure
 * in linked list #source_list by given id.
 * @param id : Identification of #sysclk_source structure to find.
 * @return #sysclk_source structure in linked list #source_list.
 */
struct sysclk_source * sysclk_source_get_node_by_id(int id)
{
    struct list_head * node;
    struct sysclk_source * iter;
    /*! #source_list was initialized in #sys_clock_init_common */
    for (   node = source_list.next; 
            node != &source_list; 
            node = node->next) {
        iter = list_entry(node, struct sysclk_source, node);
        if (iter->id == id) {
            return iter;
        }
    }
    return NULL;
}
/*--- End of function --------------------------------------------------------------------------- */

/*! This function finds the first non-NULL #sysclk_source structure 
 * in linked list #source_list.
 * @return First non-NULL #sysclk_source structure in linked list #source_list.
 */ 
struct sysclk_source * sysclk_source_get_head (void)
{
    struct list_head * node;
    struct sysclk_source * iter = NULL;
    struct sysclk_source * master = NULL;
    /*! #source_list was initialized in #sys_clock_init_common */
    for (   node = source_list.next; 
            node != &source_list; /* Full cycle through linked list */
            node = node->next
            ) {
        iter = list_entry(node, struct sysclk_source, node);
        if (iter) master = iter;
    } //end for
    return master;
}
/*--- End of function --------------------------------------------------------------------------- */

/*! This function finds #sysclk_source structure in the #source_list by ID and deletes it.
 * @param id : ID of the #sysclk_source structure to delete.
 */
int sysclk_source_remove(int id)
{
    struct sysclk_source *source;

    down(&source_list_lock);
    source = sysclk_source_get_node_by_id(id);
    if (!source) {
        up(&source_list_lock);
        return -ENOENT;
    }
    list_del(&source->node);
    up(&source_list_lock);
    kfree(source);
    return 0;
}
/*--- End of function --------------------------------------------------------------------------- */

/*! This function initializes and adds new #sysclk_source to the #source_list linked list.
 * @param cfg   : Configuration for the #sysclk_source.
 * @return      System clock source given ID.
 */
int sysclk_source_add(struct s_sysclk_config * cfg)
{
    int id = -EINVAL;
    struct sysclk_source * source;

    if (!cfg) return -EINVAL;
    /// -# Take #source_list_lock semaphore. 
    down(&source_list_lock);
    /// -# Allocate new #sysclk_source structure.
    source = kmalloc(sizeof(struct sysclk_source), GFP_KERNEL);
    
    if (!source) {
        up(&source_list_lock);
        return -ENOMEM;
    }
    /// -# Set #sysclk_source structure's ID from
    /// global variable #sysclk_source_id.
    id = source->id = sysclk_source_id++;
    source->tick_count = 0;
    source->cfg = *cfg;

    /// -# Add new new #sysclk_source structure to the #source_list.
    list_add_tail(&source->node, &source_list);
    
    /// -# Return #source_list_lock semaphore. 
    up(&source_list_lock);


    return id;
}
/*--- End of function --------------------------------------------------------------------------- */

/*! Main purpose of this function is to call for 'tasklet_schedule' to reschedule source_tasklet.
 * This function changes tick count to the #sysclock_source structure with the given ID.
 * This function represented to kernel for use in other modules.
 * @param id            :   ID of #sysclock_source to operate on. 
 *                          ID was returned from #sysclk_source_add.
 * @param tick_count    :   New tick count to set.
 */
int sysclk_source_tick(
                        int id, 
                        unsigned long tick_count
                        ) 
{
    int result = -EINVAL;
    struct sysclk_source * source;
    struct sysclk_source * head;

    down(&source_list_lock);    
    source = sysclk_source_get_node_by_id(id);
    if (!source) {
        up(&source_list_lock);
        return -ENOENT;
    }
    source->tick_count = tick_count;
    result = 0;
    
    /*! If #sysclock_source for given ID is the first in the list,
     * it schedules #source_tasklet. */
    head = sysclk_source_get_head();
    if (source == head)
    {
        tasklet_schedule(&source_tasklet);
    }
    up(&source_list_lock);
    __PRINTK(GRN, "New system clock added.\n");
    return result;
}
/*--- End of function --------------------------------------------------------------------------- */

/*! Prints state of #curr_clock variable.
 */
void sys_clock_print(struct sysclk_source * source) 
{
    if (curr_clock == 0) {
        PRINTK(KERN_CRIT, "System clock error: too low or absent !!!\n");
    } else if (
                ((curr_clock >= 1) 
                && (curr_clock <= (source->cfg.normal_clock_low-1))) 
                ||((curr_clock >= (source->cfg.normal_clock_high+1)) 
                && (curr_clock <= 2048000))
                ) {
        PRINTK(KERN_CRIT, "System clock error: %lu Hz !!!\n", curr_clock);
    } else if (
                (curr_clock >= source->cfg.normal_clock_low) 
                && (curr_clock <= source->cfg.normal_clock_high)
                ) {
        PRINTK(KERN_INFO, "System clock normal: %lu Hz\n", curr_clock);
    } else {
        curr_clock = 0x7FFFFFFF;
        PRINTK(KERN_CRIT, "System clock error: too high !!!\n");
    } //end if (curr_clock
}
/*--- End of function --------------------------------------------------------------------------- */


/*! Send notification to all registered in #notify_list. */
void notify_all(void)
{
    struct list_head * node;
    struct sysclk_state_notify * notify;
    extern struct list_head         notify_list;

    sys_clock_change = 1;
    for (   node = notify_list.next; 
            node != &notify_list; 
            node = node->next
            ) {
        notify = list_entry(node, struct sysclk_state_notify, node);
        (notify->cb) (notify->ctx, sys_clock_ok, curr_clock);
    } //end for
}
/*--- End of function --------------------------------------------------------------------------- */
/*! Prints the reason of failure and resets sys clock failure state.
 * @param source : #sysclk_source 
 */
void sys_clock_fail(struct sysclk_source * source) 
{
    if ( 
        sys_clock_ok 
        && sys_clock_change_threshold++ 
        >= source->cfg.clock_fail_threshold
        ) {
        __PRINTK_RATELTD(RED, "System clock source failed.\n");
        sys_clock_ok = 0;               /// -# Reset #sys_clock_ok flag.
        sys_clock_change_threshold = 0; /// -# Clear change threshold.
        sys_clock_print(source);        /// -# Prints state of #curr_clock variable
        notify_all();                   /// -# Notify everyone about sys clock failure.
    }
}
/*--- End of function --------------------------------------------------------------------------- */
void sys_clock_normal(struct sysclk_source  *source) 
{
    if (sys_clock_first_time) {
        sys_clock_ok = 0;
    }
    if (
        !sys_clock_ok 
        && sys_clock_change_threshold++ >= 
        source->cfg.clock_normal_threshold
        ) {
        sys_clock_ok = 1;               /// -# Set #sys_clock_ok flag to failure.
        sys_clock_change_threshold = 0; /// -# Clear change threshold.
        sys_clock_print(source);        /// -# Prints state of #curr_clock variable
        notify_all();                   /// -# Notify everyone about sys clock failure.
    }
}
/*--- End of function --------------------------------------------------------------------------- */



/*! Tasklet callback function.
 * Tasklet reschedules from #sysclk_source_tick.
 * @param data : Is not used.
 */
static void source_tasklet_callback(unsigned long data)
{
    struct sysclk_source * source;
    
    /// -# Find first non-NULL #sysclk_source structure in linked list #source_list.
    source = sysclk_source_get_head();
    if (source) {
        /// -# Remember current tick.
        curr_tick = source->tick_count;
        /// -# Set interrupt flag.
        /// If this flag willn't be set, #sys_clk_ctl_tmr_callback sets error.
        was_gpt_int = 1;
        /// -# Call for millisecond scheduler.
        msec_scheduler(source->cfg.tick_inc);
    } else {
        __PRINTK_RATELTD(RED, "No system clock source provided.\n");        
    } //end if (source)
}
/*--- End of function --------------------------------------------------------------------------- */
/*! Timer callback function.
 * Timer is created and configured in #sys_clock_init.
 * Function changes global variable #curr_clock.
 * @param arg : Is not used.
 */
static void sys_clk_ctl_tmr_callback(u_long arg) {
    struct sysclk_source * source;
    u_long diff;
    
    /// -# Find first non-NULL #sysclk_source structure in linked list #source_list.
    source = sysclk_source_get_head();
    if (source) {
        /// -# Define how many ticks passed after last call.
        diff = curr_tick - prev_tick;
        prev_tick = curr_tick;
        /// -# If #source_tasklet_callback wasn't called, set failure.
        if (!was_gpt_int) {
            curr_clock = 0;
            sys_clock_fail(source);
        /// -# If #source_tasklet_callback was called,
        /// change #curr_clock.
        } else {
            curr_clock = diff ?                     \
                    (SYS_CLOCK_CHECK_INTERVAL_MSEC  \
                    * source->cfg.ticks_per_ms      \
                    * source->cfg.normal_clock)     \
                    : -1;
            curr_clock /= diff;
            curr_clock_frac = 0;
            /// -# If #curr_clock exceeds limits that is set by 
            /// #sysclk_source.#s_sysclk_config.normal_clock_low and
            /// #sysclk_source.#s_sysclk_config.normal_clock_high - call #sys_clock_fail.
            if (
                curr_clock < source->cfg.normal_clock_low
                || curr_clock > source->cfg.normal_clock_high
                ) {
                sys_clock_fail(source);
            } else {
                /// -# If not, call #sys_clock_normal.
                sys_clock_normal(source);
            } //end if (curr_clock
        } //end if (!was_gpt_int)
        /// -# Reset #sys_clock_first_time flag.
        sys_clock_first_time = 0;
        /// -# Reset #was_gpt_int flag.
        was_gpt_int = 0;
    } else {
        __PRINTK(RED, "No system clock source provided.\n");
    }
    /// -# Change the expiration time of an already scheduled #sys_clk_ctl_tmr.
    mod_timer(&sys_clk_ctl_tmr, jiffies + SYS_CLOCK_CHECK_INTERVAL);
}
/*--- End of function --------------------------------------------------------------------------- */



/*! @name Initialization facilities
 * @{ */
/*! Called once from #sys_clock_init.
 * Initialization of #source_tasklet here.
 */
void sys_clock_init_common(void) {
    __PRINTK(GRN, "Initializing system clock facilities.\n");
    /// -# Initialize all global variables.
    sys_clock_ok = 0;
    sys_clock_first_time = 1;
    sys_clock_check_first_time = 1; 
    sys_clock_change_threshold = 0;
    prev_tick = curr_tick = 0;
    tick_high = 0;
    curr_clock = 0;
    curr_clock_frac = 0;

    /// -# Initialize two lists : #notify_list and #source_list
    /// and its semaphores.
    init_notify();
    INIT_LIST_HEAD(&source_list);
    sema_init(&source_list_lock, 1);
    
    /// -# Initialize tasklet #source_tasklet with 
    /// callback #source_tasklet_callback and without data.
    /// This tasklet will be scheduled in function #sysclk_source_tick.
    tasklet_init(&source_tasklet, source_tasklet_callback, 0);
    return;
}

/*! Initialization function.
 * Will be added to #init_struct and called from #init_startup function.
 * This function initializes kernel timer with callback #sys_clk_ctl_tmr_callback 
 * and timeout @a SYS_CLOCK_CHECK_INTERVAL.
 * @param context - Not used in this routine.
 *                  Structure #hal_context will be passed by #init_startup.
 */
int sys_clock_init(void * context)
{
    /// Tasklet #source_tasklet initializes here.
    sys_clock_init_common();
    __PRINTK(GRN, "Initializing system clock timer.\n");    
    init_timer(&sys_clk_ctl_tmr);
    sys_clk_ctl_tmr.function = sys_clk_ctl_tmr_callback;
    sys_clk_ctl_tmr.expires = jiffies + SYS_CLOCK_CHECK_INTERVAL;
    add_timer(&sys_clk_ctl_tmr);
    return 0;
}

/*! De-initialization function.
 * @param context - Not used in this routine.
 *                  Structure #hal_context will be passed by #init_startup.
 */
void sys_clock_deinit(void *context)
{
    __PRINTK(GRN, "De-initializing system clock timer.\n");  
    tasklet_disable(&source_tasklet);
    tasklet_kill(&source_tasklet);
    del_timer(&sys_clk_ctl_tmr);
    notify_cleanup_list();
    sysclk_source_cleanup_list();
}
/*! @} -------------------------------------------------------------------------------------------*/


/*! Function return value of global variable #ts_clock.
 * This variable is increased from parameter #ts_inc of function #msec_scheduler
 * @return Value of global variable #ts_clock defined in file msec-sched.c.
 */
u32 ts_timer(void)
{
    extern volatile u32 ts_clock;
    return ts_clock;
}
/*--- End of function --------------------------------------------------------------------------- */


EXPORT_SYMBOL(ts_timer);
EXPORT_SYMBOL(sysclk_source_tick);
EXPORT_SYMBOL(sysclk_source_add);
EXPORT_SYMBOL(sysclk_source_remove);
