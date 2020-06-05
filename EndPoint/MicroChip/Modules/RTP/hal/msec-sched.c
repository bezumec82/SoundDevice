/*! @file msec-sched.c
 * @brief Implementation of custom millisecond scheduler.
 * @details
 * #msec_scheduler should be called synchronously from some sync irq source
 * for this to work.
 * #msec_scheduler is called from #source_tasklet_callback.
 * 
 * After you add your tasklet in #msec_sched_add_task,
 * this task will be scheduled in #msec_scheduler accordingly to
 * its #msec_task.counter <-> #msec_task.period.
 * 
 */

#include "driver.h"
#include "msec-sched.h"
#include <linux/list.h>


/*! @addtogroup Lists
 * @{ --------------------------------------------------------------------------------------------*/
 /*! @var msec_queue -  List to hold #msec_task structures.
 * Each such structure holds tasklet created
 * in function #msec_sched_add_task.
 */
LIST_HEAD(msec_queue);

/*! Semaphore to protect #msec_queue from concurency. */
struct semaphore msec_queue_lock;
/*! @} -------------------------------------------------------------------------------------------*/

/*! This parameter can only rise. 
 * It is increased from parameter #ts_inc of function #msec_scheduler.
 */
volatile u32 ts_clock;

/*! @name Initialization facilities
 * @{ */
/*! Millisecond scheduler initialization routine.
 * @param context - Not used in this routine.
 *                  Structure #hal_context will be passed by #init_startup.
 */
int msec_sched_init(void *context)
{
    __PRINTK(GRN, "Initializing millisecond scheduler.\n"); 
    /// -# Initialize #msec_queue list and its semaphore.
    INIT_LIST_HEAD(&msec_queue);
    sema_init(&msec_queue_lock, 1);
    /// -# Initialize scheduler clock #ts_clock to zero
    ts_clock=0;
    return 0;
};
/*! @} -------------------------------------------------------------------------------------------*/


/*! Millisecond scheduler de-initialization routine.
 * @param context - Not used in this routine.
 *                  Structure #hal_context will be passed by #init_startup.
 */
void msec_sched_deinit(void * context)
{
    struct list_head *loop_cursor, *tmp;
    struct msec_task *task;
    __PRINTK(YEL, "De-initializing millisecond scheduler.\n"); 
    /// -# Take #msec_queue_lock semaphore.
    down(&msec_queue_lock);
    /// -# Iterate over a list #msec_queue.
    list_for_each_safe(loop_cursor, tmp, &msec_queue)
    {
        /// -# Get #msec_task structure from the found list entry.
        task=list_entry(loop_cursor, struct msec_task, node);
        /// -# Disable and kill tasklet inside found #msec_task structure.
        tasklet_disable(&task->tasklet);
        tasklet_kill(&task->tasklet);
        /// -# Delite found list entry.
        list_del(&task->node);
        /// -# Deallocate found #msec_task structure.
        kfree(task);
    }    
    /// -# Return #msec_queue_lock semaphore.
    up(&msec_queue_lock);
}
/*--- End of function ----------------------------------------------------------------------------*/


/*! Custom millisecond scheduler.
 * @param ts_inc : Used to increase global variable #ts_clock.
 */
void msec_scheduler(int ts_inc)
{
    struct list_head * loop_cursor;
    struct list_head * tmp;
    struct msec_task * task;

    /// -# Increment scheduler clock #ts_clock
    if (ts_inc > 0) ts_clock+=ts_inc;
    
    /// -# Iterate over a list #msec_queue.
    /// starting tasklets which #msec_task.counter is 0.
    /// @bug Overkill to use safe mechanism here. No 'list_del' calls.
    /// @code
    list_for_each_safe(loop_cursor, tmp, &msec_queue)
    {        
        /// -# Get task from list #msec_task.
        task=list_entry(loop_cursor, struct msec_task, node);
        /// -# Decrement task counter.
        task->counter--;
        /// -# If task counter is zero schedule tasklet
        if(!task->counter)
        {
            /// -# Schedule the tasklet for execution.
            tasklet_schedule(&task->tasklet);
            /// -# Restore task counter.
            task->counter=task->period;
        } //end if(!task->counter)
    } //end list_for_each_safe
    ///@endcode
    return;
}
/*--- End of function ----------------------------------------------------------------------------*/

