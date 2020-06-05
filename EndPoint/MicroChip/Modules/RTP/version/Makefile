#
#
#

SAM_LNX_PTH		:= /media/constantine/Work/SoundDevice/SoundDevice_endPoint/SAMA5D27_linux
SAM_TCH_PTH		:= $(SAM_LNX_PTH)/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabi

TCH				:= $(SAM_TCH_PTH)/bin/arm-linux-gnueabi-
CC				:= $(TCH)gcc

LINUX_DIR		:= $(SAM_LNX_PTH)/linux-at91

PWD     		:= $(shell pwd)

RTP_MOD_PTH		:= $(SAM_LNX_PTH)/Modules/RTP
SRC_DIR 		:= $(RTP_MOD_PTH)

#To build module:
obj-m 			:= version.o

version_include := version.h
mod_cflags := \
-I$(SRC_DIR)/include \
-I$(SRC_DIR)/version \
-DHAL_$(hal_type) \
-DDEBUG \
-DVERSION_INCLUDE=\"$(version_include)\" \
-DREVISION=${REVISION} \
-DBUILD_DATE="\"${BUILD_DATE}\"" \
-DCOPYRIGHT="\"${COPYRIGHT}\""

#To build application:
app_cflags=\
$(app_cflags) \
-DVERSION_INCLUDE=\"$(version_include)\" \
$(target_cflags) \
-I. \
-I$(drv_libs_path) \
-I$(LINUX_DIR)/include -c \
-DHAL_$(hal_type) \
-DDEBUG \
-DVERSION_INCLUDE=\"$(version_include)\" \
-DREVISION=${REVISION} \
-DBUILD_DATE="\"${BUILD_DATE}\"" \
-DCOPYRIGHT="\"${COPYRIGHT}\""

all:
	$(MAKE) \
	ARCH=arm CROSS_COMPILE=$(TCH) \
	EXTRA_CFLAGS='$(mod_cflags)' \
	-C $(LINUX_DIR) M=$(PWD) modules
	$(CC) -g $(app_cflags) \
	-o version_app.o version.c
	
clean:
	$(MAKE) -C $(LINUX_DIR) M=$(PWD) clean
