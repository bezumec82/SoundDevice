#ifndef _SIG_PROTO_H_
#define _SIG_PROTO_H_

#include <netinet/in.h>

#define SIG_PROTO_PROFILE_ID	0x1337 /* special for all 1337 */

#define MSHL(x,m,s) (((x) & (m)) << (s))
#define MSHR(x,m,s) (((x) & (m)) >> (s))
#define SHRM(x,m,s) (((x) >> (s)) & (m))

#define ASHL8(x,s,d) (((x) << (s)) | ((d) & 0x0FF))

enum 
{
	GW_FLAG = 0x01,
    GW_ADJUST_CHAN = 0x02,
	GW_FLAG_PTT = 0x03,
	GW_FLAG_BSS = 0x04,
	GW_FLAG_RV = 0x05,
	GW_FLAG_SH = 0x06,
	GW_FLAG_PW = 0x07,
	GW_FLAG_RS = 0x08,
	GW_SEL_CALL = 0x09,
	GW_SELECT_RC_CHAN = 0x0A,

	WM_FLAG = 0x81,
	WM_TRANS = 0x82,
    WM_RADIO_QUALITY = 0x83,
    WM_CHAN = 0x84,
    WM_SEL_CALL_TRANS = 0x85,
    WM_SELECT_RC_CHAN =  0x86
};

#define SIG_PROTO_PACKETS_COUNT			32   /* Real count = 16 */
#define CALLERID_LEN					256

enum
{
	SIG_PROTO_UNPACK_BAD_PROFILE_ID=-10,
	SIG_PROTO_UNPACK_BAD_LENGHT,
	SIG_PROTO_UNPACK_BAD_TYPE,
	SIG_PROTO_UNPACK_NULL_PTR,
	SIG_PROTO_UNPACK_OK=0
};

enum
{
	SIG_PROTO_PACK_BAD_LENGHT=-9,
	SIG_PROTO_PACK_OK=0
};

union u32float
{
	float flt;
	unsigned long u32;
};

struct sig_proto_wm_chan
{
    unsigned long	id;
    float			freq;
};

struct sig_proto_gw_chan
{
    unsigned long	id;
    float			freq;
};

struct sig_proto_gw_select_rc_chan
{
	unsigned char	interface;
	unsigned char	channel;
};

struct sig_proto_gw_sel_call
{
	unsigned char	callerID_length;
	char 			callerID[CALLERID_LEN];
};


struct sig_proto_wm_trans
{
	struct 	in_addr	session_addr;
	unsigned long	ssrc;
};

struct sig_proto_wm_sel_call_trans
{
	struct 	in_addr	session_addr;
	unsigned long	ssrc;
	unsigned char	callerID_length;
	char 			callerID[CALLERID_LEN];
};

struct sig_proto_wm_select_rc_chan
{
	unsigned char	interface;
	unsigned char	channel;
};

struct sig_proto_wm_radio_quality
{
	float			radio_quality;
};

union sig_proto_payload {
	
	struct sig_proto_gw_chan			gw_adjust_chan;
	struct sig_proto_gw_select_rc_chan	gw_select_rc_chan;
	struct sig_proto_gw_sel_call		gw_sel_call;
	
	struct sig_proto_wm_radio_quality 	wm_radio_quality;
	struct sig_proto_wm_trans			wm_trans;
    struct sig_proto_wm_chan            wm_chan;
    struct sig_proto_wm_sel_call_trans	wm_sel_call_trans;
    struct sig_proto_wm_select_rc_chan	wm_select_rc_chan;
};

struct sig_proto_flags
{
	unsigned long	priority;
	unsigned long 	flag_t;
	unsigned long 	flag_p;
	unsigned long 	flag_r;
	unsigned long 	flag_s;
	unsigned long 	flag_b;
	unsigned long 	flag_rv;
	unsigned long 	flag_sh;
	unsigned long 	flag_rs;
	unsigned long 	flag_pw;
	unsigned long 	flag_c;
	unsigned long	status;
	unsigned long 	reserved1;
};

struct sig_proto
{
	struct 	in_addr	rtp_session_addr;
	unsigned long	rtp_ssrc;
		
	unsigned long 	type;
	
	struct sig_proto_flags	flags;
	union sig_proto_payload payload;
};

struct sig_proto_queue
{
	struct sig_proto 	queue[SIG_PROTO_PACKETS_COUNT];
	long 				need_process[SIG_PROTO_PACKETS_COUNT];
};

int init_sig_proto_queue(struct sig_proto_queue  *queue);

int sig_proto_pack(struct sig_proto *sp, unsigned char 	*payload_ext, int payload_len);
int sig_proto_unpack(struct sig_proto *sp, unsigned char 	*payload_ext, int payload_len);

int sig_proto_init(struct sig_proto *sp);


int sig_proto_sprintf(struct sig_proto *sp, char *str, int size);

int sig_proto_set_base(struct sig_proto *sp, 
							int priority, 
							int flag_t, int flag_p,
							int flag_r, int flag_s,
							int flag_b, int flag_rv,
							int flag_sh,
							int flag_pw, int flag_rs,
							int flag_c,
							int status);

int sig_proto_is_ptt_command(struct sig_proto *sp);

int sig_proto_set_gw_adjust_chan(struct sig_proto *sp, unsigned int	id, float	freq);
int sig_proto_set_gw_select_rc_chan(struct sig_proto *sp, unsigned char interface, unsigned char channel);
int sig_proto_set_gw_selcall(struct sig_proto *sp, const char *callerID, unsigned char callerID_length);

int sig_proto_set_wm_radio_quality(struct sig_proto *sp, float	radio_quality);
int sig_proto_set_wm_transmite(struct sig_proto *sp, struct in_addr session_addr, unsigned int	ssrc);
int sig_proto_set_wm_selcall_transmite(struct sig_proto *sp, struct in_addr session_addr, unsigned int	ssrc, const char *callerID, unsigned char callerID_length);
int sig_proto_set_wm_select_rc_chan(struct sig_proto *sp, unsigned char interface, unsigned char channel);
int sig_proto_set_wm_chan(struct sig_proto *sp, unsigned int	id, float	freq);

#define SIG_PROTO_TABLE_NAME_LENGHT	128
struct s_sig_proto_table_name
{
	int 	id;
	char	*name;
};

enum
{
	SIG_PROTO_SPRINTF_MODE_FULL = 0x01,
	SIG_PROTO_SPRINTF_MODE_PRETTY = 0x02
};

struct s_sig_proto_sprintf
{
	int 	type;
	long (* sprintf)(char *buff,long size, struct sig_proto *sp, int mode);
};

long sprintf_sig_proto(char *buff, long size, struct sig_proto *sp, int mode);
long cmp_sig_proto(struct sig_proto *d1, struct sig_proto *d2);
char * sig_proto_find_name_by_id(struct s_sig_proto_table_name *table, int id);
int sig_proto_find_id_by_name(struct s_sig_proto_table_name *table, char *name);

#endif /* _SIG_PROTO_H_ */
