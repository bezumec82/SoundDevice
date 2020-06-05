#ifndef __MCASP_RPMSG_MOD__
#define __MCASP_RPMSG_MOD__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/rpmsg.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <asm/cacheflush.h>
#include <linux/mm_types.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <uapi/linux/types.h>
#include <linux/time.h>

#define TRUE                    1
#define FALSE                   0

/*
 * Each PRU can handle only one MCASP.
 * Each MCASP has only one AFSX signal,
 * so I assume that ammount
 * of receive and transmit timeslots are equal.
 * For example I2S : two TX timeslots and two RX.
 */
#define TS_AMMNT                2
#define BUFS_AMMNT              16

#define NUM_ABUFS               ( TS_AMMNT * BUFS_AMMNT * 2 ) /* Read and write buffers. */
#define ABUFS_SIZE              NUM_ABUFS * PAGE_SIZE
#define RX_BUF_BASE_OFFSET      (uint32_t)( TS_AMMNT * BUFS_AMMNT * PAGE_SIZE )

#define BUF_SIZE_WORDS          64
#define BUF_SIZE_BYTES          BUF_SIZE_WORDS * 4

#define LINKED_BUFS             TRUE
#define PAGE_ALIGNED_BUFS       TRUE

#define DYNAMIC_BUF             FALSE
/* Debug switches */
#define DEBUG_BUF_FILL          TRUE
#define DEBUG_BUF_RUNTIME       FALSE
#define DEBUG_RPC               FALSE
#define DEBUG_W_BLOCK           FALSE
#define DEBUG_W_BLOCK_SEVERE    FALSE
#define DEBUG_R_BLOCK           FALSE
#define DEBUG_R_BLOCK_SEVERE    FALSE
#define DEBUG_POLL              FALSE
#define DEBUG_RX_FLAG           FALSE
#define DEBUG_RX_FLAG_SEVERE    FALSE
#define DEBUG_TX_FLAG           FALSE

/*!
 * Defines how many PRU-MCASP buffers
 * can-be written until process blocks.
 */
#define BUF_QUANTUM             5
#if (DEBUG_W_BLOCK)
    #define TX_BUF_GAP          1
#else
    #define TX_BUF_GAP          1
#endif

#define RX_BUF_GAP              BUF_QUANTUM


#define OVERKILL_FLUSH          FALSE
/*! This structure represents one node
 * in the linked chain of buffers
 * for the given direction.
 */
struct buffer {
    uint32_t *      buf;
#if (LINKED_BUFS)
    struct buffer * next;
#endif
};

/*!
 * I2S has two timeslots for RX and two for TX.
 * This structure represents data
 * for the time slot couple RX - TX.
 * One 'cdev' writes to TX bufs and reads from RX bufs
 * of the selected timeslot.
 */
struct ts_data {
    /// Char device part
    struct mutex            wr_mutex;           ///< Mutual exclusion semaphore for write operations.
    struct mutex            rd_mutex;           ///< Mutual exclusion semaphore for read operations.
    struct cdev             cdev;	            ///< Char device structure.
    struct class *          class;              ///< To create '/dev' node.
    char                    dev_name[64];       ///< Name of '/dev' node.
    void *                  private;            ///< To save data for callbacks.
    int                     ts_idx;             ///< To save timeslot idx responsible for the device.

    /// These indexes will be updated on each read-write operation.
    int16_t        tx_head;    ///< Represents last ARM write position.
    int16_t        rx_tail;    ///< Represents last ARM read position.
    ///< This flag shows that first transmission happend and tail was catched.
    ///< If head == tail this flag will be dropped.
    int                     catch_tx_tail;
    int                     catch_rx_tail;
    wait_queue_head_t       read_q;
    wait_queue_head_t       write_q;
    volatile int            can_write;  ///< Flag shows that userspace can write new chunk of data.
    volatile int            can_read;   ///< Flag shows that userspace can read new chunk of data.
    #if (DYNAMIC_BUF)
        char *              tx_buf;
        char *              rx_buf;
    #else
        char                tx_buf[BUF_QUANTUM * BUF_SIZE_BYTES];
        char                rx_buf[BUF_QUANTUM * BUF_SIZE_BYTES];
    #endif
};

