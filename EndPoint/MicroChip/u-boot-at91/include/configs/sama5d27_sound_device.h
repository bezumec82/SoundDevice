/* microElk */

#ifndef __CONFIG_H
#define __CONFIG_H

/* #include "at91-sama5_common.h" */
#include <asm/hardware.h>

#define CONFIG_BAUDRATE                 115200

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN           (4 * 1024 * 1024)

#undef  CONFIG_SYS_TEXT_BASE
#undef  CONFIG_SYS_AT91_MAIN_CLOCK
#define CONFIG_SYS_TEXT_BASE            0x23f00000

/* ARM asynchronous clock */
#define CONFIG_SYS_AT91_SLOW_CLOCK      32768
#define CONFIG_SYS_AT91_MAIN_CLOCK      24000000 /* from 24 MHz crystal */

#define CONFIG_ARCH_CPU_INIT
#define CONFIG_MISC_INIT_R

#ifndef CONFIG_SPL_BUILD
#define CONFIG_SKIP_LOWLEVEL_INIT
#endif

#define CONFIG_ENV_VARS_UBOOT_CONFIG

/* general purpose I/O */
#ifndef CONFIG_DM_GPIO
#define CONFIG_AT91_GPIO
#endif

/* SDRAM */
#define CONFIG_NR_DRAM_BANKS            1
#define CONFIG_SYS_SDRAM_BASE           ATMEL_BASE_DDRCS
#define CONFIG_SYS_SDRAM_SIZE           0x08000000

#ifdef CONFIG_SPL_BUILD
#define CONFIG_SYS_INIT_SP_ADDR         0x218000
#else

#define CONFIG_SYS_INIT_SP_ADDR         (CONFIG_SYS_SDRAM_BASE \
                                        + 16 * 1024 \
                                        - GENERATED_GBL_DATA_SIZE)
#endif


#define CONFIG_SYS_LOAD_ADDR            0x20000000 /* load address */

/* NAND flash */
#undef CONFIG_CMD_NAND

/* SPI flash */
#define CONFIG_SF_DEFAULT_SPEED         66000000

/* EEPROM MAC */
#define MAC24AA_I2C_BUS_NUM             0
#define MAC24AA_ADDR                    0x50
#define MAC24AA_REG                     0xfa

/*** SD BOOT ***/
#  ifdef  CONFIG_SD_BOOT
/* u-boot env in sd/mmc card */
#define CONFIG_ENV_IS_IN_FAT
#define CONFIG_FAT_WRITE
#define FAT_ENV_INTERFACE               "mmc"
#define FAT_ENV_DEVICE_AND_PART         "0"
#define FAT_ENV_FILE                    "uboot.env"
#define CONFIG_ENV_SIZE                 0x4000
/* bootstrap + u-boot + env in sd card */
# undef  CONFIG_BOOTCOMMAND
# define CONFIG_BOOTCOMMAND     "fatload mmc 0:1 0x21000000 sama5d27_sound_device.dtb; "    \
                                "fatload mmc 0:1 0x22000000 zImage; "                       \
                                "bootz 0x22000000 - 0x21000000"

# undef CONFIG_BOOTARGS
# define CONFIG_BOOTARGS        "console=ttyS0,115200 earlyprintk root=/dev/mmcblk0p2 rw rootwait"

#define CONFIG_SPL_LDSCRIPT                     arch/arm/mach-at91/armv7/u-boot-spl.lds
#define CONFIG_SYS_MMCSD_FS_BOOT_PARTITION      1
#define CONFIG_SPL_FS_LOAD_PAYLOAD_NAME         "u-boot.img"
#  endif

/*** QSPI BOOT ***/
#  ifdef CONFIG_QSPI_BOOT
#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_OFFSET               0xb0000
#define CONFIG_ENV_SIZE                 0x10000
#define CONFIG_ENV_SECT_SIZE            0x10000

