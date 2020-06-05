#
#
#

SAM_LNX_PTH		:= /media/constantine/RAID/SoundDevice/SoundDevice_endPoint/SAMA5D27_linux
SAM_TCH_PTH		:= $(SAM_LNX_PTH)/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabi
TCH				:= $(SAM_TCH_PTH)/bin/arm-linux-gnueabi-
CC				:= $(TCH)gcc
LINUX_DIR		:= $(SAM_LNX_PTH)/linux-at91
RTP_MOD_PTH		:= $(SAM_LNX_PTH)/Modules/RTP
HAL_MOD_PTH		:= $(SAM_LNX_PTH)/Modules/RTP/hal

PWD     := $(shell pwd)
BIN_DIR := $(SRC_DIR)/build-$(BOARD_VERSION).x-$(TARGET_CPU)
SRC_DIR := $(RTP_MOD_PTH)

target 	:= hal.ko
obj-m 	:= hal.o
hal_type=GW50

hal-y := module.o
hal-y += init.o \
	 msec-sched.o \
	 clk-common.o \
	 clk-common-notify.o \
	 timeslots.o \
	 dsp-check.o \
	 dsp-common.o \
	 channel.o \
	 codec.o \
	 vcodec.o \
	 vdspmixer.o \
	 pcm.o \
	 g711.o \
	 cb.o \
	 vdsp.o \
	 vmux.o \
	 hal-ctl.o \
	 hal-ioctl-tdm.o \
	 hal-ioctl-syslck.o \
	 hal-ioctl-dsp.o \
	 hal-ioctl-mux.o \
	 hal-ioctl-mem.o \
	 hal-ioctl-flash.o \
	 hal-ioctl-misc.o \
	 ../version/version.o

version_include := version.h
extra_cflags := \
-I$(SRC_DIR)/include \
-I$(SRC_DIR)/version \
-DHAL_$(hal_type) \
-DDEBUG \
-DVERSION_INCLUDE=\"$(version_include)\" \
-DREVISION=${REVISION} \
-DBUILD_DATE="\"${BUILD_DATE}\"" \
-DCOPYRIGHT="\"${COPYRIGHT}\""


.PHONY: all clean

all:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(TCH) \
	EXTRA_CFLAGS='$(extra_cflags)' \
	-C $(LINUX_DIR) M=$(PWD) modules
	cp *.ko $(RTP_MOD_PTH)

clean:
	$(MAKE) -C $(LINUX_DIR) M=$(PWD) clean
	rm -rf $(RTP_MOD_PTH)/hal.ko

