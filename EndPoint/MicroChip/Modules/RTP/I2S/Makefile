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

extra_cflags := \
-I$(SRC_DIR)/hal \
-I$(SRC_DIR)/include \
-DDEBUG

target 	:= i2s.ko
obj-m 	:= i2s.o

i2s-y := i2s_mod.o 
i2s-y += i2s_sys.o
i2s-y += i2s_dev.o
i2s-y += i2s_dma.o

SRC := $(shell pwd)

.PHONY: all clean

all:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(TCH) \
	EXTRA_CFLAGS='$(extra_cflags)' \
	-C $(LINUX_DIR) M=$(PWD) modules
	cp *.ko $(RTP_MOD_PTH)

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
