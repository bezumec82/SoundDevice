#ifndef __MSEC_SCHED_H_
#define __MSEC_SCHED_H_

/*! @file msec-sched.h */

extern struct list_head msec_queue;

/*!
 * @{ --------------------------------------------------------------------------------------------*/
struct msec_task
{
    struct list_head node;          ///< Node of #msec_queue list. 
    struct tasklet_struct tasklet;  ///< Tasklet to run.
    
/// After tasklet was scheduled, 
/// #msec_task.counter will be restored from this value.
    unsigned int period;
    
/// Counter decrementing in #msec_scheduler.
/// When counter achieves 0, tasklet will be scheduled.
    unsigned int counter;           
};
/*! @} -------------------------------------------------------------------------------------------*/


int msec_sched_init(void *context);
void msec_sched_deinit(void *context);
void msec_scheduler(int ts_inc);
int msec_sched_add_task(void (*funct)(unsigned long), unsigned long, uint period);
int msec_sched_remove_task(void (*funct)(unsigned long), unsigned long);
int msec_sched_run_now(void (*funct)(unsigned long), unsigned long data);

#endif /* __MSEC_SCHED_H_ */

