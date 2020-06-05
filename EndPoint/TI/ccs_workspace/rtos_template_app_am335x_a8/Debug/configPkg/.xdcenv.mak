#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /home/constantine/ti/bios_6_75_01_05/packages;/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am335x/RTOS_SDK/pdk_am335x_1_0_13/packages
override XDCROOT = /home/constantine/ti/xdctools_3_50_07_20_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /home/constantine/ti/bios_6_75_01_05/packages;/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am335x/RTOS_SDK/pdk_am335x_1_0_13/packages;/home/constantine/ti/xdctools_3_50_07_20_core/packages;..
HOSTOS = Linux
endif
