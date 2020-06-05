/*
 * am335x_evm.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __CONFIG_AM335X_EVM_H
#define __CONFIG_AM335X_EVM_H

#include <configs/ti_am335x_common.h>
#include <linux/sizes.h>

#ifndef CONFIG_SPL_BUILD
    #define CONFIG_TIMESTAMP
#endif

#define CONFIG_SYS_BOOTM_LEN    SZ_16M

#define CONFIG_MACH_TYPE        MACH_TYPE_AM335XEVM

/* Clock Defines */
#define V_OSCK                  24000000  /* Clock output from T2 */
#define V_SCLK                  (V_OSCK)

/* Custom script for NOR */
#define CONFIG_SYS_LDSCRIPT     "board/ti/am335x/u-boot.lds"

/* Always 128 KiB env size */
#define CONFIG_ENV_SIZE         SZ_128K

#ifdef CONFIG_NAND
    #define NANDARGS \
        "mtdids=" CONFIG_MTDIDS_DEFAULT "\0" \
        "mtdparts=" CONFIG_MTDPARTS_DEFAULT "\0" \
        "nandargs=setenv bootargs console=${console} " \
            "${optargs} " \
            "root=${nandroot} " \
            "rootfstype=${nandrootfstype}\0" \
        "nandroot=ubi0:rootfs rw ubi.mtd=NAND.file-system,2048\0" \
        "nandrootfstype=ubifs rootwait=1\0" \
        "nandboot=echo Booting from nand ...; " \
            "run nandargs; " \
            "nand read ${fdtaddr} NAND.u-boot-spl-os; " \
            "nand read ${loadaddr} NAND.kernel; " \
            "bootz ${loadaddr} - ${fdtaddr}\0"
#else
    #define NANDARGS ""
#endif

#define BOOTENV_DEV_LEGACY_MMC(devtypeu, devtypel, instance) \
    "bootcmd_" #devtypel #instance "=" \
    "setenv mmcdev " #instance"; "\
    "setenv bootpart " #instance":2 ; "\
    "run mmcboot\0"

#define BOOTENV_DEV_NAME_LEGACY_MMC(devtypeu, devtypel, instance) \
    #devtypel #instance " "

#define BOOTENV_DEV_NAND(devtypeu, devtypel, instance) \
    "bootcmd_" #devtypel "=" \
    "run nandboot\0"

#define BOOTENV_DEV_NAME_NAND(devtypeu, devtypel, instance) \
    #devtypel #instance " "

#if CONFIG_IS_ENABLED(CMD_PXE)
    # define BOOT_TARGET_PXE(func) func(PXE, pxe, na)
#else
    # define BOOT_TARGET_PXE(func)
#endif

#if CONFIG_IS_ENABLED(CMD_DHCP)
    # define BOOT_TARGET_DHCP(func) func(DHCP, dhcp, na)
#else
    # define BOOT_TARGET_DHCP(func)
#endif

#define BOOT_TARGET_DEVICES(func)   \
    func(MMC, mmc, 0)               \
    func(LEGACY_MMC, legacy_mmc, 0) \
    func(MMC, mmc, 1)               \
    func(LEGACY_MMC, legacy_mmc, 1) \
    func(NAND, nand, 0)             \
    BOOT_TARGET_PXE(func)           \
    BOOT_TARGET_DHCP(func)

#if (0)
#include <config_distro_bootcmd.h>
#endif

