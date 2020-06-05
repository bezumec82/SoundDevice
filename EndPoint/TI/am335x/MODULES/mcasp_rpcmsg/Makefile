TI_PTH		:= /media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI
AM335x_PTH	:= ${TI_PTH}/am335x
LINUX_DIR	:= ${AM335x_PTH}/processor-sdk-linux-rt
TCH_PTH		:= ${TI_PTH}/gcc-linaro-7.2.1-2017.11-x86_64_arm-linux-gnueabihf
TCH			:= $(TCH_PTH)/bin/arm-linux-gnueabihf-
CC			:= $(TCH)gcc
PWD		:= $(shell pwd)
BIN_DIR 	:= $(SRC_DIR)/build-$(BOARD_VERSION).x-$(TARGET_CPU)
SRC_DIR 	:= $(RTP_MOD_PTH)

extra_cflags := \
-DDEBUG

target	:= mcasp_rpmsg_mod.ko
obj-m	:= mcasp_rpmsg.o
mcasp_rpmsg-y	+= pru_mcasp_dev.o
mcasp_rpmsg-y	+= pru_mcasp_sys.o
mcasp_rpmsg-y	+= mcasp_rpmsg_mod.o

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
