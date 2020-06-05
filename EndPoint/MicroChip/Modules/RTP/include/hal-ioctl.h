#ifndef __HAL_IOCTL_H
#define __HAL_IOCTL_H

/*! @file hal-ioctl.h */

#ifndef _IOR
#include <sys/ioctl.h>
#endif

/* device name /dev/hal */

#define HAL_FATAL_ERROR_IGNORE              0
#define HAL_FATAL_ERROR_KERNEL_PANIC        1

#ifndef DSP_OWNER_MAXNAMELEN
#define DSP_OWNER_MAXNAMELEN                64
#endif

#ifndef FRAMER_OWNER_MAXNAMELEN
#define FRAMER_OWNER_MAXNAMELEN             64
#endif


#define IN
#define OUT

#define DSP_BASE                'D'
#define FRAMER_BASE             'F'
#define MUX_BASE                'M'
#define CLOCK_BASE              'C'
#define MISC_BASE               'B'
#define LAMP_BASE               'L'
#define FLASH_BASE              'E'
#define DSPROC_BASE             'd'

#define param_read_dsp param_dsp_mem
#define param_write_dsp param_dsp_mem

#define HAL_GET_CLOCK           _IOR(CLOCK_BASE,    0, struct param_get_clock)
#define HAL_SET_CLOCK           _IOW(CLOCK_BASE,    1, struct param_set_clock)
#define HAL_GET_SYS_CLK_STATE   _IOR(CLOCK_BASE,    1, struct param_get_sysclk_state)
    
#define HAL_READ_DSP            _IOWR(DSP_BASE,     0, struct param_read_dsp)
#define HAL_WRITE_DSP           _IOW(DSP_BASE,      1, struct param_write_dsp)
#define HAL_RESET_DSP           _IOW(DSP_BASE,      2, struct param_reset_dsp)
#define HAL_DSP_OWNER           _IOWR(DSP_BASE,     3, struct param_dsp_owner)
#define HAL_DSP_GET_MAX         _IOR(DSP_BASE,      4, struct param_dsp_get_max)
#define HAL_DSP_UPLOAD          _IOR(DSP_BASE,      5, struct param_dsp_upload)
#define HAL_TDM_FIRST_BUS       _IOR(DSP_BASE,      6, struct param_tdm_bus_number)
#define HAL_TDM_LAST_BUS        _IOR(DSP_BASE,      7, struct param_tdm_bus_number)
#define HAL_TDM_BUS_INFO        _IOWR(DSP_BASE,     8, struct param_tdm_bus_info)

#define HAL_READFRM             _IOWR(FRAMER_BASE,  0, struct param_read_framer)
#define HAL_WRITEFRM            _IOW(FRAMER_BASE,   1, struct param_write_framer)
#define HAL_FRM_OWNER           _IOWR(FRAMER_BASE,  2, struct param_framer_owner)
#define HAL_FRM_GET_MAX         _IOR(FRAMER_BASE,   3, struct param_framer_get_max)

#define HAL_READ_MUX            _IOWR(MUX_BASE,     0, struct param_read_mux)
#define HAL_WRITE_MUX           _IOW(MUX_BASE,      1, struct param_write_mux)
#define HAL_MUX_CONNECT         _IOW(MUX_BASE,      2, struct param_connect)
#define HAL_MUX_DISCONNECT      _IOW(MUX_BASE,      3, struct param_disconnect)
#define HAL_MUX_MSG_WR          _IOWR(MUX_BASE,     4, struct mux_msg_write)
#define HAL_MUX_MSG_RD          _IOWR(MUX_BASE,     5, struct mux_msg_read)
#define HAL_MUX_READ_CM         _IOWR(MUX_BASE,     6, struct param_read_mux)
#define HAL_MUX_READ_DM         _IOWR(MUX_BASE,     7, struct param_read_mux)
#define HAL_MUX_ON_FATAL_ERROR  _IOW(MUX_BASE,      8, int)

#define HAL_MAP_MMCR            _IOWR(MISC_BASE,    0, union param_map_mmcr)

