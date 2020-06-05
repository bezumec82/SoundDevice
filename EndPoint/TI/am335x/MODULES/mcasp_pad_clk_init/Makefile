TI_PTH		:= /media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI
AM335_PTH	:= ${TI_PTH}/am335x
LINUX_DIR	:= ${AM335_PTH}/processor-sdk-linux-rt
TCH_PTH		:= ${TI_PTH}/gcc-linaro-7.2.1-2017.11-x86_64_arm-linux-gnueabihf
TCH		:= $(TCH_PTH)/bin/arm-linux-gnueabihf-
CC		:= $(TCH)gcc

PWD     := $(shell pwd)
BIN_DIR := $(SRC_DIR)/build-$(BOARD_VERSION).x-$(TARGET_CPU)
SRC_DIR := $(RTP_MOD_PTH)

extra_cflags := \
-DDEBUG

target	:= mcasp_pad_clk.ko
obj-m	:= mcasp_pad_clk.o
mcasp_pad_clk-y	+= mcasp_pad_clk_init.o


SRC := $(shell pwd)

.PHONY: all clean

all:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(TCH) \
	EXTRA_CFLAGS='$(extra_cflags)' \
	-C $(LINUX_DIR) M=$(PWD) modules

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
