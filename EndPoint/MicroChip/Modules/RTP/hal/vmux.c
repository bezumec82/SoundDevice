#include "driver.h"
#include "vmux.h"
#include "msec-sched.h"

/*! @file vmux.c */

#define DEF_PACKET_FACTOR   2
#define MAX_PACKET_FACTOR   8 ///< Defines buffers size.

/// NOT USED
static int kernelPanicEnabled = 1;

/*! @addtogroup Lists
 * @{ --------------------------------------------------------------------------------------------*/
/*! @var mux_nodes - list to hold MUX channels couples. */
LIST_HEAD(mux_nodes);
/*! Semaphore to protect #mux_nodes from concurrency. */
struct semaphore mux_nodes_lock;
/*! @} -------------------------------------------------------------------------------------------*/

/// Temporary buffer for MUX operations.
static short mux_buff[ LIN16_DSP_FRAME_SIZE * MAX_PACKET_FACTOR ]; 
/// Decoder buffer.
static short dec_buff[ LIN16_DSP_FRAME_SIZE * MAX_PACKET_FACTOR];
/// Encoder buffer.
static short enc_buff[ LIN16_DSP_FRAME_SIZE * MAX_PACKET_FACTOR];
/*------------------------------------------------------------------------------------------------*/

/*! Moves through #mux_nodes list and copies 
 * one frame from source channel to the destination channel.
 * @param data : Is not used.
 */
static void process_mux(unsigned long data)
{

    channel_t           * channel_src;
    channel_t           * channel_dst;
    
    struct list_head    * loop_cursor;
    struct list_head    * tmp;
    
    struct mux_node     * mux;
    int                 mux_size, dec_size, enc_size;
    int                 nread, nwrite, frame_size;
    int                 payloadtype = 0;
    int                 empty = 0;

    __PRINTK_RATELTD(GRN, "Processing MUX.\n");

    mux_size = sizeof(mux_buff);
    dec_size = sizeof(dec_buff);
    enc_size = sizeof(enc_buff);
    
    down(&mux_nodes_lock);
        /// For each node in #mux_nodes list:
        list_for_each_safe(loop_cursor, tmp, &mux_nodes)
        {
            mux = list_entry(loop_cursor, struct mux_node, node);
            channel_src = mux->src;
            channel_dst = mux->dst;
            /// -1 Get frame size. 
            frame_size  = channel_src->frame_bytes(channel_src);
            /// -2 Get amount of decoded bytes.
            nread       = channel_src->decoded_bytes(channel_src);
            /// -3 If amount of decoded bytes bigger than frame:
            if (nread >= frame_size)
            {
                /// -3.1 read one frame from decoder buffer of the source channel in the #mux_buff. 
                nread = channel_src->read_dec_frame(channel_src, &payloadtype, mux_buff, frame_size);
                empty = (nread <= 0);
                /// -3.2 If payload type in both (source and destination) channels are equal:
                if (channel_src->decoder_payloadtype == channel_dst->encoder_payloadtype)
                {
                    /// -3.2.1 copy #mux_buff to the #enc_buff.
                    memcpy(enc_buff, mux_buff, nread);
                    nwrite = nread;
                    payloadtype = channel_dst->encoder_payloadtype;
                } else { /// -3.3 If not:
                    /// -3.3.1 decode #mux_buff to the #dec_buff,
                    nread = channel_src->decode_frame(channel_src, mux_buff, nread, dec_buff, dec_size);
                    /// -3.3.2 and then encode #dec_buff to the #enc_buff.
                    nread = channel_dst->encode_frame(channel_dst, dec_buff, nread, enc_buff, enc_size);
                    nwrite = nread;
                    payloadtype = channel_dst->encoder_payloadtype;
                } //end if (src->decoder_payloadtype ...
                /// -3.3 Write data from #enc_buff to the destination channel.
                nwrite  = channel_dst->write_enc_frame(channel_dst, payloadtype, enc_buff, nwrite);
                nread   = channel_src->decoded_bytes(channel_src);
            } //end if (nread >= frame_size)
        } //end list_for_each_safe
    up(&mux_nodes_lock);
}
/*--- End of function ----------------------------------------------------------------------------*/

/*! Adds source and destination timeslots to the #mux_nodes list.
 * @param dst : Destination timeslot.
 * @param src : Source timeslot.
 */
