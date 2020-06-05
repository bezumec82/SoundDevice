
include ${PWD}/config.mk

subtargets=

ifeq ($(BOARD_VERSION), 5)

subtargets=version utils hal voice spi_tdm

endif

ifeq ($(BOARD_VERSION), 6)

subtargets=version utils hal voice spi_tdm

endif

root:=$(SRC_DIR)

clean:
	rm -rf build-*
	$(foreach dir,$(subtargets),$(MAKE) -C $(dir) $(MAKECMDGOALS) && ) true
	rm -rf $(OBJ_DIR)
        
install:
	$(foreach dir,$(subtargets),$(MAKE) -C $(dir) $(MAKECMDGOALS) && ) true

all:
	$(foreach dir,$(subtargets),$(MAKE) -C $(dir) $(MAKECMDGOALS) && ) true