#define HAL_LAMP_SEMA_UP        _IO(LAMP_BASE,      0)
#define HAL_LAMP_SEMA_DOWN      _IO(LAMP_BASE,      1)

#define HAL_FLASH_SETUP         _IOW(FLASH_BASE,    0, u_int)
#define HAL_FLASH_UNSETUP       _IO(FLASH_BASE,     1)
#define HAL_FLASH_WRITE         _IOW(FLASH_BASE,    2, struct param_flash_write)
#define HAL_FLASH_READ          _IOWR(FLASH_BASE,   3, struct param_flash_read)

#define HAL_READ_MEM            _IOWR(MISC_BASE,    1, struct param_mem)
#define HAL_WRITE_MEM           _IOR(MISC_BASE,     2, struct param_mem)

#define HAL_READ_SPI            _IOWR(MISC_BASE,    3, struct param_spi)
#define HAL_WRITE_SPI           _IOWR(MISC_BASE,    4, struct param_spi)

#define CLK_SEL_FRM_0       0
#define CLK_SEL_FRM_1       1
#define CLK_SEL_FRM_3       2
#define CLK_SEL_FRM_4       3
#define CLK_SEL_FREERUN     4
#define CLK_SEL_X3          5
#define CLK_SEL_MASK        0x7F

#define CLK_TCLK_EN         0x80
#define CLK_TCLK_DIS        0x00
#define CLK_TCLK_MASK       0x80

/*!
 * @{ @brief Clock ioctls. */
struct param_get_clock {
    OUT u_int clock;
};

struct param_set_clock {
    IN u_int clock;
};
/*! @} -------------------------------------------------------------------------------------------*/

/*!
 * @{ @brief DSP ioctls. */
#define OVERLAY_PAGE_PM_SHIFT 16

#define OVERLAY_PAGE(pm, dm) (((u_int)((u_short)pm) << OVERLAY_PAGE_PM_SHIFT) | ((u_int)((u_short)dm)))
#define OVERLAY_PAGE_DM(ovr) ((u_short)((u_int)ovr))
#define OVERLAY_PAGE_PM(ovr) ((u_short)(((u_int)ovr) >> OVERLAY_PAGE_PM_SHIFT))


struct param_tdm_bus_number {
    IN u_int value;
};

struct param_tdm_bus_info {
    IN u_int bus_number;
    OUT u_int channels;
    OUT u_int free_channels;
};

struct param_spi {
    IN u_char device;
    IN u_char addr;
    IN u_char data;
};
struct param_dsp_mem {
    IN u_int number;
    IN u_int overlay_page;
    IN u_int address;
    IN OUT u_int data;
}; //__attribute__((packed));
struct param_reset_dsp {
    IN u_int number;
    IN u_int action;
};

#define RESETDSP_ACTION_RESET    0
#define RESETDSP_ACTION_UNRESET  1

struct param_dsp_owner {
    IN u_int number;
    IN char owner_name[DSP_OWNER_MAXNAMELEN + 1];
    IN u_int bus;
    IN u_int ts_base;
    IN u_int ts_count;
};
struct param_dsp_get_max {
    OUT u_int max;
};
struct param_dsp_upload {
    IN u_int number;
    IN u_short   *code;
    IN u_int size;
};
/*! @} -------------------------------------------------------------------------------------------*/


/*!
 * @{ @brief Framer ioctls. */
struct param_read_framer {
    IN u_int number;
    IN u_int address;
    OUT u_int data;
};
struct param_write_framer {
    IN u_int number;
    IN u_int address;
    IN u_int data;
};
struct param_framer_owner {
    IN u_int number;
    IN char owner_name[FRAMER_OWNER_MAXNAMELEN + 1];
};
struct param_framer_get_max {
    OUT u_int max;
};
/*! @} -------------------------------------------------------------------------------------------*/

/*!
 * @{ @brief MUX ioctls. */
