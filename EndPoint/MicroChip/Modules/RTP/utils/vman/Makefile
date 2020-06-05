#
#
#

SAM_LNX_PTH		:= /media/constantine/Work/SoundDevice/SoundDevice_endPoint/SAMA5D27_linux
SAM_TCH_PTH		:= $(SAM_LNX_PTH)/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabi

CROSS_COMPILE	:= $(SAM_TCH_PTH)/bin/arm-linux-gnueabi-
CC=$(CROSS_COMPILE)gcc

CFLAGS=-I.
target=vman


include root.mk

objs=	playrec.o \
		install.o \
		dsp.o mux.o \
		clock.o \
		dtmf.o \
		rtperr.o \
		rtp.o \
		conf-mux.o \
		vman.o \
		sysclk.o \
		muxlist.o \
		channelinfo.o \
		io.o \
		mem.o \
		param.o

$(target): $(objs)
	$(make_app)

.c.o:
	$(make_app_obj)

vman.o: vman.c $(version_include)
	$(make_main_obj)


