/*! @file clk-common-notify.c */
#include "driver.h"
#include "msec-sched.h"

/*! @addtogroup Lists
 * @{ --------------------------------------------------------------------------------------------*/
struct list_head            notify_list;        ///< List for #sysclk_state_notify nodes.
struct semaphore            notify_list_lock;   ///< Semaphore to protect #notify_list.
/*! @} -------------------------------------------------------------------------------------------*/


/*! Removes all nodes from #notify_list.
 * No deallocation. 
 */
void notify_cleanup_list(void)
{
    struct list_head *node;
    struct sysclk_state_notify *notify;

    down(&notify_list_lock);

    for (   node = notify_list.next; 
            node != &notify_list; 
            node = notify_list.next) {
        list_del(node);
        notify = list_entry(node, struct sysclk_state_notify, node);
    }

    up(&notify_list_lock);
}
/*--- End of function --------------------------------------------------------------------------- */

/*! Find #notify_list node by provided notification callback and its parameter.
 * @param cb  : Notification callback function.
 * @param ctx : Parameter for notification callback function.
 */
struct sysclk_state_notify * find_notify_node (sysclk_notify_cb_t cb, void * ctx)
{
    struct list_head *node;
    struct sysclk_state_notify *notify;
    for (   node = notify_list.next; 
            node != &notify_list; 
            node = node->next) {
        notify = list_entry(node, struct sysclk_state_notify, node);
        if (notify->cb == cb && notify->ctx == ctx) {
            return notify;
        }
    }
    return NULL;
}
/*--- End of function --------------------------------------------------------------------------- */

/*! Find #notify_list node by provided notification callback and its parameter and deletes it.
 * @param cb  : Notification callback function.
 * @param ctx : Parameter of notification callback function.
 */
int delete_notify_node(sysclk_notify_cb_t cb, void * ctx)
{
    struct sysclk_state_notify *notify;

    down(&notify_list_lock);
    notify = find_notify_node(cb, ctx);
    if (!notify) {
        up(&notify_list_lock);
        return -ENOENT;
    }
    list_del(&notify->node);
    up(&notify_list_lock);
    kfree(notify);
    return 0;
}
/*--- End of function --------------------------------------------------------------------------- */

/*! Adds new node to the #notify_list.
 * @param cb  : Notification callback function.
 * @param ctx : Parameter of notification callback function.
 */
int add_notify_node(sysclk_notify_cb_t cb, void * ctx)
{
    struct sysclk_state_notify * notify;
    down(&notify_list_lock);
    
    /// -# Check that such node already exists.
    notify = find_notify_node(cb, ctx);
    if (notify) {
        up(&notify_list_lock);
        return -EBUSY;
    }
    /// -# Allocate new node.
    notify = kmalloc(sizeof(struct sysclk_state_notify), GFP_KERNEL);
    if (!notify) {
        up(&notify_list_lock);
        return -ENOMEM;
    }
    notify->cb = cb;
    notify->ctx = ctx;
    /// -# Add new node to the list.
    list_add_tail(&notify->node, &notify_list);
    
    up(&notify_list_lock);
    return 0;
}
/*--- End of function --------------------------------------------------------------------------- */

/// Wrapper around static variables #notify_list and #notify_list_lock.
/// @code
void init_notify (void)
{
    INIT_LIST_HEAD(&notify_list);
    sema_init(&notify_list_lock, 1);
}
/// @endcode
