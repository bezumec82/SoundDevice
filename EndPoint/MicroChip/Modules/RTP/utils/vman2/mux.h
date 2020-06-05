#ifndef _MUX_H_
#define _MUX_H_

struct s_mux_conn
{
	unsigned int dest;
	unsigned int src;
};

struct s_mux_stat
{
	unsigned int dest;
	unsigned int src;
};

int mux_connect(struct s_mux_conn *c);
int mux_disconnect(unsigned int chn);
int mux_clear_region(unsigned int from, unsigned int number);
int mux_clear(void);

#endif /* _MUX_H_ */