# undef  CONFIG_BOOTCOMMAND
# define CONFIG_BOOTCOMMAND     "sf probe 0; "                          \
                                "sf read 0x21000000 0xc0000 0x20000; "  \
                                "sf read 0x22000000 0xe0000 0x400000; " \
                                "bootz 0x22000000 - 0x21000000"

# undef CONFIG_BOOTARGS
# define CONFIG_BOOTARGS        "console=ttyS0,115200 earlyprintk root=/dev/mmcblk0p2 rw rootwait"

#define CONFIG_SPL_SPI_LOAD
#define CONFIG_SYS_SPI_U_BOOT_OFFS      0x10000

#  endif

/*** QSPI+SD BOOT ***/
#  ifdef CONFIG_QSPI_SD_BOOT
#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_OFFSET               0x000b0000
#define CONFIG_ENV_SIZE                 0x00010000
#define CONFIG_ENV_SECT_SIZE            0x00010000

# undef  CONFIG_BOOTCOMMAND
# define CONFIG_BOOTCOMMAND     "fatload mmc 0:1 0x21000000 sama5d27_sound_device.dtb; "    \
                                "fatload mmc 0:1 0x22000000 zImage; "                       \
                                "bootz 0x22000000 - 0x21000000"

# undef CONFIG_BOOTARGS
# define CONFIG_BOOTARGS        "console=ttyS0,115200 earlyprintk root=/dev/mmcblk0p2 rw rootwait"

#define CONFIG_SPL_SPI_LOAD
#define CONFIG_SYS_SPI_U_BOOT_OFFS              0x10000
#define CONFIG_SYS_MMCSD_FS_BOOT_PARTITION      1
#  endif

/*** TFTP BOOT ***/
#if (1)
# ifdef CONFIG_TFTP_BOOT
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_ENV_SIZE         (128 << 10) /* 128 KiB */

#define CONFIG_SERVERIP         192.168.12.200
#define CONFIG_IPADDR           192.168.12.108

# undef CONFIG_BOOTCOMMAND
# undef CONFIG_EXTRA_ENV_SETTINGS

# define CONFIG_EXTRA_ENV_SETTINGS \
    "bootdelay=1\0"                                 \
    "ethaddr=00:0a:35:00:01:22\0"                   \
    "serverip="__stringify(CONFIG_SERVERIP)"\0"     \
    "ipaddr="__stringify(CONFIG_IPADDR)"\0"         \
    ""                                              \
    "kernel_image=/tftpboot/SoundDevice/uImage\0"   \
    "kernel_load_address=0x21000000\0"      \
    "kernel_size=0x00500000\0"              \
    ""                                      \
    "devicetree_image=/tftpboot/SoundDevice/at91-sama5d27_sound_device.dtb\0" \
    "devicetree_load_address=0x20f00000\0"  \
    "devicetree_size=0x00020000\0"          \
    ""                                      \
    "ramdisk_image=/tftpboot/SoundDevice/uramdisk.gz\0"    \
    "ramdisk_load_address=0x22000000\0"                    \
    "ramdisk_size=0x00A00000\0"                            \
    ""

# define CONFIG_BOOTCOMMAND                                                 \
        "echo Booting from tftp server: ${serverip} && "                    \
        "tftpboot ${kernel_load_address} ${kernel_image} && "               \
        "tftpboot ${devicetree_load_address} ${devicetree_image} && "       \
        "tftpboot ${ramdisk_load_address} ${ramdisk_image} && "             \
        "echo Loading LINUX && "                                            \
        "bootm ${kernel_load_address} ${ramdisk_load_address} ${devicetree_load_address}"

#if (1)
# undef CONFIG_BOOTARGS
#else
# define CONFIG_BOOTARGS                        "console=ttyS0,115200 earlyprintk root=/dev/ram rw"
#endif

#define CONFIG_SPL_SPI_LOAD
#define CONFIG_SYS_SPI_U_BOOT_OFFS              0x10000
#define CONFIG_SYS_MMCSD_FS_BOOT_PARTITION      1