#ifndef CONFIG_SPL_BUILD
    #if (0)
        #include <environment/ti/dfu.h>
        #include <environment/ti/mmc.h>
    #endif

    #define HOST_NAME           "BBB"
    
    #define FDT_BLOB            "/tftpboot/" HOST_NAME "/am335x-boneblack.dtb"
    #define FDT_RAM_ADDR        0x80f00000 /* DDR start + 1x16MiB - 1MiB */
    
    #define KNL_IMAGE           "/tftpboot/" HOST_NAME "/zImage"
    #define KNL_RAM_ADDR        0x81000000 /* DDR start + 1x16MiB */

    #define INITRD_IMAGE        "/tftpboot/" HOST_NAME "/rootfs.cpio.gz.u-boot"
    #define INITRD_RAM_ADDR     0x83000000 /* DDR start + 3x16MiB */

    #define CONFIG_BOOTCOMMAND  "run tftp_boot"
    #define NFS_BOOT            0
    #define INIT_RAM_FS         0
    #define FIT_LOAD            0
    #define FIT_XIP             1
    #if (NFS_BOOT)
        # define ETH_TO_USE     "cpsw"
        # define ROOT_PATH      "/media/constantine/Work/SoundDevice/SoundDevice_endPoint"        \
                                "/TI/am335x/nfsroot"
        # define BOOT_ARGS      "console=ttyS0,115200n8"                                          \
                                "root=/dev/nfs rw rootfstype=nfs nfsroot=${serverip}:"ROOT_PATH" "\
                                "ip=dhcp;"
        # define BOOT_CMD \
                "echo Booting NFS;"                               \
                "setenv ethact " ETH_TO_USE ";"                   \
                "setenv bootfile " KNL_IMAGE ";"                  \
                "dhcp ${kernel_addr_r} ${kernel_image};"          \
                "setenv bootargs " BOOT_ARGS ";"                  \
                "echo Loading FDT from tftp server: ${serverip};" \
                "tftpboot ${fdt_addr_r} ${fdt_blob};"             \
                "bootz ${kernel_addr_r} - ${fdt_addr_r};"
    #elif (INIT_RD)
        # define ETH_TO_USE     "usb_ether"
        # define ROOT_PATH      " "
        # define BOOT_ARGS      "console=ttyS0,115200n8 root=/dev/ram rw earlyprintk"
        # define BOOT_CMD \
                "echo Booting INIT_RD;"                                 \
                "setenv ethact " ETH_TO_USE ";"                         \
                "setenv bootfile " KNL_IMAGE ";"                        \
                "dhcp ${kernel_addr_r} ${kernel_image};"                \
                "setenv bootargs " BOOT_ARGS ";"                        \
                "echo Loading FDT from tftp server: ${serverip};"       \
                "tftpboot ${fdt_addr_r} ${fdt_blob};"                   \
                "echo Loading initramfs from tftp server: ${serverip};" \
                "tftpboot ${initrd_addr_r} ${initrd_image};"            \
                "bootz ${kernel_addr_r} ${initrd_addr_r} ${fdt_addr_r};"
    #elif (INIT_RAM_FS)
        # define ETH_TO_USE     "usb_ether"
        # define ROOT_PATH      " "
        # define BOOT_ARGS      "console=ttyS0,115200n8 root=/dev/ram rw earlyprintk"
        # define BOOT_CMD \
                "echo Booting INIT_RAM_FS;"                             \
                "setenv ethact " ETH_TO_USE ";"                         \
                "setenv bootfile " KNL_IMAGE ";"                        \
                "dhcp ${kernel_addr_r} ${kernel_image};"                \
                "setenv bootargs " BOOT_ARGS ";"                        \
                "echo Loading FDT from tftp server: ${serverip};"       \
                "tftpboot ${fdt_addr_r} ${fdt_blob};"                   \
                "bootz ${kernel_addr_r} - ${fdt_addr_r};"
    #elif (FIT_XIP)
        # define ETH_TO_USE     "usb_ether"
        # define ROOT_PATH      " "
        # define BOOT_ARGS      "console=ttyS0,115200n8 root=/dev/ram rw earlyprintk"
        # define FIT_RAM_ADDR   0x82000000 /* DDR start + 2x16MiB */
        # define CFG_TO_USE     "config@2"
        # define BOOT_CMD \
                "echo Booting FIT;"                                     \
                "setenv ethact     " ETH_TO_USE ";"                     \
                "setenv fit_addr_r "__stringify(FIT_RAM_ADDR)";"        \
                "setenv bootargs " BOOT_ARGS ";"                        \
                "bootm ${fit_addr_r}#"CFG_TO_USE";"
    #elif (FIT_LOAD)
        # define ETH_TO_USE     "usb_ether"
        # define ROOT_PATH      " "
        # define BOOT_ARGS      "console=ttyS0,115200n8 root=/dev/ram rw earlyprintk"
        # define FIT_IMAGE      "/tftpboot/" HOST_NAME "/fit.itb"
        # define FIT_RAM_ADDR   0x82000000 /* DDR start + 2x16MiB */
        # define CFG_TO_USE     "config@2"
        # define BOOT_CMD \
                "echo Booting FIT;"                                     \
                "setenv ethact     " ETH_TO_USE ";"                     \
                "setenv fit_image  " FIT_IMAGE  ";"                     \
                "setenv fit_addr_r "__stringify(FIT_RAM_ADDR)";"        \
                "dhcp ${fit_addr_r} ${fit_image};"                      \
                "setenv bootargs " BOOT_ARGS ";"                        \
                "bootm ${fit_addr_r}#"CFG_TO_USE";"
    #endif
    

    #define CONFIG_EXTRA_ENV_SETTINGS \
        "boot_os=y\0"                                    \
        "rootpath="                 ROOT_PATH        "\0"\
        "hostname="                 HOST_NAME        "\0"\
        "fdt_blob="                 FDT_BLOB         "\0"\
        "fdt_addr_r="   __stringify(FDT_RAM_ADDR)    "\0"\
        "kernel_image="             KNL_IMAGE        "\0"\
        "kernel_addr_r="__stringify(KNL_RAM_ADDR)    "\0"\
        "initrd_image="             INITRD_IMAGE     "\0"\
        "initrd_addr_r="__stringify(INITRD_RAM_ADDR) "\0"\
        "tftp_boot="                BOOT_CMD         "\0"