struct statistics 
{
    uint64_t        cntr_tx_msg_ts1;
    struct timespec frst_tx_msg_ts1;
    struct timespec last_tx_msg_ts1;

    uint64_t        cntr_tx_msg_ts2;
    struct timespec frst_tx_msg_ts2;
    struct timespec last_tx_msg_ts2;

    uint64_t        cntr_rx_msg_ts1;
    struct timespec frst_rx_msg_ts1;
    struct timespec last_rx_msg_ts1;

    uint64_t        cntr_rx_msg_ts2;
    struct timespec frst_rx_msg_ts2;
    struct timespec last_rx_msg_ts2;
};

struct pru_mcasp_device
{
    struct device *         dev;
    struct rpmsg_device *   rpdev;
    void *                  abuf;
    struct page *           abuf_page;
    phys_addr_t             ddr_start_addr; //u32 underneath

    /// These indexes will be updated from RPC call back
    volatile uint8_t        tx_tail; ///<Represents current PRU-MCASP TX position.
    volatile uint8_t        rx_head; ///<Represents current PRU-MCASP RX position.

    struct buffer tx_bufs [TS_AMMNT] [BUFS_AMMNT];
    struct buffer rx_bufs [TS_AMMNT] [BUFS_AMMNT];

    /// Char device part
    dev_t                   dev_num; ///< Device numbers start region (MAJOR:MINOR)
    struct ts_data          ts_data [TS_AMMNT];
    struct statistics       stat;
};



#define DIRECTION_TX            0x1
#define DIRECTION_RX            0x2

/*! @addtogroup External variables
 * @{ */
extern struct statistics stat;
extern struct attribute_group pru_mcasp_group;
/*! @} */

/*! @addtogroup Console_output
 * @{ */
#define NORM  "\x1B[0m"   ///normal
#define RED   "\x1B[31m"  ///red
#define GRN   "\x1B[32m"  ///green
#define YEL   "\x1B[33m"  ///yellow
#define BLU   "\x1B[34m"  ///blue
#define MAG   "\x1B[35m"  ///magenta
#define CYN   "\x1B[36m"  ///cyan
#define WHT   "\x1B[37m"  ///white
#define BOLD  "\e[1m"     ///bold symbols
#define ITAL  "\e[3m"     ///italic text
#define BLINK "\e[5m"     ///blink


/* Strip down path from filename */
#define __FILENAME__    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#ifdef DEBUG
    # define __PRINTK(color, text, ...) \
                        printk(color"%-15.15s %-15.15s #%-5i: "text""NORM, \
                        __FILENAME__, __FUNCTION__, __LINE__ , ##__VA_ARGS__);

#else   /* #ifdef DEBUG */
    # define __PRINTK(color, text, ...)
#endif  /* #ifdef DEBUG */

#define __PRINTERR(text, ...)   __PRINTK(RED, "ERROR : "text""NORM, ##__VA_ARGS__);

#define __PRINTK_COND(condition, color, text, ... ) \
        if (condition) { __PRINTK(color, text""NORM, ##__VA_ARGS__); }
#define PRINTK_COND(condition, color, text, ... ) \
        if (condition) { printk(color""text""NORM, ##__VA_ARGS__); }
/*! @} */

/* Functions prototypes */
int pru_mcasp_create_dev_iface(struct pru_mcasp_device * pru_mcasp_dev_ptr);
int pru_mcasp_destroy_dev_iface(struct pru_mcasp_device * pru_mcasp_dev_ptr);

#endif  /* __MCASP_RPMSG_MOD__ */
