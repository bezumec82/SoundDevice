#
#
#

PWD     := $(shell pwd)
BIN_DIR := $(SRC_DIR)/build-$(BOARD_VERSION).x-$(TARGET_CPU)

ifeq ($(BOARD_VERSION), 5)

target := spi_tdm5x.ko
obj-m := spi_tdm5x.o
hal_type=GW50

spi_tdm5x-y := spi_tdm.o \
     bf5xx_sport_ex.o \
     ../version/version.o
     
endif

ifeq ($(BOARD_VERSION), 6)

target := spi_tdm6x.ko
obj-m := spi_tdm6x.o
hal_type=GW60

spi_tdm6x-y := spi_tdm.o \
     bf6xx_sport_ex.o \
     ../version/version.o
     
endif

version_include := version.h
extra_cflags := -I$(SRC_DIR)/include -I$(SRC_DIR)/hal -I$(SRC_DIR)/version -DHAL_$(hal_type) -DDEBUG \
 -DVERSION_INCLUDE=\"$(version_include)\" -DREVISION=${REVISION} -DBUILD_DATE="\"${BUILD_DATE}\"" -DCOPYRIGHT="\"${COPYRIGHT}\""

.PHONY: all clean

all:
	$(MAKE) EXTRA_CFLAGS='$(extra_cflags)' KBUILD_EXTRA_SYMBOLS=$(SRC_DIR)/hal/Module.symvers -C $(LINUX_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(LINUX_DIR) M=$(PWD) clean

install:
	$(MAKE) EXTRA_CFLAGS='$(extra_cflags)' -C $(LINUX_DIR) M=$(PWD) INSTALL_MOD_PATH='$(DEST_DIR)/' modules_install
	find $(PWD) -name $(target) -exec cp {} $(BIN_DIR) \;

