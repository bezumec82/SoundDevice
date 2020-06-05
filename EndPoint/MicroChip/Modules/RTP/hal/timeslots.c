/*! @file timeslots.c */

#include "driver.h"
#include "msec-sched.h"
#include "hal-ioctl.h"

/// Matching timeslots, #channel and its operations.
static struct timeslot_info tdm_map [TDM_MAP_SIZE]; 

struct semaphore tdm_lock;

static char * dummy_name(struct channel* c) { return "none"; }

static codec_ops dummy_ops = {
    .name = dummy_name
};

#define GET_OWNER(ts)\
    if ((ts) < 0) return -ENODEV;\
    if ((ts) >= TDM_MAP_SIZE) return -ENODEV;\
    struct timeslot_info* owner = &tdm_map[(ts)];\
    if (owner == NULL) return -ENODEV;

/*------------------------------------------------------------------------------------------------*/



/*! @addtogroup Exported Exported to kernel functions
 * @{ --------------------------------------------------------------------------------------------*/
 /*!Returns #channel for asked TDM timeslot.
  * param@ timeslot - Timeslot of asked #channel structure.
  */
struct channel * tdm_map_context(unsigned int timeslot)
{
    if (timeslot >= TDM_MAP_SIZE) return NULL;
    return tdm_map[timeslot].channel;
}
/*--- End of function ----------------------------------------------------------------------------*/

/*! Add info about channel to the #tdm_map. 
 * @param ch_ptr    : Pointer to the #channel structure.
 * @param ops       : #codec_ops structure.
 */
int register_channel(int ts, struct channel * ch_ptr, codec_ops ops)
{
    struct timeslot_info* ti;
    __PRINTK(MAG, "Registering channel for TDM timeslot %d\n", ts);
    down(&tdm_lock);    
        ti  = &tdm_map[ts];
        if(ti->channel) {
            __PRINTK(RED, "Channel for TDM timeslot %d already registered.\n", ts);
            up(&tdm_lock);
            return -EBUSY;
        }
        ti->channel     = ch_ptr;
        ti->ops         = ops;
    up(&tdm_lock);
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

/*! Remove info about channel from the #tdm_map. 
 * @param timeslot : Time slot to free.
 */
int unregister_channel(int timeslot)
{
    struct timeslot_info * timeslot_info_ptr;
    __PRINTK(CYN, "Unregistering TDM timeslot #%i.\n", timeslot); 
    timeslot_info_ptr  = &tdm_map[timeslot];
    down(&tdm_lock);
        timeslot_info_ptr->channel  = NULL;
        timeslot_info_ptr->ops      = dummy_ops;
    up(&tdm_lock);
    return 0;
}
/*! @} -------------------------------------------------------------------------------------------*/



/*! @name Initialization facilities
 * @{ */
/*! TDM initialization function.
 * @param context - Not used in this routine.
 *                  Structure #hal_context will be passed by #init_startup.
 */
int tdm_init(void *context)
{
    int timeslot;
    __PRINTK(GRN, "Initializing TDM.\n"); 
    sema_init(&tdm_lock, 1);
    __PRINTK(YEL, "Unregistering all TDM channels.\n"); 
    for (timeslot=0; timeslot<TDM_MAP_SIZE;timeslot++){
        unregister_channel(timeslot);
    }
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/
int tdm_deinit(void *context) { return 0;}
/*! @} -------------------------------------------------------------------------------------------*/

EXPORT_SYMBOL(unregister_channel);
EXPORT_SYMBOL(register_channel);
EXPORT_SYMBOL(tdm_map_context);
/*--- End of the file ----------------------------------------------------------------------------*/
