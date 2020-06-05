/*
 * $Id: vman.h,v 1.1 2006/08/31 15:11:14 root Exp $
 */

#ifndef __VMAN_H
#define __VMAN_H

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <asm/types.h>
#include <sys/socket.h>      /* struct sockaddr */
#include <netdb.h>           /* gethostbyname() */
#include <netinet/in.h>      /* sockaddr_in */
#include <arpa/inet.h>       /* inet_addr() */
#include "voice-ioctl.h"
#include "hal-ioctl.h"
#include <sys/ioctl.h>


extern int h_ctl, h_conf, h_dtmf, h_rtp, hal_ctl;
extern const char *clk_srcs[];
extern int clk_srcs_count;
extern char *dump_cm_filter;


extern void open_ctl(void);
extern void close_ctl(void);
extern void open_hal(void);
extern void close_hal(void);
extern void open_conf(void);
extern void close_conf(void);
extern void open_dtmf(unsigned int mode);
extern void close_dtmf(void);
extern void open_rtp(void);
extern void close_rtp(void);
extern int get_codec_id(char *codec_id);
extern const char *get_codec_devname(int codec_id);
extern const char *get_codec_name(int codec_id);
int do_ioctl(int h, unsigned ctl, void *arg, char *perror_msg);

struct record {
	char *codec_id;
	unsigned channel;
	unsigned max_size;
	char *filename;
};

struct play {
	char *codec_id;
	unsigned channel;
	char *filename;
};

struct install {
	int dsp;
	char *codec_id;
};

struct upload_dsp_image {
	int dsp;
	char *image_name;
};

struct test_dsp_param {
	int dsp;
	char *mode_str;
	char *input_filename;
	char *output_filename;
	char *crc_filename;
	unsigned short flags;
	unsigned short ipage, opage, iaddr, oaddr, isize, osize;
};

struct mux_conn {
 unsigned int dest;
 unsigned int src;
};

struct dump_dsp {
	int dsp;
	unsigned overlay;
	unsigned address;
	int count;
};

struct dump_mux_dm {
	unsigned address;
	int count;
};

struct mux_read_mem {
	unsigned mem_type;
	unsigned channel;
};

struct rtp_session {
	char* dest;
//	char* display;
	unsigned channel;
	char* payload;
	unsigned pf;
	char* attr;
/*	unsigned gain;
	unsigned vad;
	unsigned dsp;*/
};

struct _mux_tx_mode {
 int dev_no;
 int conf_no;
 char *flag1;
 char *flag2;
 char *flag3;
};

struct mux_rx_add {
	int dev_no;
	int conf_no;
	int chn;
	int gain;
};

struct mux_rx_remove {
	int dev_no;
	int conf_no;
	int chn;
};

struct mux_rx_clear {
	int dev_no;
	int conf_no;
};

struct playtone {
	int timeslot;
	char *string;
};

extern void vman_version(void);
extern void vman_usage(void);

extern int record_file(struct record *r);
extern int play_file(struct play *r);

extern int install_codec(struct install *i);
extern int uninstall_codec(int dsp);

extern int list_dsp(void);
extern int read_dsp_mem(struct param_dsp_mem *m);
extern int dump_dsp_mem(struct dump_dsp *m);
extern int write_dsp_mem(struct param_dsp_mem *m);
extern int reset_dsp(int dsp);
extern int upload_dsp_image(struct upload_dsp_image *ud);
extern int vtest_dsp(struct test_dsp_param *tparm);
extern int dsp_show_bus(int number);

extern int mux_connect(struct mux_conn *c);
extern int mux_disconnect(unsigned int chn);
extern int mux_clear(char *region);
extern int mux_read_mem(struct mux_read_mem *mem);
extern int mux_read_msg(unsigned channel);
extern int mux_write_msg(struct mux_msg_write *mw);
extern int mux_list(char *filters);
extern int dump_mux_data_memory(struct dump_mux_dm *m);

extern int voice_set_clock_source(char *clock_source);

extern int clear_dtmf(unsigned int ts);
extern int write_dtmf(void);
extern int read_dtmf(void);

extern int rtp_session(struct rtp_session *ses);

extern int mux_tx_mode(struct _mux_tx_mode *mode);
extern int mux_rx_add(struct mux_rx_add *add);
extern int mux_rx_remove(struct mux_rx_remove *rm);
extern int mux_rx_clear(struct mux_rx_clear *clr);
extern int mux_enum_devs(void);

extern int show_sysclk_status(void);


extern int do_mux_connect(unsigned dest, unsigned src);
extern int do_mux_disconnect(unsigned chn);

extern int do_channelinfo(void);

extern int codec_echo_ctrl(unsigned on);
extern int codec_vad_ctrl(unsigned on);

extern void enable_panic( void );
extern void disable_panic( void );

int mux_read_channel_state(unsigned channel);

#endif
