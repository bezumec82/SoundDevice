#ifndef __HAL_H
#define __HAL_H

/*! @file hal.h
 * @brief HAL header.
 */

/*! @cond DO_NOT_SHOW ----------------------------------------------------------------------------*/
#include <asm/io.h>
/*! @endcond -------------------------------------------------------------------------------------*/

#include "hal-ioctl.h"


/*! @addtogroup Type_definitions
 * @{ --------------------------------------------------------------------------------------------*/
typedef volatile u8*    ptr_t;
/*! @} -------------------------------------------------------------------------------------------*/

/*! @name Parameters get/put
 * @{ */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#define CHECK_PARAM(ptr, acc) if (verify_area(acc, ptr, sizeof(*(ptr)))) { return -EFAULT; }
#else
#define CHECK_PARAM(ptr, acc) do {;} while(0)
#endif

#define GET_PARAM(local_var, src) get_user(local_var, (typeof(src) *)&(src))
#define PUT_PARAM(local_var, dest) put_user(local_var, (typeof(dest) *)&(dest))
/*! @} -------------------------------------------------------------------------------------------*/

/*! @name Timeslots facilities
 * @{ */
#define TDM_DSP_START   0
#define TDM_MAP_SIZE    4
struct channel;
typedef struct {
    /* codec name of selected channel */
    char* (*name) (struct channel*);
} codec_ops;
struct timeslot_info
{
    struct channel * channel;
    codec_ops ops;
};
/*! @} -------------------------------------------------------------------------------------------*/

/*! @name System clock facilities
 * @{ */
typedef void (*sysclk_notify_cb_t) (void *, int, u_long); ///< Prototype of notification callback.

int get_sysclk_state            (u_long * freq, u_long * freq_frac);
int get_sysclk_change           (void);
int notify_add_sysclk_change    (sysclk_notify_cb_t cb, void * ctx);
int notify_remove_sysclk_change (sysclk_notify_cb_t cb, void * ctx);

/* NOT USED */
// u_long gptmr_get_tick_count(u_long * high);
// u_long gptmr_ticks_to_us(u_long ticks);
/*! @} -------------------------------------------------------------------------------------------*/

/*! @name DSP facilities 
 * @{ */
#define DSP_NUMBER_DONT_CARE                  (-1)
#define DSP_OWNER_MAXNAMELEN                  64

#include "dsp-hal.h"

int     dsp_get_last_number     (void);
u32     dsp_get_pm_overlay_mask (dsp_t);
u32     dsp_get_dm_overlay_mask (dsp_t);
ptr_t   dsp_get_addr_ptr        (dsp_t);
ptr_t   dsp_get_data_ptr        (dsp_t);

void    dsp_upload_image        (dsp_t, u16 *, int, int);
void    dsp_upload_image_nolock (dsp_t, u16 * image, int size, int image_type);
dsp_t   dsp_lookup              (int number);
int     dsp_allocate            (int, dsp_t *, char *);
void    dsp_free                (dsp_t);

void    dsp_reset               (dsp_t);
void    dsp_unreset             (dsp_t);
void    dsp_enable_poll         (dsp_t);
void    dsp_disable_poll        (dsp_t);
void    dsp_enable_interrupt    (dsp_t);
void    dsp_disable_interrupt   (dsp_t);

void    dsp_set_int_handler     (dsp_t, dsp_handler_t, void *);
void    dsp_set_int_hang_handler(dsp_t, dsp_handler_t, void *);
void    dsp_set_poll_handler    (dsp_t, dsp_handler_t, void *);
int     dsp_get_number          (dsp_t);
const char * dsp_get_owner      (dsp_t);
int     dsp_alloc_tdm           (dsp_t dsp, int num_chns, int from_pref, int strict, int *chns);
void    dsp_free_tdm            (dsp_t dsp, int chn);

/* store, get and remove channels from/to channels-database */
int     register_channel        (int ts, struct channel*, codec_ops ops);
struct  channel* tdm_map_context(unsigned int number);
int     unregister_channel      (int ts);
/*! @} -------------------------------------------------------------------------------------------*/


/*! @name Initialization facilities
 * @{ */
/*!
 * Initialization structure.
 * Used in 'init.c'.
 */
struct init_struct
{
    int  (* constructor) (void * context);  /*!< Pointer to the constructor function. */
    void (* destructor)  (void * context);  /*!< Pointer to the destructor function. */
    int initialized;                        /*!< This flag will be set in #init_startup function. */
    int enabled;                            /*!<    This flag is set at the beginning. 
                                                    And checked in #init_startup function */
    const char * const enable_name;
};


#define INIT_STRUCT_NAME(inst) inst##_init_struct

/*! @cond DO_NOT_SHOW ----------------------------------------------------------------------------*/
/// Generates array of #init_struct.
#define INIT_PROLOGUE(inst)     static struct init_struct inst##_init_struct[] = {
#define   INIT_PART(class, init, en)    { class##_init, class##_deinit, 0,  en, init },
#define   INIT_PART_DEFAULT(class)      { class##_init, class##_deinit, 0,  1,  NULL },
#define INIT_EPILOGUE           };
/*! @endcond -------------------------------------------------------------------------------------*/

/// Returns amount of initialized items
#define INIT_NUM_ITEMS(inst)    (sizeof(inst##_init_struct) / sizeof(struct init_struct))

int init_startup    (
                    void * context, 
                    struct init_struct * initstruct, 
                    int nitems, 
                    char ** enstr, 
                    int nenstr 
                    );
void init_shutdown  (
                    void * context, 
                    struct init_struct * initstruct, 
                    int nitems
                    );
/*! @} -------------------------------------------------------------------------------------------*/

/*! @name Miscellaneous facilities
 * @{ */
#ifdef DECLARE_WAITQUEUE
#define WAIT_QUEUE(name) wait_queue_head_t      name
#else
#define WAIT_QUEUE(name) struct wait_queue      *##name##
#endif

#ifndef MIN
# define MIN(a,b)   ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
# define MAX(a,b)   ((a) > (b) ? (a) : (b))
#endif
enum {
    LEVEL_ACTIVE_LOW = 0,
    LEVEL_ACTIVE_HIGH = 1,
};

const char * errno_str(int errno);
/*! @} -------------------------------------------------------------------------------------------*/

/*! @brief Framer related facilities
 * @{ */
//    u_long framer_region_size(void);
//    void lamp_sema_up(void);
//    void lamp_sema_down(void);
/*! @} -------------------------------------------------------------------------------------------*/

#endif
