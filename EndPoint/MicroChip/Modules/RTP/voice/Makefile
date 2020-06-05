#
#
#
SAM_LNX_PTH		:= /media/constantine/Work/SoundDevice/SoundDevice_endPoint/SAMA5D27_linux
SAM_TCH_PTH		:= $(SAM_LNX_PTH)/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabi
TCH				:= $(SAM_TCH_PTH)/bin/arm-linux-gnueabi-
CC				:= $(TCH)gcc
LINUX_DIR		:= $(SAM_LNX_PTH)/linux-at91
RTP_MOD_PTH		:= $(SAM_LNX_PTH)/Modules/RTP
HAL_MOD_PTH		:= $(SAM_LNX_PTH)/Modules/RTP/hal

PWD     := $(shell pwd)
BIN_DIR := $(SRC_DIR)/build-$(BOARD_VERSION).x-$(TARGET_CPU)
SRC_DIR := $(RTP_MOD_PTH)

target := kvoice.ko
obj-m := kvoice.o

kvoice-y := module.o
kvoice-y += rtp.o \
      rtp_ext.o \
      rtp_common.o \
      rtp_recv_skb.o \
      rtp_send.o \
      sequencer.o \
      voice_ioctl.o \
     ../version/version.o

version_include := version.h
extra_cflags := \
-I$(SRC_DIR)/include \
-I$(SRC_DIR)/hal \
-I$(SRC_DIR)/version \
-DDEBUG \
-DVERSION_INCLUDE=\"$(version_include)\" \
-DREVISION=${REVISION} -DBUILD_DATE="\"${BUILD_DATE}\"" \
-DCOPYRIGHT="\"${COPYRIGHT}\""

.PHONY: all clean

all:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(TCH) \
	EXTRA_CFLAGS='$(extra_cflags)' \
	KBUILD_EXTRA_SYMBOLS=$(SRC_DIR)/hal/Module.symvers \
	-C $(LINUX_DIR) M=$(PWD) modules
	cp *.ko $(RTP_MOD_PTH)
	
clean:
	$(MAKE) -C $(LINUX_DIR) M=$(PWD) clean