#define MUX_ADDR_CFG           0x01000000
#define MUX_ADDR_CM_BACKPLANE  0x02000000
#define MUX_ADDR_CM_LOCAL_LOW  0x03000000
#define MUX_ADDR_CM_LOCAL_HIGH 0x04000000
#define MUX_ADDR_DM_BACKPLANE  0x05000000
#define MUX_ADDR_DM_LOCAL      0x06000000

struct param_read_mux {
    IN u_int address;
    OUT u_int data;
};
struct param_write_mux {
    u_int address;
    u_int data;
};
struct param_connect {
    IN u_int destination;
    IN u_int source;
};
struct param_disconnect {
    u_int channel;
};
struct param_get_sysclk_state {
    int prognosed_state;
    u_long freq;
    u_long freq_frac;   /* thousands of HZ (i.e. double FREQ = freq + freq_frac / 1000) */
};
/*! @} -------------------------------------------------------------------------------------------*/


#define CLOCK_PROGNOSED_STATE_INVALID 0
#define CLOCK_PROGNOSED_STATE_NORMAL  1

/*!
 * @{ */
union param_map_mmcr {
    struct {
        int unused;
    } in;
    struct {
        caddr_t mapped_addr;
        int size;
    } out;
};
struct param_flash_write {
    unsigned int addr;
    int data_size;
    int count;
};
struct param_flash_read {
    unsigned int addr;
    int data_size;
    int count;
};
struct param_mem {
    unsigned long addr;
    char data;
};

struct mux_msg_write {
    unsigned int channel;
    unsigned int data;
};
struct mux_msg_read {
    unsigned int channel;
    unsigned int data;
};
/*! @} -------------------------------------------------------------------------------------------*/


/*!
 * @{ @brief DTMF-related structures. */
#define DTMF_DEVNAME "dtmf"

struct dtmf_dial_params
{
    IN unsigned short f1;
    IN unsigned short f2;
    IN unsigned short a1;
    IN unsigned short a2;
};
struct channel_dial_params
{
    unsigned int    ts;
    struct dtmf_dial_params dp;
};
typedef struct dtmf_event
{
    unsigned int code;
    unsigned int timeslot;
    unsigned int timestamp;
    unsigned int    id;
} dtmf_event_t;
struct dtmf_detector_cfg
{
    unsigned int    ts;
    unsigned int    twist;
    unsigned int    ratio;
    unsigned int    silence;
    unsigned int    noise;
};
struct dtmf_enable
{
    unsigned int    timeslot;
    unsigned int    id;
};
/*! @} -------------------------------------------------------------------------------------------*/

/*!
 * @{ */
typedef enum {
    CHANNEL_CLASS_NONE  = 0,    /*!< not exists */
    CHANNEL_CLASS_TRUNK = 1,    /*!< point-to-point connection (E1, T1, BRI)              */
    CHANNEL_CLASS_BUS   = 2,    /*!< point-to-multipoint connection (ST-BUS)              */
    CHANNEL_CLASS_DSP   = 3,    /*!< complex TDM to network bridge (firmware configured)  */
    CHANNEL_CLASS_MCC   = 4,    /*!< trivial TDM to network bridge (persistent)           */
} channel_type_t;

typedef enum {
    BPS_NONE    = 0,
    BPS_300     = 1,
    BPS_600     = 2,
    BPS_1200    = 3,
    BPS_2400    = 4,
    BPS_4800    = 5,
    BPS_7200    = 6,
    BPS_9600    = 7,
    BPS_12000   = 8,
    BPS_14400   = 9,
    BPS_19200   = 10,
    BPS_28800   = 11,
    BPS_33600   = 12,
    BPS_56000   = 13,
    BPS_8000    = 14,   /*!< voice emulation: G729, G723 */
    BPS_64000   = 15    /*!< voice emulation: G711 and ISDN BRI */
} modem_bps;

