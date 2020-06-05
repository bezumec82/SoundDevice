#
#
#

target=vman2

include root.mk

objs=vman2.o file.o kars_cli.o rtp.o rtp_ext.o sig_proto.o dsp.o mux.o

ifeq ("$(TARGET_CPU)", "BF527")
objs+= spi_tdm.o
target_ldflags=-lncurses -lreadline -L$(target_lib_path)
target_cflags= -I../../include -D_GNU_SOURCE -DBFIN_SPI_TDM
endif

ifeq ("$(TARGET_CPU)", "BF609")
objs+= spi_tdm.o
target_ldflags=-lncurses -lreadline -L$(target_lib_path)
target_cflags= -I../../include -D_GNU_SOURCE -DBFIN_SPI_TDM
endif


$(target): $(objs)
	$(make_app)

.c.o:
	$(make_app_obj)

vman2.o: vman2.c $(version_include)
	$(make_main_obj)