#else
    #define CONFIG_EXTRA_ENV_SETTINGS \
        "netretry=yes\0"
    
#endif /* CONFIG_SPL_BUILD */

/* NS16550 Configuration */
#define CONFIG_SYS_NS16550_COM1     0x44e09000  /* Base EVM has UART0 */
#define CONFIG_SYS_NS16550_COM2     0x48022000  /* UART1 */
#define CONFIG_SYS_NS16550_COM3     0x48024000  /* UART2 */
#define CONFIG_SYS_NS16550_COM4     0x481a6000  /* UART3 */
#define CONFIG_SYS_NS16550_COM5     0x481a8000  /* UART4 */
#define CONFIG_SYS_NS16550_COM6     0x481aa000  /* UART5 */

#define CONFIG_ENV_EEPROM_IS_ON_I2C
#define CONFIG_SYS_I2C_EEPROM_ADDR      0x50    /* Main EEPROM */
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN  2

/* PMIC support */
#if (1)
    #define CONFIG_POWER_TPS65217
#else
    #define CONFIG_POWER_TPS65910
#endif
/* SPL */
#ifndef CONFIG_NOR_BOOT
/* Bootcount using the RTC block */
#define CONFIG_SYS_BOOTCOUNT_BE
#endif

#ifdef CONFIG_NAND
/* NAND: device related configs */
#define CONFIG_SYS_NAND_5_ADDR_CYCLE
#define CONFIG_SYS_NAND_PAGE_COUNT  (CONFIG_SYS_NAND_BLOCK_SIZE / \
                     CONFIG_SYS_NAND_PAGE_SIZE)
#define CONFIG_SYS_NAND_PAGE_SIZE   2048
#define CONFIG_SYS_NAND_OOBSIZE     64
#define CONFIG_SYS_NAND_BLOCK_SIZE  (128*1024)
/* NAND: driver related configs */
#define CONFIG_SYS_NAND_BAD_BLOCK_POS   NAND_LARGE_BADBLOCK_POS
#define CONFIG_SYS_NAND_ECCPOS      { 2, 3, 4, 5, 6, 7, 8, 9,        \
                                     10, 11, 12, 13, 14, 15, 16, 17, \
                                     18, 19, 20, 21, 22, 23, 24, 25, \
                                     26, 27, 28, 29, 30, 31, 32, 33, \
                                     34, 35, 36, 37, 38, 39, 40, 41, \
                                     42, 43, 44, 45, 46, 47, 48, 49, \
                                     50, 51, 52, 53, 54, 55, 56, 57, }

#define CONFIG_SYS_NAND_ECCSIZE     512
#define CONFIG_SYS_NAND_ECCBYTES    14
#define CONFIG_SYS_NAND_ONFI_DETECTION
#define CONFIG_NAND_OMAP_ECCSCHEME  OMAP_ECC_BCH8_CODE_HW
#define CONFIG_SYS_NAND_U_BOOT_OFFS 0x000c0000
/* NAND: SPL related configs */
#ifdef CONFIG_SPL_OS_BOOT
#define CONFIG_SYS_NAND_SPL_KERNEL_OFFS 0x00200000 /* kernel offset */
#endif
#endif /* !CONFIG_NAND */

/*
 * For NOR boot, we must set this to the start of where NOR is mapped
 * in memory.
 */


/*
 * USB configuration.  We enable MUSB support, both for host and for
 * gadget.  We set USB0 as peripheral and USB1 as host, based on the
 * board schematic and physical port wired to each.  Then for host we
 * add mass storage support and for gadget we add both RNDIS ethernet
 * and DFU.
 */
#define CONFIG_USB_MUSB_DISABLE_BULK_COMBINE_SPLIT
#define CONFIG_AM335X_USB0
#define CONFIG_AM335X_USB0_MODE MUSB_PERIPHERAL
#define CONFIG_AM335X_USB1
#define CONFIG_AM335X_USB1_MODE MUSB_HOST

/*
 * Disable MMC DM for SPL build and can be re-enabled after adding
 * DM support in SPL
 */
#ifdef CONFIG_SPL_BUILD
    #undef CONFIG_DM_MMC
    #if (0)
        #undef CONFIG_TIMER
    #endif
#endif