typedef enum {
    MODE_NONE   = 0,
    MODE_G711A  = 1,
    MODE_G711U  = 2,
    MODE_G723H  = 3,
    MODE_G723L  = 4,
    MODE_G729   = 5,
    MODE_V21    = 6,
    MODE_V27    = 7,
    MODE_V29    = 8,
    MODE_V17    = 9,
    MODE_V34    = 10,
    MODE_V23    = 11,
    MODE_V90    = 12,
} channel_mode_t;

typedef enum {
    CHANNEL_FREE = 0,
    CHANNEL_BUSY = 1,
    CHANNEL_FAIL = 2,
} channel_state_t;
/*! @} -------------------------------------------------------------------------------------------*/

#define FEATURE(n) (1<<n)

struct channel_info
{
    unsigned int ts;            /*!< timeslot */
    channel_type_t type;        /*!< codec class. current supported DSP only */
    channel_state_t state;      /*!< busy, failure */
    /* use FEATURE macro */
    unsigned int ecaps;         /*!< encoder capability set   */
    unsigned int dcaps;         /*!< decoder capability set   */
    unsigned int emode;         /*!< current encoder mode     */
    unsigned int dmode;         /*!< current decoder mode     */
    struct {
        unsigned char n_chs;    /*!< number of inputs. if zero - ignore next field */
        unsigned char base;     /*!< first input timeslot     */
    } mixer;
};

#define DTMF_CLEAR_QUEUE        _IOR(MISC_BASE, 1, unsigned int)
#define DTMF_CONFIGURE_DETECTOR _IOR(MISC_BASE, 2, struct dtmf_detector_cfg)
#define DTMF_ENABLE_DETECTOR    _IOR(MISC_BASE, 3, struct dtmf_enable)
#define DTMF_DISABLE_DETECTOR   _IOR(MISC_BASE, 4, unsigned int)
/**/
#define DTMF_DIAL               _IOR(MISC_BASE, 5, struct channel_dial_params)

/*!
 * @{ @brief Enable/disable echo compensation. */
#define CHANNEL_LMS_ON          _IOR(MISC_BASE, 6, unsigned int)
#define CHANNEL_LMS_OFF         _IOR(MISC_BASE, 7, unsigned int)
#define CHANNEL_FEATURE         _IOR(MISC_BASE, 8, struct channel_info)
/*! @} -------------------------------------------------------------------------------------------*/

/*! 
 * @{ @brief Digital signal processing structures. */
#define DSPROC_DEVNAME "dsp"

enum {
    VAD_DIR_RX_MASK     = 0x01,
    VAD_DIR_TX_MASK     = 0x02
};
enum {
    VAD_DIR_RX          = 0,
    VAD_DIR_TX          = 1
};
enum {
    DSPROC_TYPE_NONE    = 0x0000,   /* none type */
    DSPROC_TYPE_VAD     = 0x0001,    /* VADs */
    DSPROC_TYPE_ALL     = 0xFFFF     /* All DSP algorithms */
};

typedef struct dsproc_vad_event
{
    unsigned int        dir_mask;   /* RX | TX */
    int                 value[2];   /* 0  | 1  */
} dsproc_vad_event_t;

struct vad_config
{
    int                 vad_amp;    /* 0x0000 - 0x7FFF, 0x0000 - disable VAD */
};

typedef struct dsproc_vad_config
{
    unsigned int        dir_mask;   /* RX / TX */
    struct vad_config   config[2];
} dsproc_vad_config_t;

typedef struct dsproc_event
{
    unsigned int    ts;
    unsigned int    id;
    unsigned int    dsproc_type;
    union
    {
        struct dsproc_vad_event vad;
    } payload;
} dsproc_event_t;

typedef struct dsproc_config
{
    unsigned int    ts;
    unsigned int    dsproc_type;
    union
    {
        struct dsproc_vad_config    vad;
    } config;
} dsproc_config_t;

struct dsproc_enable
{
    unsigned int    ts;
    unsigned int    id;
    unsigned int    dsproc_type_mask;
};