int mux_connect(u16 dst, u16 src)
{
    channel_t           * channel_src;
    channel_t           * channel_dst;
    struct mux_node     * mux;
    
    __PRINTK(GRN, "Connecting timeslots : src(%i) -> dst(%i).\n", src, dst);    

    /// -# Find according #channel structures for given source and destination timeslots from #tdm_map.
    channel_dst = (channel_t *)tdm_map_context(dst);
    channel_src = (channel_t *)tdm_map_context(src);
    
    if (dst == src) return -1;
    if ((channel_dst == NULL) || (!channel_dst->busy)) return -1;
    if ((channel_src == NULL) || (!channel_src->busy)) return -1;

    /// -# Allocate, setup and add new #mux_node for #mux_nodes list.
    if (!(mux = kmalloc(sizeof(*mux), GFP_ATOMIC)))
        return -1;

    mux->src    = channel_src;
    mux->dst    = channel_dst;
    mux->active = 1;
    
    down(&mux_nodes_lock);
        list_add(&mux->node, &mux_nodes);
    up(&mux_nodes_lock);
    
    jb_reset(channel_src);
    
    __PRINTK(GRN, "Timeslots src(%i) and dst(%i) is connected.\n", src, dst);    

    
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

/*! Disconnects given timeslot from its appropriate pair
 * and delete #mux_nodes list node.
 * @param timeslot : Timeslot to disconnect.
 */
int mux_disconnect(u16 timeslot)
{
    struct list_head    * loop_cursor, *tmp;
    channel_t           * channel_disc;
    struct mux_node     * mux;
    int                 count = 0;
    __PRINTK(GRN, "Disconnecting timeslot %i from its pair.\n", timeslot);    

    channel_disc = (channel_t *)tdm_map_context(timeslot);
    if (channel_disc == NULL) return -1;

    down(&mux_nodes_lock);
        list_for_each_safe(loop_cursor, tmp, &mux_nodes)
        {
            mux = list_entry(loop_cursor, struct mux_node, node);
            if ((mux->src == channel_disc) || (mux->dst == channel_disc))
            {
                list_del(&mux->node);
                kfree(mux);
                count++;
            }
        }
    up(&mux_nodes_lock);    
    if (count) __PRINTK(GRN, "Timeslot %i disconnected %i times.\n", timeslot, count);        

    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/



/*! @name Initialization facilities
 * @{ */
/*! MUX initialization routine.
 * Will be added to #init_struct and called from #init_startup function.
 * This function initializes kernel timer with callback #sys_clk_ctl_tmr_callback 
 * and timeout @a SYS_CLOCK_CHECK_INTERVAL.
 * @param context - Not used in this routine.
 *                  Structure #hal_context will be passed by #init_startup.
 */
int mux_init(void *context)
{
    __PRINTK(GRN, "Initializing MUX.\n"); 
    INIT_LIST_HEAD(&mux_nodes);
    sema_init(&mux_nodes_lock, 1);
    __PRINTK(GRN, "Adding task to custom scheduler.\n"); 
    msec_sched_add_task(process_mux, (unsigned long)context, MUX_PACKET_FACTOR);
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/
/*! MUX de-initialization routine.
 * @param context - Not used in this routine.
 *                  Structure #hal_context will be passed by #init_startup.
 */
void mux_deinit(void *context)
{
    struct mux_node *   mux;
    struct list_head *  p;
    /// -# Remove task from custom scheduler.
    msec_sched_remove_task(process_mux, (unsigned long)context);
    
    /// -# Delete all nodes from #mux_nodes list.
    /// -# Deallocate all #mux_nodes structures.
    down(&mux_nodes_lock);
        /// @bug  'list_for_each_safe' should be used instead of manual node deletion.
        /// @code
            while ( !list_empty(&mux_nodes) ) 
            {
                p = mux_nodes.next;
                list_del(p);
                mux = list_entry( p, struct mux_node, node );
                kfree(mux);
            }
        /// @endcode
    up(&mux_nodes_lock);    
}
/*! @} -------------------------------------------------------------------------------------------*/


void mux_set_kernel_panic_enabled( int enabled )
{
    kernelPanicEnabled = enabled;
    if( enabled )
        PRINTK(KERN_DEBUG, "kernel panic enabled\n");
    else
        PRINTK(KERN_DEBUG, "kernel panic disabled\n");
}
/*--- End of function ----------------------------------------------------------------------------*/
#if (0)
/// NOT USED
static void fatalError( void )
{
    if( kernelPanicEnabled )
        panic("FATAL ERROR: DSM i/o failure\n");
    else
        PRINTK(KERN_CRIT, " FATAL ERROR: DSM i/o failure\n");
}
#endif

/*--- End of the file ----------------------------------------------------------------------------*/