#if defined(CONFIG_SPL_BUILD) && defined(CONFIG_SPL_USB_ETHER)
    /* Remove other SPL modes. */
    /* disable host part of MUSB in SPL */
    /* disable EFI partitions and partition UUID support */
#endif

/* USB Device Firmware Update support */
#ifndef CONFIG_SPL_BUILD
    #define DFUARGS       \
        DFU_ALT_INFO_EMMC \
        DFU_ALT_INFO_MMC  \
        DFU_ALT_INFO_RAM  \
        DFU_ALT_INFO_NAND
#endif

/*
 * Default to using SPI for environment, etc.
 * 0x000000 - 0x020000 : SPL (128KiB)
 * 0x020000 - 0x0A0000 : U-Boot (512KiB)
 * 0x0A0000 - 0x0BFFFF : First copy of U-Boot Environment (128KiB)
 * 0x0C0000 - 0x0DFFFF : Second copy of U-Boot Environment (128KiB)
 * 0x0E0000 - 0x442000 : Linux Kernel
 * 0x442000 - 0x800000 : Userland
 */
#if defined(CONFIG_SPI_BOOT)
/* SPL related */
#define CONFIG_SYS_SPI_U_BOOT_OFFS  0x20000

#define CONFIG_SYS_REDUNDAND_ENVIRONMENT
#define CONFIG_ENV_SPI_MAX_HZ       CONFIG_SF_DEFAULT_SPEED
#define CONFIG_ENV_SECT_SIZE        (4 << 10) /* 4 KB sectors */
#define CONFIG_ENV_OFFSET           (768 << 10) /* 768 KiB in */
#define CONFIG_ENV_OFFSET_REDUND    (896 << 10) /* 896 KiB in */
#elif defined(CONFIG_EMMC_BOOT)
#define CONFIG_SYS_MMC_ENV_DEV      1
#define CONFIG_SYS_MMC_ENV_PART     0
#define CONFIG_ENV_OFFSET           0x260000
#define CONFIG_ENV_OFFSET_REDUND    (CONFIG_ENV_OFFSET + CONFIG_ENV_SIZE)
#define CONFIG_SYS_REDUNDAND_ENVIRONMENT
#define CONFIG_SYS_MMC_MAX_DEVICE   2
#elif defined(CONFIG_NOR_BOOT)
#define CONFIG_ENV_SECT_SIZE        (128 << 10) /* 128 KiB */
#define CONFIG_ENV_OFFSET           (512 << 10) /* 512 KiB */
#define CONFIG_ENV_OFFSET_REDUND    (768 << 10) /* 768 KiB */
#elif defined(CONFIG_ENV_IS_IN_NAND)
#define CONFIG_ENV_OFFSET           0x001c0000
#define CONFIG_ENV_OFFSET_REDUND    0x001e0000
#define CONFIG_SYS_ENV_SECT_SIZE    CONFIG_SYS_NAND_BLOCK_SIZE
#endif

/* SPI flash. */
#define CONFIG_SF_DEFAULT_SPEED     24000000

/* Network. */
#if (1)
    # define CONFIG_PHY_SMSC
#else
    /* Enable Atheros phy driver */
    # define CONFIG_PHY_ATHEROS
#endif

/*
 * NOR Size = 16 MiB
 * Number of Sectors/Blocks = 128
 * Sector Size = 128 KiB
 * Word length = 16 bits
 * Default layout:
 * 0x000000 - 0x07FFFF : U-Boot (512 KiB)
 * 0x080000 - 0x09FFFF : First copy of U-Boot Environment (128 KiB)
 * 0x0A0000 - 0x0BFFFF : Second copy of U-Boot Environment (128 KiB)
 * 0x0C0000 - 0x4BFFFF : Linux Kernel (4 MiB)
 * 0x4C0000 - 0xFFFFFF : Userland (11 MiB + 256 KiB)
 */
#if defined(CONFIG_NOR)
    #define CONFIG_SYS_MAX_FLASH_SECT   128
    #define CONFIG_SYS_MAX_FLASH_BANKS  1
    #define CONFIG_SYS_FLASH_BASE       (0x08000000)
    #define CONFIG_SYS_FLASH_CFI_WIDTH  FLASH_CFI_16BIT
    #define CONFIG_SYS_FLASH_SIZE       0x01000000
    #define CONFIG_SYS_MONITOR_BASE     CONFIG_SYS_FLASH_BASE
#endif  /* NOR support */

#if (1)
    #ifdef CONFIG_DRIVER_TI_CPSW
    #define CONFIG_CLOCK_SYNTHESIZER
    #define CLK_SYNTHESIZER_I2C_ADDR 0x65
    #endif
#endif

#endif  /* ! __CONFIG_AM335X_EVM_H */