# endif /* CONFIG_TFTP_BOOT */
#else /* #if (0) */
# ifdef CONFIG_TFTP_BOOT
#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_OFFSET               0x000b0000
#define CONFIG_ENV_SIZE                 0x00010000
#define CONFIG_ENV_SECT_SIZE            0x00010000

#define TFTP_SERVER_IP      "192.168.12.200"
#define OWN_IP_ADDR         "192.168.12.108"

#  define CONFIG_EXTRA_ENV_SETTINGS         \
    "bootdelay=1\0"                         \
    "ethaddr=00:0a:35:00:01:22\0"           \
    "serverip="TFTP_SERVER_IP"\0"           \
    "ipaddr="OWN_IP_ADDR"\0"                \
    ""                                      \
    "kernel_image=/tftpboot/SoundDevice/zImage\0" \
    "kernel_load_address=0x22000000\0"      \
    "kernel_size=0x400000\0"                \
    ""                                      \
    "devicetree_image=/tftpboot/SoundDevice/at91-sama5d27_sound_device.dtb\0" \
    "devicetree_load_address=0x21000000\0"  \
    "devicetree_size=0x40000\0"             \

# undef  CONFIG_BOOTCOMMAND
# define CONFIG_BOOTCOMMAND                                                 \
        "echo Booting from tftp server: ${serverip} && "                    \
        "tftpboot ${kernel_load_address} ${kernel_image} && "               \
        "tftpboot ${devicetree_load_address} ${devicetree_image} && "       \
        "echo Loading LINUX && "                                            \
        "bootz 0x22000000 - 0x21000000"                                     \

# undef CONFIG_BOOTARGS
# define CONFIG_BOOTARGS                        "console=ttyS0,115200 earlyprintk root=/dev/mmcblk0p1 rw rootwait"

#define CONFIG_SPL_SPI_LOAD
#define CONFIG_SYS_SPI_U_BOOT_OFFS              0x10000
#define CONFIG_SYS_MMCSD_FS_BOOT_PARTITION      1

# endif
#endif
/*
 * DDR : start = 0x20000000 size = 0x080000000
 * System map   size    size HEX        QSPI off. from 0x0          QSPI space given        load addr.
 * bootStr      32KiB   0x0000'8000     +0x0000'0000 (+O    KiB)    0x0001'0000 (64  KiB)
 * u-boot.bin   512KiB  0x0008'0000     +0x0001'0000 (+64   KiB)    0x000b'0000 (704 KiB)
 * dtb          128KiB  0x0002'0000     +0x000c'0000 (+768  KiB)    0x0004'0000 (256 KiB)   0x20f00000      +15MiB
 * kernel       5MiB    0x0050'0000     +0x0010'0000 (+1024 KiB)    0x0050'0000 (5   MiB)   0x21000000      +16MiB
 * init_rd      10MiB   0x00A0'0000     +0x0060'0000 (+6    MiB)    0x00A0'0000 (10  MiB)   0x22000000      +32MiB
 *                                      +0x0100'0000 (+16MiB=end)   -----------
 *                                                                  0x0100'0000
 *
setenv bootargs "console=ttyS0,115200 earlyprintk root=/dev/mmcblk0p1 rw rootwait"
 */

/* hash */
#define CONFIG_SYS_CBSIZE           2048
#define CONFIG_SYS_MAXARGS          32
#define CONFIG_SYS_LONGHELP
#define CONFIG_CMDLINE_EDITING
#define CONFIG_AUTO_COMPLETE

/* SPL */
# undef CONFIG_SPL_FRAMEWORK
#define CONFIG_SPL_TEXT_BASE        0x00200000
#define CONFIG_SPL_MAX_SIZE         0x00010000
#define CONFIG_SPL_BSS_START_ADDR   0x20000000
#define CONFIG_SPL_BSS_MAX_SIZE     0x00080000
#define CONFIG_SYS_SPL_MALLOC_START 0x20080000
#define CONFIG_SYS_SPL_MALLOC_SIZE  0x00080000

# undef CONFIG_SPL_BOARD_INIT
#define CONFIG_SYS_MONITOR_LEN      (512 << 10)

#endif