#define DSPROC_CONFIGURE    _IOR(DSPROC_BASE, 1, struct dsproc_config)
#define DSPROC_ENABLE       _IOR(DSPROC_BASE, 2, struct dsproc_enable)
#define DSPROC_DISABLE      _IOR(DSPROC_BASE, 3, struct dsproc_enable)
/*! @} -------------------------------------------------------------------------------------------*/

struct flash_context {
    int par_id;
    void *map;
    struct semaphore lock;
    u_int map_size;
};
struct hal_fd_context {
    int lamp_sema_info;
    struct flash_context flash;
};

/*! @{ @brief Defined in hal-ioctl-tdm.c */
int hal_ioctl_get_tdm_first_bus (struct file *file, struct param_tdm_bus_number *parm);
int hal_ioctl_get_tdm_last_bus  (struct file *file, struct param_tdm_bus_number *parm);
int hal_ioctl_get_tdm_bus_info  (struct file *file, struct param_tdm_bus_info *parm);
/*! @} -------------------------------------------------------------------------------------------*/

/*! @{ @brief Defined in hal-ioctl-sysclk.c */
int hal_ioctl_get_sysclk_state  (struct file *file, struct param_get_sysclk_state *parm);
int hal_ioctl_set_clock         (struct file *file, struct param_set_clock *parm);
int hal_ioctl_get_clock         (struct file *file, struct param_get_clock *parm);
/*! @} -------------------------------------------------------------------------------------------*/

/*! @{ @brief Defined in hal-ioctl-dsp.c */
int hal_ioctl_read_dsp      (struct file *file, struct param_read_dsp *parm);
int hal_ioctl_write_dsp     (struct file *file, struct param_write_dsp *parm);
int hal_ioctl_reset_dsp     (struct file *file, struct param_reset_dsp *parm);
int hal_ioctl_dsp_owner     (struct file *file, struct param_dsp_owner *parm);
int hal_ioctl_dsp_get_max   (struct file *file, struct param_dsp_get_max *parm);
int hal_ioctl_dsp_upload    (struct param_dsp_upload *ios);
/*! @} -------------------------------------------------------------------------------------------*/

/*! @{ @brief Defined in hal-ioctl-mux.c */
int hal_ioctl_read_mux              (struct file *file, struct param_read_mux *parm);
int hal_ioctl_write_mux             (struct file *file, struct param_write_mux *parm);
int hal_ioctl_mux_connect           (struct file *file, struct param_connect *parm);
int hal_ioctl_mux_disconnect        (struct file *file, struct param_disconnect *parm);
int hal_ioctl_write_mux_msg         (struct file *file, struct mux_msg_write *ios);
int hal_ioctl_read_mux_msg          (struct file *file, struct mux_msg_read *ios);
int hal_ioctl_mux_on_fatal_error    (struct file *file, int action);
/*! @} -------------------------------------------------------------------------------------------*/

/*! @{ @brief Defined in hal-ioctl-flash.c */
int hal_ioctl_flash_setup       (struct file *file);
int hal_ioctl_flash_unsetup     (struct file *file);
int hal_ioctl_flash_write       (struct file *file, struct param_flash_write *ios);
int hal_ioctl_flash_read        (struct file *file, struct param_flash_read *ios);
/*! @} -------------------------------------------------------------------------------------------*/

/*! @{ @brief Defined in hal-ioctl-mem.c */
int hal_ioctl_read_mem  (struct file *file, struct param_mem *ios);
int hal_ioctl_write_mem (struct file *file, struct param_mem *ios);
/*! @} -------------------------------------------------------------------------------------------*/

/*! @{ @brief Defined in hal-ioctl-misc.c */
int hal_ioctl_map_mmcr          (struct file *file, union param_map_mmcr *parm);
int hal_ioctl_lamp_sema_up      (struct file *file);
int hal_ioctl_lamp_sema_down    (struct file *file);
int hal_ioctl_read_cmdm         (struct file *file, struct param_read_mux *ios, int do_read_cm);
/*! @} -------------------------------------------------------------------------------------------*/

#endif
/*--- End of the file ----------------------------------------------------------------------------*/
