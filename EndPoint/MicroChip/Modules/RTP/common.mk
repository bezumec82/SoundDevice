
OBJ_DIR=.objs-$(BOARD_VERSION).x-$(TARGET_CPU)

ifeq ("$(TARGET_CPU)", "BF527")

cpu_cflags=  -D__uClinux__ -DNOMMU -mcpu=bf527-any

endif

ifeq ("$(TARGET_CPU)", "BF609")

cpu_cflags=  -D__uClinux__ -DNOMMU -mcpu=bf609-any

endif

linux_kernel := $(SRC_DIR)/include
versionPath := $(SRC_DIR)/version
version_mod := $(versionPath)/$(OBJ_DIR)/version.o
version_app := $(versionPath)/$(OBJ_DIR)/version_app.o

common_flags=-pipe -MMD -I.
optimize_flags=-O3 -fomit-frame-pointer
warining_flags=-Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Wall
  
mod_cflags=$(common_flags) $(optimize_flags) $(warining_flags) $(cpu_cflags) $(module_cpu_cflags)
app_cflags=$(common_flags) $(optimize_flags) $(warining_flags) $(cpu_cflags)

drv_libs_path=$(curr)include

application_ldflags=$(target_ldflags) -L$(app_libs_path)/$(OBJDIR) $(version_app)
application_cflags=$(app_cflags) -DVERSION_INCLUDE=\"$(version_include)\" $(target_cflags) -I. -I$(drv_libs_path) -I$(linux_kernel) -c

version_include=$(root)/version/version.h

.PHONY: all clean install $(BIN_DIR) $(OBJ_DIR)

all: 	$(BIN_DIR) $(OBJ_DIR) $(target)
	$(make_subtargets)
	
clean:
	[ -d $(OBJ_DIR) ] || rm -rf $(OBJ_DIR)
	
install: ;

define make_subtargets
	$(foreach dir,$(subtargets),$(MAKE) -C $(dir) $(MAKECMDGOALS) &&) true
endef

define make_app_obj
	$(CC) -g $(application_cflags) $(app_cincludes) $< -o $(OBJ_DIR)/$@
endef

define make_main_obj
	$(CC) -g $(application_cflags) $(app_cincludes) $< -o $(OBJ_DIR)/$@
endef

define make_app
	$(CC) $(application_ldflags) $(patsubst %,$(OBJ_DIR)/%,$^) $(app_libs_opt) -o $(BIN_DIR)/$@
	@echo -n $@ > target-filename
endef

$(BIN_DIR):
	@[ -d $@ ] || mkdir -p $@
	
$(OBJ_DIR):
	[ -d $@ ] || mkdir -p $@
	@echo "include ../root.mk" >$@/root.mk

