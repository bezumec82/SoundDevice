#ifndef _FILE_H_
#define _FILE_H_

int open_spi_tdm(void);
int open_ctl(void);
int open_hal(void);
int open_conf(void);
int open_dtmf(unsigned int mode);
int open_dsp(void);
int open_rtp(void);
int open_rtp_ext(void);
int do_ioctl(int h, unsigned ctl, void *arg, char *perror_msg);

int do_mux_connect(int hal_ctl, unsigned dest, unsigned src);
int do_mux_disconnect(int hal_ctl, unsigned chn);


#endif /* _FILE_H_ */