/*! @addtogroup Exported Exported to kernel functions
 * @{ --------------------------------------------------------------------------------------------*/
/*! Adds new function to execute in millisecond scheduler. 
 * @param funct     :   Function that represents new task.
 * @param data      :   Argument that will be passed to this function.
 * @param period    :   Execution period. 
 *                      Will be used to restore #msec_task.counter in function #msec_scheduler.
 */
int msec_sched_add_task(
                        void (*funct)(unsigned long), 
                        unsigned long data, 
                        uint period
                        ){
    /// -# Allocate new #msec_task structure.            
    struct msec_task * task;
    task=kmalloc(sizeof(struct msec_task), GFP_ATOMIC);
    if(task==NULL) return -ENOMEM;

    /// -# Fill #msec_task structure's fields.
    task->period=period;
    task->counter=1;
    /// -# Create tasklet inside #msec_task structure with given function and its parameter.
    tasklet_init(&task->tasklet, funct, (unsigned long) data);
    /// -# Disable created tasklet.
    tasklet_disable_nosync(&task->tasklet);
    INIT_LIST_HEAD(&task->node);
    
    down(&msec_queue_lock);
        /// -# Add new #msec_task structure to the #msec_queue list.
        list_add_tail(&task->node, &msec_queue);
    up(&msec_queue_lock);
    
    /// -# Enable created tasklet.
    /// To start tasklet, function 'tasklet_schedule' called in #msec_scheduler.
    tasklet_enable(&task->tasklet);
    return 0;
}
/*! @} -------------------------------------------------------------------------------------------*/


/*! Schedules task represented by parameter 'func' immediately.
 * Not vaiting for #msec_task.counter to achieve 0.
 * Not exported to the kernel. No calls to this function were found.
 * @param funct     :   Function that represents the task to schedule.
 * @param data      :   Argument that will be passed to this function.
 */
int msec_sched_run_now(
                        void (*funct)(unsigned long), 
                        unsigned long data
                        ){
    struct msec_task * task=NULL;
    struct list_head * tmp;

    down(&msec_queue_lock);
    list_for_each(tmp, &msec_queue)
    {
        task=list_entry(tmp, struct msec_task, node);
        if( task->tasklet.func==funct 
            && task->tasklet.data==data
          ) break;
    }
    
    if( tmp!=&msec_queue 
        && task!= NULL
        ){
        tasklet_schedule(&task->tasklet);
        task->counter=task->period;
    }
    
    up(&msec_queue_lock);
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

/*! @addtogroup Exported Exported to kernel functions
 * @{ --------------------------------------------------------------------------------------------*/
/*! Finds task represented by parameter 'func', kills tasklet and deletes list entry.
 * @param funct     :   Function that represents the task to delete.
 * @param data      :   Argument that will be passed to this function.
 */
int msec_sched_remove_task(
                            void (*funct)(unsigned long), 
                            unsigned long data){
    struct msec_task *task=NULL;
    struct list_head *tmp;
    uint found;

    found=0;
    down(&msec_queue_lock);

    list_for_each(tmp, &msec_queue)
    {
        task=list_entry(tmp, struct msec_task, node);
        if(task->tasklet.func==funct && task->tasklet.data==data) break;
    }
    if(tmp!=&msec_queue && task!= NULL)
    {
        tasklet_disable(&task->tasklet);
        tasklet_kill(&task->tasklet);
        list_del(&task->node);
        kfree(task);
    }
    up(&msec_queue_lock);
    if(tmp==&msec_queue) return -ENOENT;
    return 0;
}
/*! @} -------------------------------------------------------------------------------------------*/


EXPORT_SYMBOL(msec_sched_add_task);
EXPORT_SYMBOL(msec_sched_remove_task);
