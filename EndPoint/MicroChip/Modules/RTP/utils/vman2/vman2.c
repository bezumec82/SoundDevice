#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "vman2.h"
#include "kars_cli.h"
#include "voice-ioctl.h"


static int							option_verbose = 0;
static int							need_quit = 0;
static int							fd_console_std_in = STDIN_FILENO;
static char							console_ps1[]="\rVMAN@CLI>";
static struct s_voice_man			vman;
static struct sig_proto				sig_proto_tx;

struct s_codec_types codecs[] =
{
    {.id = CODEC_ID_G711  , .name = "g711"  , .dev = NULL },
/*    {.id = CODEC_ID_G723  , .name = "g723"  , .dev = NULL },
    {.id = CODEC_ID_G729  , .name = "g729"  , .dev = NULL },
    {.id = CODEC_ID_MIXER , .name = "mixer" , .dev = NULL },
    {.id = CODEC_ID_CMIXER, .name = "cmixer", .dev = NULL },*/
    {.id = CODEC_ID_LIN16 , .name = "lin16",  .dev = NULL },
};


static int vman_exit(int fd, int argc, char *argv[]);
static int vman_info(int fd, int argc, char *argv[]);
static int vman_rtp_filter_set(int fd, int argc, char *argv[]);
static int vman_rtp_filter_reset(int fd, int argc, char *argv[]);
static int vman_rtp_show(int fd, int argc, char *argv[]);
static int vman_rtp_add(int fd, int argc, char *argv[]);
static int vman_rtp_del(int fd, int argc, char *argv[]);
static int vman_rtp_ext_show(int fd, int argc, char *argv[]);
static int vman_rtp_ext_attach(int fd, int argc, char *argv[]);
static int vman_rtp_ext_detach(int fd, int argc, char *argv[]);
static int vman_rtp_ext_send_uni(int packet_type, int fd, int argc, char *argv[]);
static int vman_rtp_ext_send_gw_flag_ptt(int fd, int argc, char *argv[]);
static int vman_rtp_ext_send_gw_flag_bss(int fd, int argc, char *argv[]);
static int vman_rtp_ext_send_gw_flag_recv(int fd, int argc, char *argv[]);
static int vman_rtp_ext_send_gw_flag_selfhear(int fd, int argc, char *argv[]);
static int vman_rtp_ext_send_gw_flag_power(int fd, int argc, char *argv[]);
static int vman_rtp_ext_send_gw_flag_reserv(int fd, int argc, char *argv[]);
static int vman_rtp_ext_send_gw_flag(int fd, int argc, char *argv[]);
static int vman_rtp_ext_send_gw_adjust_chan(int fd, int argc, char *argv[]);
static int vman_rtp_ext_send_gw_sel_call(int fd, int argc, char *argv[]);
static int vman_rtp_ext_send_gw_select_rc_chan(int fd, int argc, char *argv[]);

static int vman_rtp_ext_send_wm_flag(int fd, int argc, char *argv[]);
static int vman_rtp_ext_send_wm_trans(int fd, int argc, char *argv[]);
static int vman_rtp_ext_send_wm_radio_quality(int fd, int argc, char *argv[]);
static int vman_rtp_ext_send_wm_chan(int fd, int argc, char *argv[]);
static int vman_rtp_ext_send_wm_sel_call_trans(int fd, int argc, char *argv[]);
static int vman_rtp_ext_send_wm_select_rc_chan(int fd, int argc, char *argv[]);

static int vman_dsp_list(int fd, int argc, char *argv[]);
static int vman_dsp_vad_on(int fd, int argc, char *argv[]);
static int vman_dsp_vad_off(int fd, int argc, char *argv[]);
static int vman_dsp_vad_show(int fd, int argc, char *argv[]);
static int vman_dsp_install_codec(int fd, int argc, char *argv[]);
static int vman_dsp_uninstall_codec(int fd, int argc, char *argv[]);
static int vman_dsp_channel_info(int fd, int argc, char *argv[]);

static int vman_mux_connect(int fd, int argc, char *argv[]);
static int vman_mux_disconnect(int fd, int argc, char *argv[]);
static int vman_mux_clear(int fd, int argc, char *argv[]);

#ifdef BFIN_SPI_TDM
static int vman_spi_add(int fd, int argc, char *argv[]);
static int vman_spi_remove(int fd, int argc, char *argv[]);
static int vman_spi_stat(int fd, int argc, char *argv[]);
#endif


static int rtp_del(struct s_voice_man	*pvman, int session_id);
static int rtp_ext_detach(struct s_voice_man	*pvman, int rtp_ext_session_id);

/*----------------------------------------------------------------------------*/
static char exit_usage[] =
	"Usage: exit\r\n"
	"";
	
static char info_usage[] =
	"Usage: info\r\n"
	"";
	
#ifdef BFIN_SPI_TDM
static char vman_spi_add_usage[] =
	"Usage: spi add CHANNEL [TDM]\r\n"
	"";
	
static char vman_spi_remove_usage[] =
	"Usage: spi remove CHANNEL\r\n"
	"";
	
static char vman_spi_stat_usage[] =
	"Usage: spi stat CHANNEL\r\n"
	"";
#endif	

static char vman_mux_connect_usage[] =
	"Usage: mux connect DEST SRC\r\n"
	"";
	
static char vman_mux_disconnect_usage[] =
	"Usage: mux disconnect CHANNEL\r\n"
	"";
	
static char vman_mux_clear_usage[] =
	"Usage: mux clear\r\n"
	"";	

static char vman_dsp_list_usage[] =
	"Usage: dsp list\r\n"
	"";
	
static char vman_dsp_vad_on_usage[] =
	"Usage: dsp vad on TIMESLOT\r\n"
	"";
	
static char vman_dsp_vad_show_usage[] =
	"Usage: dsp vad show [TIMESLOT]\r\n"
	"";
	
static char vman_dsp_vad_off_usage[] =
	"Usage: dsp vad off [TIMESLOT]\r\n"
	"";
	
static char vman_dsp_install_codec_usage[] =
	"Usage: dsp install DSP CODEC\r\n"
	"";	
	
static char vman_dsp_uninstall_codec_usage[] =
	"Usage: dsp install DSP\r\n"
	"";	
	
static char vman_dsp_channel_info_usage[] =
	"Usage: dsp channel info CHANNEL\r\n"
	"";
	
static char vman_rtp_filter_reset_usage[] =
	"Usage: rtp filter reset [RTP_SESSION_ID]\r\n"
	"";
	
static char vman_rtp_filter_set_usage[] =
	"Usage: rtp filter set [RTP_SESSION_ID [IP/SSRC]]\r\n"
	"";
	
static char vman_rtp_show_usage[] =
	"Usage: rtp show [RTP_SESSION_ID]\r\n"
	"";
	
static char vman_rtp_del_usage[] =
	"Usage: rtp del [RTP_SESSION_ID]\r\n"
	"";
	
static char vman_rtp_add_usage[] =
	"Usage: rtp add PAYLOAD IP_DEST/PORT_DEST/SRC_DEST [CHANNEL [ATTRIBUTES [PACKET_FACTOR]]]\r\n"
	"";
	
static char vman_rtp_ext_show_usage[] =
	"Usage: rtp_ext show [RTP_EXT_SESSION_ID]\r\n"
	"";
	
static char vman_rtp_ext_detach_usage[] =
	"Usage: rtp_ext detach [RTP_EXT_SESSION_ID]\r\n"
	"";
	
static char vman_rtp_ext_attach_usage[] =
	"Usage: rtp_ext attach RTP_SESSION_ID\r\n"
	"";
	
static char vman_rtp_ext_send_gw_flag_ptt_usage[] =
	"Usage: rtp_ext send gw_flag_ptt [RTP_EXT_SESSION_ID [FLAG | PRIORITY/T/P/R/S/B/RV/SH/PW/RS/C/STATUS]]\r\n"
	" NO ANY ARGUMENTS   - send PTT_OFF to all sessions\r\n"
	" NO SECOND ARGUMENT - send PTT_OFF to RTP_EXT_SESSION_ID session\r\n"
	" FLAG SECOND ARGUMENT - send FLAG value as PTT to RTP_EXT_SESSION_ID session\r\n"
	" SLASH_LIST SECOND ARGUMENT - send specify values to RTP_EXT_SESSION_ID session\r\n"
	"";
	
static char vman_rtp_ext_send_gw_flag_bss_usage[] =
	"Usage: rtp_ext send gw_flag_bss [RTP_EXT_SESSION_ID [FLAG | PRIORITY/T/P/R/S/B/RV/SH/PW/RS/C/STATUS]]\r\n"
	" NO ANY ARGUMENTS   - send BSS_OFF to all sessions\r\n"
	" NO SECOND ARGUMENT - send BSS_OFF to RTP_EXT_SESSION_ID session\r\n"
	" FLAG SECOND ARGUMENT - send FLAG value as BSS to RTP_EXT_SESSION_ID session\r\n"
	" SLASH_LIST SECOND ARGUMENT - send specify values to RTP_EXT_SESSION_ID session\r\n"
	"";
	
static char vman_rtp_ext_send_gw_flag_recv_usage[] =
	"Usage: rtp_ext send gw_flag_recv [RTP_EXT_SESSION_ID [FLAG | PRIORITY/T/P/R/S/B/RV/SH/PW/RS/C/STATUS]]\r\n"
	" NO ANY ARGUMENTS   - send RECEIVER_OFF to all sessions\r\n"
	" NO SECOND ARGUMENT - send RECEIVER_OFF to RTP_EXT_SESSION_ID session\r\n"
	" FLAG SECOND ARGUMENT - send FLAG value as RECEIVER to RTP_EXT_SESSION_ID session\r\n"
	" SLASH_LIST SECOND ARGUMENT - send specify values to RTP_EXT_SESSION_ID session\r\n"
	"";
	
static char vman_rtp_ext_send_gw_flag_selfhear_usage[] =
	"Usage: rtp_ext send gw_flag_selfhear [RTP_EXT_SESSION_ID [FLAG | PRIORITY/T/P/R/S/B/RV/SH/PW/RS/C/STATUS]]\r\n"
	" NO ANY ARGUMENTS   - send SELFHEAR_OFF to all sessions\r\n"
	" NO SECOND ARGUMENT - send SELFHEAR_OFF to RTP_EXT_SESSION_ID session\r\n"
	" FLAG SECOND ARGUMENT - send FLAG value as SELFHEAR to RTP_EXT_SESSION_ID session\r\n"
	" SLASH_LIST SECOND ARGUMENT - send specify values to RTP_EXT_SESSION_ID session\r\n"
	"";
	
static char vman_rtp_ext_send_gw_flag_power_usage[] =
	"Usage: rtp_ext send gw_flag_power [RTP_EXT_SESSION_ID [FLAG | PRIORITY/T/P/R/S/B/RV/SH/PW/RS/C/STATUS]]\r\n"
	" NO ANY ARGUMENTS   - send POWER_OFF to all sessions\r\n"
	" NO SECOND ARGUMENT - send POWER_OFF to RTP_EXT_SESSION_ID session\r\n"
	" FLAG SECOND ARGUMENT - send FLAG value as POWER to RTP_EXT_SESSION_ID session\r\n"
	" SLASH_LIST SECOND ARGUMENT - send specify values to RTP_EXT_SESSION_ID session\r\n"
	"";
	
static char vman_rtp_ext_send_gw_flag_reserv_usage[] =
	"Usage: rtp_ext send gw_flag_reserv [RTP_EXT_SESSION_ID [FLAG | PRIORITY/T/P/R/S/B/RV/SH/PW/RS/C/STATUS]]\r\n"
	" NO ANY ARGUMENTS   - send RESERV_OFF to all sessions\r\n"
	" NO SECOND ARGUMENT - send RESERV_OFF to RTP_EXT_SESSION_ID session\r\n"
	" FLAG SECOND ARGUMENT - send FLAG value as RESERV to RTP_EXT_SESSION_ID session\r\n"
	" SLASH_LIST SECOND ARGUMENT - send specify values to RTP_EXT_SESSION_ID session\r\n"
	"";
	
static char vman_rtp_ext_send_gw_flag_usage[] =
	"Usage: rtp_ext send gw_flag [RTP_EXT_SESSION_ID [PRIORITY/T/P/R/S/B/RV/SH/PW/RS/C/STATUS]]\r\n"
	" NO ANY ARGUMENTS   - send all flags off to all sessions\r\n"
	" NO SECOND ARGUMENT - send all flags off to RTP_EXT_SESSION_ID session\r\n"
	" SLASH_LIST SECOND ARGUMENT - send specify values to RTP_EXT_SESSION_ID session\r\n"
	"";
	
static char vman_rtp_ext_send_gw_adjust_chan_usage[] =
	"Usage: rtp_ext send gw_adjust_chan [RTP_EXT_SESSION_ID [ID/FREQ]]\r\n"
	" NO ANY ARGUMENTS   - send ID=0/FREQ=0.0 to all sessions\r\n"
	" NO SECOND ARGUMENT - send ID=0/FREQ=0.0 to RTP_EXT_SESSION_ID session\r\n"
	" SLASH_LIST SECOND ARGUMENT - send specify values to RTP_EXT_SESSION_ID session\r\n"
	"";
	
static char vman_rtp_ext_send_gw_sel_call_usage[] =
	"Usage: rtp_ext send gw_sel_call [RTP_EXT_SESSION_ID [CALLERID]]\r\n"
	" NO ANY ARGUMENTS   - send empty CALLERID to all sessions\r\n"
	" NO SECOND ARGUMENT - send empty CALLERID to RTP_EXT_SESSION_ID session\r\n"
	" ALL ARGUMENTS - send specify CALLERID to RTP_EXT_SESSION_ID session\r\n"
	"";
	
static char vman_rtp_ext_send_gw_select_rc_chan_usage[] =
	"Usage: rtp_ext send gw_select_rc_chan [RTP_EXT_SESSION_ID [INTERFACE/CHANNEL]]\r\n"
	" NO ANY ARGUMENTS   - send INTERFACE=0/CHANNEL=0 to all sessions\r\n"
	" NO SECOND ARGUMENT - send INTERFACE=0/CHANNEL=0 to RTP_EXT_SESSION_ID session\r\n"
	" ALL ARGUMENTS - send specify INTERFACE/CHANNEL to RTP_EXT_SESSION_ID session\r\n"
	"";
	
static char vman_rtp_ext_send_wm_flag_usage[] =
	"Usage: rtp_ext send wm_flag [RTP_EXT_SESSION_ID [PRIORITY/T/P/R/S/B/RV/SH/PW/RS/C/STATUS]]\r\n"
	" NO ANY ARGUMENTS   - send all flags off to all sessions\r\n"
	" NO SECOND ARGUMENT - send all flags off to RTP_EXT_SESSION_ID session\r\n"
	" SLASH_LIST SECOND ARGUMENT - send specify values to RTP_EXT_SESSION_ID session\r\n"
	"";
	
static char vman_rtp_ext_send_wm_trans_usage[] =
	"Usage: rtp_ext send wm_trans [RTP_EXT_SESSION_ID [IP/SSRC]]\r\n"
	" NO ANY ARGUMENTS   - send IP=0.0.0.0/SSRC=0 to all sessions\r\n"
	" NO SECOND ARGUMENT - send IP=0.0.0.0/SSRC=0 to RTP_EXT_SESSION_ID session\r\n"
	" SLASH_LIST SECOND ARGUMENT - send specify values to RTP_EXT_SESSION_ID session\r\n"
	"";
	
static char vman_rtp_ext_send_wm_radio_quality_usage[] =
	"Usage: rtp_ext send wm_radio_quality [RTP_EXT_SESSION_ID [RQ]]\r\n"
	" NO ANY ARGUMENTS   - send RQ=0.0 to all sessions\r\n"
	" NO SECOND ARGUMENT - send RQ=0.0 to RTP_EXT_SESSION_ID session\r\n"
	" SLASH_LIST SECOND ARGUMENT - send specify values to RTP_EXT_SESSION_ID session\r\n"
	"";
	
static char vman_rtp_ext_send_wm_chan_usage[] =
	"Usage: rtp_ext send wm_chan [RTP_EXT_SESSION_ID [ID/FREQ]]\r\n"
	" NO ANY ARGUMENTS   - send ID=0/FREQ=0.0 to all sessions\r\n"
	" NO SECOND ARGUMENT - send ID=0/FREQ=0.0 to RTP_EXT_SESSION_ID session\r\n"
	" SLASH_LIST SECOND ARGUMENT - send specify values to RTP_EXT_SESSION_ID session\r\n"
	"";
	
static char vman_rtp_ext_send_wm_sel_call_trans_usage[] =
	"Usage: rtp_ext send wm_trans [RTP_EXT_SESSION_ID [IP/SSRC/CALLERID]]\r\n"
	" NO ANY ARGUMENTS   - send IP=0.0.0.0/SSRC=0/CALLERID='' to all sessions\r\n"
	" NO SECOND ARGUMENT - send IP=0.0.0.0/SSRC=0/CALLERID='' to RTP_EXT_SESSION_ID session\r\n"
	" SLASH_LIST SECOND ARGUMENT - send specify values to RTP_EXT_SESSION_ID session\r\n"
	"";
	
static char vman_rtp_ext_send_wm_select_rc_chan_usage[] =
	"Usage: rtp_ext send wm_select_rc_chan [RTP_EXT_SESSION_ID [INTERFACE/CHANNEL]]\r\n"
	" NO ANY ARGUMENTS   - send INTERFACE=0/CHANNEL=0 to all sessions\r\n"
	" NO SECOND ARGUMENT - send INTERFACE=0/CHANNEL=0 to RTP_EXT_SESSION_ID session\r\n"
	" ALL ARGUMENTS      - send specify INTERFACE/CHANNEL to RTP_EXT_SESSION_ID session\r\n"
	"";	

/*----------------------------------------------------------------------------*/
static struct ks_cli_entry cli_vman[] = {

	{ { "exit", NULL },
	vman_exit, "Exit from application.",
	exit_usage, NULL},
	
	{ { "info", NULL },
	vman_info, "Application info.",
	info_usage, NULL},

#ifdef BFIN_SPI_TDM

	{ { "spi", "add", NULL },
	vman_spi_add, "Add SPI TDM channel.",
	vman_spi_add_usage, NULL},
	
	{ { "spi", "remove", NULL },
	vman_spi_remove, "Remove SPI TDM channel.",
	vman_spi_remove_usage, NULL},
	
	{ { "spi", "stat", NULL },
	vman_spi_stat, "Get state and statistic of SPI TDM channel.",
	vman_spi_stat_usage, NULL},	
	
#endif

	{ { "mux", "connect", NULL },
	vman_mux_connect, "Make connection two channels.",
	vman_mux_connect_usage, NULL},
	
	{ { "mux", "disconnect", NULL },
	vman_mux_disconnect, "Brake connection channel.",
	vman_mux_disconnect_usage, NULL},
	
	{ { "mux", "clear", NULL },
	vman_mux_clear, "Brake connection all channel.",
	vman_mux_clear_usage, NULL},
	
	{ { "dsp", "list", NULL },
	vman_dsp_list, "Show list of DSPs.",
	vman_dsp_list_usage, NULL},
	
	{ { "dsp", "vad", "on", NULL },
	vman_dsp_vad_on, "Turn on VAD on timeslot.",
	vman_dsp_vad_on_usage, NULL},
	
	{ { "dsp", "vad", "off", NULL },
	vman_dsp_vad_off, "Turn off VAD in timeslot or all.",
	vman_dsp_vad_off_usage, NULL},	

	{ { "dsp", "vad", "show", NULL },
	vman_dsp_vad_show, "Show VAD info in timeslot or all.",
	vman_dsp_vad_show_usage, NULL},
	
	{ { "dsp", "install", NULL },
	vman_dsp_install_codec, "Install codec to DSP.",
	vman_dsp_install_codec_usage, NULL},
	
	{ { "dsp", "uninstall", NULL },
	vman_dsp_uninstall_codec, "Uninstall codec from DSP.",
	vman_dsp_uninstall_codec_usage, NULL},
	
	{ { "dsp", "channel", "info", NULL },
	vman_dsp_channel_info, "Get channel info.",
	vman_dsp_channel_info_usage, NULL},
	
	{ { "rtp", "filter", "set", NULL },
	vman_rtp_filter_set, "Set filter for rtp session",
	vman_rtp_filter_set_usage, NULL},
	
	{ { "rtp", "filter", "reset", NULL },
	vman_rtp_filter_reset, "Reset filter for rtp session",
	vman_rtp_filter_reset_usage, NULL},
	
	{ { "rtp", "show", NULL },
	vman_rtp_show, "Show rtp sessions info",
	vman_rtp_show_usage, NULL},
	
	{ { "rtp", "add", NULL },
	vman_rtp_add, "Add new rtp session",
	vman_rtp_add_usage, NULL},
	
	{ { "rtp", "del", NULL },
	vman_rtp_del, "Delete rtp session",
	vman_rtp_del_usage, NULL},
	
	{ { "rtp_ext", "show", NULL },
	vman_rtp_ext_show, "Show rtp extension sessions info",
	vman_rtp_ext_show_usage, NULL},
	
	{ { "rtp_ext", "attach", NULL },
	vman_rtp_ext_attach, "Attach rtp session to rtp extension session",
	vman_rtp_ext_attach_usage, NULL},
	
	{ { "rtp_ext", "detach", NULL },
	vman_rtp_ext_detach, "Detach rtp session from rtp extension session",
	vman_rtp_ext_detach_usage, NULL},
	
	{ { "rtp_ext", "send", "gw_flag", NULL },
	vman_rtp_ext_send_gw_flag, "Send GW_FLAG packet over rtp extension session",
	vman_rtp_ext_send_gw_flag_usage, NULL},
	
	{ { "rtp_ext", "send", "gw_flag_ptt", NULL },
	vman_rtp_ext_send_gw_flag_ptt, "Send GW_FLAG_PTT packet over rtp extension session",
	vman_rtp_ext_send_gw_flag_ptt_usage, NULL},
	
	{ { "rtp_ext", "send", "gw_flag_bss", NULL },
	vman_rtp_ext_send_gw_flag_bss, "Send GW_FLAG_BSS packet over rtp extension session",
	vman_rtp_ext_send_gw_flag_bss_usage, NULL},
	
	{ { "rtp_ext", "send", "gw_flag_recv", NULL },
	vman_rtp_ext_send_gw_flag_recv, "Send GW_FLAG_RV packet over rtp extension session",
	vman_rtp_ext_send_gw_flag_recv_usage, NULL},
	
	{ { "rtp_ext", "send", "gw_flag_selfhear", NULL },
	vman_rtp_ext_send_gw_flag_selfhear, "Send GW_FLAG_SH packet over rtp extension session",
	vman_rtp_ext_send_gw_flag_selfhear_usage, NULL},
	
	{ { "rtp_ext", "send", "gw_flag_power", NULL },
	vman_rtp_ext_send_gw_flag_power, "Send GW_FLAG_PW packet over rtp extension session",
	vman_rtp_ext_send_gw_flag_power_usage, NULL},
	
	{ { "rtp_ext", "send", "gw_flag_reserv", NULL },
	vman_rtp_ext_send_gw_flag_reserv, "Send GW_FLAG_RS packet over rtp extension session",
	vman_rtp_ext_send_gw_flag_reserv_usage, NULL},	

	{ { "rtp_ext", "send", "gw_adjust_chan", NULL },
	vman_rtp_ext_send_gw_adjust_chan, "Send GW_ADJUST_CHAN packet over rtp extension session",
	vman_rtp_ext_send_gw_adjust_chan_usage, NULL},
	
	{ { "rtp_ext", "send", "gw_sel_call", NULL },
	vman_rtp_ext_send_gw_sel_call, "Send GW_SEL_CALL packet over rtp extension session",
	vman_rtp_ext_send_gw_sel_call_usage, NULL},
	
	{ { "rtp_ext", "send", "gw_select_rc_chan", NULL },
	vman_rtp_ext_send_gw_select_rc_chan, "Send GW_SELECT_RC_CHAN packet over rtp extension session",
	vman_rtp_ext_send_gw_select_rc_chan_usage, NULL},

	{ { "rtp_ext", "send", "wm_flag", NULL },
	vman_rtp_ext_send_wm_flag, "Send WM_FLAG packet over rtp extension session",
	vman_rtp_ext_send_wm_flag_usage, NULL},
	
	{ { "rtp_ext", "send", "wm_trans", NULL },
	vman_rtp_ext_send_wm_trans, "Send WM_TRANS packet over rtp extension session",
	vman_rtp_ext_send_wm_trans_usage, NULL},
	
	{ { "rtp_ext", "send", "wm_radio_quality", NULL },
	vman_rtp_ext_send_wm_radio_quality, "Send WM_RADIO_QUALITY packet over rtp extension session",
	vman_rtp_ext_send_wm_radio_quality_usage, NULL},
	
	{ { "rtp_ext", "send", "wm_chan", NULL },
	vman_rtp_ext_send_wm_chan, "Send WM_CHAN packet over rtp extension session",
	vman_rtp_ext_send_wm_chan_usage, NULL},

	{ { "rtp_ext", "send", "wm_sel_call_trans", NULL },
	vman_rtp_ext_send_wm_sel_call_trans, "Send WM_SEL_CALL_TRANS packet over rtp extension session",
	vman_rtp_ext_send_wm_sel_call_trans_usage, NULL},
	
	{ { "rtp_ext", "send", "wm_select_rc_chan", NULL },
	vman_rtp_ext_send_wm_select_rc_chan, "Send WM_SELECT_RC_CHAN packet over rtp extension session",
	vman_rtp_ext_send_wm_select_rc_chan_usage, NULL}

};
/* -------------------------------------------------------------------------- */
static void usage(const char * program)
{
	fprintf(stderr, "\n");
	fprintf(stderr, "usage : %s [-vh]\n",program );
	fprintf(stderr, "\t-v verbose.\n");
	fprintf(stderr, "\t-h this message.\n");
}
/* -------------------------------------------------------------------------- */
static int get_options( int argc, char * const argv[] )
{
	int c = 0;
	const char * progname = argv[0];

	while( (c = getopt( argc, argv, "hv")) != EOF ) 
	{
		switch(c) 
		{
			case 'v':
			option_verbose = 1;
			break;
			case 'h':
			usage(progname);
			return -2;
			default:
			usage(progname);
			return -1;
		}
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
int get_codec_id(char *s)
{
	int i, len;
	len = sizeof(codecs)/sizeof(codecs[0]);
	for (i = 0; i < len; i++)
	{
		if (strcmp(s, codecs[i].name) == 0) return codecs[i].id;
	}
	return -1;
}
/* -------------------------------------------------------------------------- */
const char *get_codec_name(int id)
{
	int len;
	len = sizeof(codecs)/sizeof(codecs[0]);
	if (id < 1 || id >= len) {
		return NULL;
	}
	return codecs[id].name;
}
/* -------------------------------------------------------------------------- */
static void	vman_default_config(struct s_voice_man			*pvman)
{
	if (pvman)
	{
		memset(pvman, 0, sizeof(*pvman));
	}
}
/* -------------------------------------------------------------------------- */
static int	vman_init(struct s_voice_man			*pvman)
{
	if (pvman)
	{
		INIT_LIST_HEAD(&pvman->rtp_list);
		INIT_LIST_HEAD(&pvman->rtp_ext_list);
		INIT_LIST_HEAD(&pvman->dsp_list);
		INIT_LIST_HEAD(&pvman->channel_list);
		
		dsp_open(&pvman->vad);
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
static int	vman_close(struct s_voice_man			*pvman)
{
	if (pvman)
	{
		rtp_ext_detach(pvman, -1);
		rtp_del(pvman, -1);
		dsp_close(&pvman->vad);
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
static int vman_exit(int fd, int argc, char *argv[])
{
	need_quit = 1;
	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_info(int fd, int argc, char *argv[])
{
	int i;
	
    fprintf(stdout, "\nAvailable codecs:\n\n");
    for (i = 0; i < sizeof(codecs)/sizeof(codecs[0]); i++)
        printf("\t%s\n", codecs[i].name);
        
	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */

/*----------------------------------------------------------------------------*/
/*----------------------------------- READLINE -------------------------------*/
/*----------------------------------------------------------------------------*/
static int clear_carriage_return(char *s, int len)
{
	char cr[2] = {0x0D, 0x00};
	char *cr_pos;
	cr_pos = (char *)memmem(s, len, cr, 1);
	if (cr_pos)
	{
		*cr_pos = 0;
		len = (unsigned long)cr_pos - (unsigned long)s;
	}
	cr[0] = 0x0A;
	cr_pos = (char *)memmem(s, len, cr, 1);
	if (cr_pos)
	{
		*cr_pos = 0;
		len = (unsigned long)cr_pos - (unsigned long)s;
	}	
	
	return len;
}
/*----------------------------------------------------------------------------*/
void rl_cb(char* line)
{
	int nread=0;
    if (NULL==line) {
         return;
    }
	nread = strlen(line);
    if(nread > 0) add_history(line);
#if 0
    printf("You typed: len=%d, \n%s\n", strlen(line), line);
#endif    
	nread = clear_carriage_return(line, nread);
	ks_cli_command(STDOUT_FILENO, line);
    free(line);
}
/*----------------------------------------------------------------------------*/
static char *ks_completion_function (const char *text, int state)
{
	return NULL;
}
/* --------------------------------------------------------------------------- */
char** ks_completion (const char* text, int start, int end)
{
	char **matches;
	
	matches = ks_cli_completion_matches(rl_line_buffer, &rl_line_buffer[start]);
	return matches;
}
/* --------------------------------------------------------------------------- */
static int console_close(void)
{
	rl_callback_handler_remove();

	return 0;
}
/* --------------------------------------------------------------------------- */
static int console_init(void)
{
	fd_console_std_in = STDIN_FILENO;
	rl_bind_key ('\t', rl_complete);
	rl_attempted_completion_function = ks_completion;
	rl_completion_entry_function = ks_completion_function;
	
    rl_callback_handler_install(console_ps1, rl_cb);
	return 0;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
short get_events(struct pollfd *list, int len, int fd)
{
	short events = 0;
	int i,find=0;
	if (list)
	{
		for (i=0;(i<len) && !find;i++)
		{
			find = (list[i].fd == fd);
			if (find) events = list[i].revents;
		}
	}
	return events;
}
/*----------------------------------------------------------------------------*/
static void __quit_handler(int num)
{
	need_quit = 1;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*-------------------------------- RTP ---------------------------------------*/
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
static int rtp_filter_set(struct s_voice_man	*pvman, int session_id, struct rtp_filter_params *parm)
{
	struct rtp_session	*prtp;
	if (pvman)
	{
		list_for_each_entry(prtp, &pvman->rtp_list, list)
		{
			if (((prtp->h_rtp > 0) && (prtp->h_rtp == session_id)) ||
				(session_id <= 0))
			{
				rtp_session_filter_set(prtp, parm);
			}
		}
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
static int rtp_add(struct s_voice_man	*pvman, struct rtp_session_params *params)
{
	struct rtp_session	*prtp;
	if (pvman && params)
	{
		prtp = malloc(sizeof(*prtp));
		if (rtp_session_open(prtp, params) < 0)
		{
			free(prtp);
		}
		else
		{
			pvman->rtp_list_lenght++;
			list_add(&(prtp->list), &(pvman->rtp_list));
		}
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
static int rtp_show(struct s_voice_man	*pvman, int session_id)
{
	struct rtp_session	*prtp;
	if (pvman)
	{
		fprintf(stdout,"RTP Sessions:\n");
		fprintf(stdout,"                  sessions count : %d\n", pvman->rtp_list_lenght);
		list_for_each_entry(prtp, &pvman->rtp_list, list)
		{
			if (((prtp->h_rtp > 0) && (prtp->h_rtp == session_id)) ||
				(session_id <= 0))
			{
				rtp_session_fshow(stdout, prtp);
			}
		}
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
static int rtp_del(struct s_voice_man	*pvman, int session_id)
{
	struct rtp_session	*prtp;
	struct list_head *pos,*tmp;
	if (pvman)
	{
		list_for_each_safe(pos, tmp, &pvman->rtp_list)
		{
			prtp= list_entry(pos, struct rtp_session, list);
			if (((prtp->h_rtp > 0) && (prtp->h_rtp == session_id)) ||
				(session_id <= 0))
			{
				fprintf(stdout, "Delete RTP session %d\n", prtp->h_rtp);
				list_del(pos);
				rtp_session_close(prtp);
				free(prtp);
				pvman->rtp_list_lenght--;
			}
		}
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_filter_set(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	struct rtp_filter_params filter_param={0};
	int deep = 3;
	int session_id = -1;
	if ((argc != deep) && (argc != (deep+1)) && (argc != (deep+2)))
		return RESULT_SHOWUSAGE;
		
	if (deep < argc)
	{
		session_id = atoi(argv[deep]);
		
		if ((deep+1)< argc)
		{
			parse_filter_param(argv[deep+1], &filter_param);
		}
	}
	
	rtp_filter_set(pvman, session_id, &filter_param);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_filter_reset(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int deep = 3;
	int session_id = -1;
	if ((argc != deep) && (argc != (deep+1)))
		return RESULT_SHOWUSAGE;
		
	if (deep < argc)
	{
		session_id = atoi(argv[deep]);
	}
	
	rtp_filter_set(pvman, session_id, NULL);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_show(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int deep = 2;
	int session_id = -1;
	if ((argc != deep) && (argc != (deep+1)))
		return RESULT_SHOWUSAGE;
		
	if (deep < argc)
	{
		session_id = atoi(argv[deep]);
	}
	rtp_show(pvman, session_id);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_add(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int deep = 2;
	struct rtp_session_params	params;
	memset(&params, 0, sizeof(params));
	params.channel = -1; /* no mux */
	
	if ((argc <= deep) || (argc > (deep+5)))
		return RESULT_SHOWUSAGE;
	
	if (deep < argc)
	{
		sprintf(params.payload, argv[deep]);
	}
	
	if ((deep+1) < argc)
	{
		sprintf(params.dest, argv[deep+1]);
	}
	
	if ((deep+2) < argc)
	{
		params.channel = atoi(argv[deep+2]);
	}
	
	if ((deep+3) < argc)
	{
		sprintf(params.attr, argv[deep+3]);
	}
	
	if ((deep+4) < argc)
	{
		params.pf = atoi(argv[deep+4]);
	}

	rtp_add(pvman, &params);
	
	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_del(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int deep = 2;
	int session_id = 0;
	if ((argc != deep) && (argc != (deep+1)))
		return RESULT_SHOWUSAGE;
		
	if (deep < argc)
	{
		session_id = atoi(argv[deep]);
	}
	rtp_del(pvman, session_id);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*-------------------------------- RTP ETENSION ------------------------------*/
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
static int rtp_ext_send(struct s_voice_man	*pvman, int rtp_ext_session_id, struct sig_proto *pproto)
{
	int res = 0;
	struct rtp_ext_session	*prtpe;
	if (pvman && pproto)
	{
		fprintf(stdout,"RTP Extension Send:\n");
		list_for_each_entry(prtpe, &pvman->rtp_ext_list, list)
		{
			if (((prtpe->h_rtp_ext > 0) && (prtpe->h_rtp_ext == rtp_ext_session_id)) ||
				(rtp_ext_session_id <= 0))
			{
				res = rtp_ext_session_write(prtpe, pproto);
				if (res > 0)
					fprintf(stdout,"session=%d: send OK\n", prtpe->h_rtp_ext);
				else	
					fprintf(stdout,"session=%d: send error %d\n", prtpe->h_rtp_ext, res);
			}
		}
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
static int rtp_ext_attach(struct s_voice_man	*pvman, struct rtp_ext_session_params *params)
{
	struct rtp_ext_session	*prtpe;
	if (pvman && params)
	{
		prtpe = malloc(sizeof(*prtpe));
		if (rtp_ext_session_open(prtpe, params) < 0)
		{
			free(prtpe);
		}
		else
		{
			pvman->rtp_ext_list_lenght++;
			list_add(&(prtpe->list), &(pvman->rtp_ext_list));
		}
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
static int rtp_ext_show(struct s_voice_man	*pvman, int rtp_ext_session_id)
{
	struct rtp_ext_session	*prtpe;
	if (pvman)
	{
		fprintf(stdout,"RTP Extension Sessions:\n");
		fprintf(stdout,"                  sessions count : %d\n", pvman->rtp_ext_list_lenght);
		list_for_each_entry(prtpe, &pvman->rtp_ext_list, list)
		{
			if (((prtpe->h_rtp_ext > 0) && (prtpe->h_rtp_ext == rtp_ext_session_id)) ||
				(rtp_ext_session_id <= 0))
			{
				rtp_ext_session_fshow(stdout, prtpe);
			}
		}
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
static int rtp_ext_detach(struct s_voice_man	*pvman, int rtp_ext_session_id)
{
	struct rtp_ext_session	*prtpe;
	struct list_head *pos,*tmp;
	if (pvman)
	{
		list_for_each_safe(pos, tmp, &pvman->rtp_ext_list)
		{
			prtpe= list_entry(pos, struct rtp_ext_session, list);
			if (((prtpe->h_rtp_ext > 0) && (prtpe->h_rtp_ext == rtp_ext_session_id)) ||
				(rtp_ext_session_id <= 0))
			{
				fprintf(stdout, "Delete RTP Extension session %d\n", prtpe->h_rtp_ext);
				list_del(pos);
				rtp_ext_session_close(prtpe);
				free(prtpe);
				pvman->rtp_ext_list_lenght--;
			}
		}
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_ext_detach(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int deep = 2;
	int rtp_ext_session_id = 0;
	if ((argc != deep) && (argc != (deep+1)))
		return RESULT_SHOWUSAGE;
		
	if (deep < argc)
	{
		rtp_ext_session_id = atoi(argv[deep]);
	}
	rtp_ext_detach(pvman, rtp_ext_session_id);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_ext_show(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int deep = 2;
	int rtp_ext_session_id = 0;
	if ((argc != deep) && (argc != (deep+1)))
		return RESULT_SHOWUSAGE;
		
	if (deep < argc)
	{
		rtp_ext_session_id = atoi(argv[deep]);
	}
	rtp_ext_show(pvman, rtp_ext_session_id);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_ext_attach(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	struct rtp_ext_session_params params;
	int deep = 2;
	int rtp_session_id = 0;
	if ((argc != deep) && (argc != (deep+1)))
		return RESULT_SHOWUSAGE;
		
	if (deep < argc)
	{
		rtp_session_id = atoi(argv[deep]);
	}
	params.h_rtp = rtp_session_id;
	rtp_ext_attach(pvman, &params);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_ext_send_gw_flag(int fd, int argc, char *argv[])
{
	return vman_rtp_ext_send_uni(GW_FLAG, fd, argc, argv);
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_ext_send_gw_flag_ptt(int fd, int argc, char *argv[])
{
	return vman_rtp_ext_send_uni(GW_FLAG_PTT, fd, argc, argv);
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_ext_send_gw_flag_bss(int fd, int argc, char *argv[])
{
	return vman_rtp_ext_send_uni(GW_FLAG_BSS, fd, argc, argv);
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_ext_send_gw_flag_recv(int fd, int argc, char *argv[])
{
	return vman_rtp_ext_send_uni(GW_FLAG_RV, fd, argc, argv);
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_ext_send_gw_flag_reserv(int fd, int argc, char *argv[])
{
	return vman_rtp_ext_send_uni(GW_FLAG_RS, fd, argc, argv);
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_ext_send_gw_flag_power(int fd, int argc, char *argv[])
{
	return vman_rtp_ext_send_uni(GW_FLAG_PW, fd, argc, argv);
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_ext_send_gw_flag_selfhear(int fd, int argc, char *argv[])
{
	return vman_rtp_ext_send_uni(GW_FLAG_SH, fd, argc, argv);
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_ext_send_uni(int packet_type, int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int deep = 3;
	int rtp_ext_session_id = 0;
	int flag = 0;
	int params_count = 0;
	struct sig_proto_flags	flags={0};
	
	if ((argc != deep) && (argc != (deep+1)) && (argc != (deep+2)))
		return RESULT_SHOWUSAGE;
		
	if (deep < argc)
	{
		rtp_ext_session_id = atoi(argv[deep]);
	}
	
	if ((deep+1) < argc)
	{
		flag = atoi(argv[deep+1]);
		params_count = parse_sig_proto_flags(argv[deep+1], &flags);
		if (params_count <= 1)
		{
			memset(&flags, 0, sizeof(flags));
			if (packet_type == GW_FLAG_PTT)
				flags.flag_p = flag;
			if (packet_type == GW_FLAG_BSS)
				flags.flag_b = flag;
			if (packet_type == GW_FLAG_RV)
				flags.flag_rv = flag;
			if (packet_type == GW_FLAG_SH)
				flags.flag_sh = flag;
			if (packet_type == GW_FLAG_PW)
				flags.flag_pw = flag;
			if (packet_type == GW_FLAG_RS)
				flags.flag_rs = flag;
		}
	}
	sig_proto_init(&sig_proto_tx);
	sig_proto_tx.type = packet_type;
	sig_proto_tx.flags = flags;

	rtp_ext_send(pvman, rtp_ext_session_id, &sig_proto_tx);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_ext_send_gw_adjust_chan(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int deep = 3;
	int rtp_ext_session_id = 0;
	int params_count = 0;
	int chan_id = 0;
	float chan_freq = 0.0;
	
	if ((argc != deep) && (argc != (deep+1)) && (argc != (deep+2)))
		return RESULT_SHOWUSAGE;
		
	if (deep < argc)
	{
		rtp_ext_session_id = atoi(argv[deep]);
	}
	
	if ((deep+1) < argc)
	{
		params_count = parse_sig_proto_adj_chan(argv[deep+1], &chan_id, &chan_freq);
	}
	
	sig_proto_init(&sig_proto_tx);
	sig_proto_tx.type = GW_ADJUST_CHAN;
	sig_proto_tx.payload.gw_adjust_chan.id = chan_id;
	sig_proto_tx.payload.gw_adjust_chan.freq = chan_freq;	

	rtp_ext_send(pvman, rtp_ext_session_id, &sig_proto_tx);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_ext_send_gw_sel_call(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int deep = 3;
	int rtp_ext_session_id = 0;
	int callerid_size = 0;
	char *callerid=NULL;
	
	if ((argc != deep) && (argc != (deep+1)) && (argc != (deep+2)))
		return RESULT_SHOWUSAGE;
		
	if (deep < argc)
	{
		rtp_ext_session_id = atoi(argv[deep]);
	}
	
	if ((deep+1) < argc)
	{
		callerid = argv[deep+1];
	}
	
	sig_proto_init(&sig_proto_tx);
	sig_proto_tx.type = GW_SEL_CALL;
	sig_proto_tx.payload.gw_sel_call.callerID_length = 0;
	sig_proto_tx.payload.gw_sel_call.callerID[0] = 0;
	if (callerid)
	{		
		callerid_size = sizeof(sig_proto_tx.payload.gw_sel_call.callerID);
		memcpy(sig_proto_tx.payload.gw_sel_call.callerID, callerid, callerid_size-1);
		sig_proto_tx.payload.gw_sel_call.callerID_length = strlen(sig_proto_tx.payload.gw_sel_call.callerID);
	}

	rtp_ext_send(pvman, rtp_ext_session_id, &sig_proto_tx);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_ext_send_gw_select_rc_chan(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int deep = 3;
	int rtp_ext_session_id = 0;
	int params_count = 0;
	int interface = 0;
	int channel = 0;
	
	
	if ((argc != deep) && (argc != (deep+1)) && (argc != (deep+2)))
		return RESULT_SHOWUSAGE;
		
	if (deep < argc)
	{
		rtp_ext_session_id = atoi(argv[deep]);
	}
	
	if ((deep+1) < argc)
	{
		params_count = parse_sig_proto_select_rc_chan(argv[deep+1], &interface, &channel);
	}
	
	sig_proto_init(&sig_proto_tx);
	sig_proto_tx.type = GW_SELECT_RC_CHAN;
	sig_proto_tx.payload.gw_select_rc_chan.interface = interface;
	sig_proto_tx.payload.gw_select_rc_chan.channel = channel;

	rtp_ext_send(pvman, rtp_ext_session_id, &sig_proto_tx);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_ext_send_wm_flag(int fd, int argc, char *argv[])
{
	return vman_rtp_ext_send_uni(WM_FLAG, fd, argc, argv);
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_ext_send_wm_sel_call_trans(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int deep = 3;
	int rtp_ext_session_id = 0;
	int params_count = 0;
	struct 	in_addr	session_addr;
	unsigned long	ssrc;
	char 			callerID[64]={0};
	int				callerID_size=sizeof(callerID);
	
	if ((argc != deep) && (argc != (deep+1)) && (argc != (deep+2)))
		return RESULT_SHOWUSAGE;
		
	if (deep < argc)
	{
		rtp_ext_session_id = atoi(argv[deep]);
	}
	
	if ((deep+1) < argc)
	{
		params_count = parse_sig_proto_trans(argv[deep+1], &session_addr, &ssrc, callerID, callerID_size);
	}
	
	sig_proto_init(&sig_proto_tx);
	sig_proto_tx.type = WM_SEL_CALL_TRANS;
	sig_proto_tx.payload.wm_sel_call_trans.session_addr	= session_addr;
	sig_proto_tx.payload.wm_sel_call_trans.ssrc			= ssrc;
	
	memcpy(sig_proto_tx.payload.wm_sel_call_trans.callerID, callerID, callerID_size-1);
	sig_proto_tx.payload.wm_sel_call_trans.callerID_length = strlen(sig_proto_tx.payload.wm_sel_call_trans.callerID);

	rtp_ext_send(pvman, rtp_ext_session_id, &sig_proto_tx);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_ext_send_wm_trans(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int deep = 3;
	int rtp_ext_session_id = 0;
	int params_count = 0;
	struct 	in_addr	session_addr;
	unsigned long	ssrc;
	
	if ((argc != deep) && (argc != (deep+1)) && (argc != (deep+2)))
		return RESULT_SHOWUSAGE;
		
	if (deep < argc)
	{
		rtp_ext_session_id = atoi(argv[deep]);
	}
	
	if ((deep+1) < argc)
	{
		params_count = parse_sig_proto_trans(argv[deep+1], &session_addr, &ssrc, NULL, 0);
	}
	
	sig_proto_init(&sig_proto_tx);
	sig_proto_tx.type = WM_TRANS;
	sig_proto_tx.payload.wm_trans.session_addr	= session_addr;
	sig_proto_tx.payload.wm_trans.ssrc			= ssrc;

	rtp_ext_send(pvman, rtp_ext_session_id, &sig_proto_tx);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_ext_send_wm_chan(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int deep = 3;
	int rtp_ext_session_id = 0;
	int params_count = 0;
	int chan_id = 0;
	float chan_freq = 0.0;
	
	if ((argc != deep) && (argc != (deep+1)) && (argc != (deep+2)))
		return RESULT_SHOWUSAGE;
		
	if (deep < argc)
	{
		rtp_ext_session_id = atoi(argv[deep]);
	}
	
	if ((deep+1) < argc)
	{
		params_count = parse_sig_proto_adj_chan(argv[deep+1], &chan_id, &chan_freq);
	}
	
	sig_proto_init(&sig_proto_tx);
	sig_proto_tx.type = WM_CHAN;
	sig_proto_tx.payload.wm_chan.id = chan_id;
	sig_proto_tx.payload.wm_chan.freq = chan_freq;	

	rtp_ext_send(pvman, rtp_ext_session_id, &sig_proto_tx);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_ext_send_wm_radio_quality(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int deep = 3;
	int rtp_ext_session_id = 0;
	float radio_quality = 0.0;
	
	if ((argc != deep) && (argc != (deep+1)) && (argc != (deep+2)))
		return RESULT_SHOWUSAGE;
		
	if (deep < argc)
	{
		rtp_ext_session_id = atoi(argv[deep]);
	}
	
	if ((deep+1) < argc)
	{
		radio_quality = atof(argv[deep+1]);
	}
	
	sig_proto_init(&sig_proto_tx);
	sig_proto_tx.type = WM_RADIO_QUALITY;
	sig_proto_tx.payload.wm_radio_quality.radio_quality = radio_quality;	

	rtp_ext_send(pvman, rtp_ext_session_id, &sig_proto_tx);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_rtp_ext_send_wm_select_rc_chan(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int deep = 3;
	int rtp_ext_session_id = 0;
	int params_count = 0;
	int interface = 0;
	int channel = 0;
	
	
	if ((argc != deep) && (argc != (deep+1)) && (argc != (deep+2)))
		return RESULT_SHOWUSAGE;
		
	if (deep < argc)
	{
		rtp_ext_session_id = atoi(argv[deep]);
	}
	
	if ((deep+1) < argc)
	{
		params_count = parse_sig_proto_select_rc_chan(argv[deep+1], &interface, &channel);
	}
	
	sig_proto_init(&sig_proto_tx);
	sig_proto_tx.type = WM_SELECT_RC_CHAN;
	sig_proto_tx.payload.wm_select_rc_chan.interface = interface;
	sig_proto_tx.payload.wm_select_rc_chan.channel = channel;

	rtp_ext_send(pvman, rtp_ext_session_id, &sig_proto_tx);

	return RESULT_SUCCESS;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*------------------------------------- D S P --------------------------------*/
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
static int vman_dsp_list(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int deep = 2;

	if ((argc != deep) && (argc != (deep+1)))
		return RESULT_SHOWUSAGE;
		
	pvman->dsp_list_lenght = dsp_list_fill(&pvman->dsp_list);
	dsp_list_show(stdout, &pvman->dsp_list);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_dsp_vad_on(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int deep = 2;
	int ts = 0;
	if ((argc != deep) && (argc != (deep+1)))
		return RESULT_SHOWUSAGE;
	
	if (deep < argc)
	{
		ts = atoi(argv[deep]);
	}	
		
	dsp_vad_on(&pvman->vad, ts);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_dsp_vad_off(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int deep = 2;
	int ts = 0;
	if ((argc != deep) && (argc != (deep+1)))
		return RESULT_SHOWUSAGE;
	
	if (deep < argc)
	{
		ts = atoi(argv[deep]);
	}
		
	dsp_vad_off(&pvman->vad, ts);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_dsp_vad_show(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int deep = 2;
	int ts = 0;
	if ((argc != deep) && (argc != (deep+1)))
		return RESULT_SHOWUSAGE;
	
	if (deep < argc)
	{
		ts = atoi(argv[deep]);
	}
		
	dsp_vad_show(&pvman->vad, ts);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_dsp_install_codec(int fd, int argc, char *argv[])
{
	struct dsp_install_info	info;
	int deep = 2;

	if ((argc != deep) && (argc != (deep+1)) && (argc != (deep+2)))
		return RESULT_SHOWUSAGE;

	if (deep < argc)
	{
		info.dsp = atoi(argv[deep]);
		if ((deep+1) < argc)
		{
			info.codec_id = get_codec_id(argv[deep+1]);
		}
	}

	if (dsp_install_codec(&info) == 0)
	{
		fprintf(stdout, "vman2: codec %s(%d) installed on dsp %d\n", argv[deep+1], info.codec_id, info.dsp);
	}
	
	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_dsp_uninstall_codec(int fd, int argc, char *argv[])
{
	int deep = 2;
	int dsp = 0;
	if ((argc != deep) && (argc != (deep+1)))
		return RESULT_SHOWUSAGE;

	if (deep < argc)
	{
		dsp = atoi(argv[deep]);
	}

	dsp_uninstall_codec(dsp);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_dsp_channel_info(int fd, int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int deep = 3;
	int channel = 0;
	if ((argc != deep) && (argc != (deep+1)))
		return RESULT_SHOWUSAGE;

	if (deep < argc)
	{
		channel = atoi(argv[deep]);
	}

	dsp_channel_info(channel, &pvman->channel_list);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*------------------------------------- M U X --------------------------------*/
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
static int vman_mux_connect(int fd, int argc, char *argv[])
{
	struct s_mux_conn	mux_con={0};
	int deep = 2;

	if ((argc != deep) && (argc != (deep+1)) && (argc != (deep+2)))
		return RESULT_SHOWUSAGE;

	if (deep < argc)
	{
		mux_con.dest = atoi(argv[deep]);
		if ((deep+1) < argc)
		{
			mux_con.src = atoi(argv[deep+1]);
		}
	}

	mux_connect(&mux_con);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_mux_disconnect(int fd, int argc, char *argv[])
{
	unsigned int chn = 0;
	int deep = 2;

	if ((argc != deep) && (argc != (deep+1)))
		return RESULT_SHOWUSAGE;

	if (deep < argc)
	{
		chn = atoi(argv[deep]);
	}
	mux_disconnect(chn);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_mux_clear(int fd, int argc, char *argv[])
{
	int deep = 2;

	if (argc != deep)
		return RESULT_SHOWUSAGE;

	mux_clear();

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
#ifdef BFIN_SPI_TDM
/* -------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*------------------------------------- S P I --------------------------------*/
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
static int vman_spi_add(int fd, int argc, char *argv[])
{
	struct s_spi_channel_info	info={0};
	int deep = 2;
	info.tdm = -1; /* dynamic allocate */

	if ((argc != deep) && (argc != (deep+1)) && (argc != (deep+2)))
		return RESULT_SHOWUSAGE;

	if (deep < argc)
	{
		info.spi_tdm_channel = atoi(argv[deep]);
		if ((deep+1) < argc)
		{
			info.tdm = atoi(argv[deep+1]);
		}
	}

	spi_add_channel(&info);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_spi_remove(int fd, int argc, char *argv[])
{
	struct s_spi_channel_info	info={0};
	int deep = 2;
	info.tdm = -1; /* dynamic allocate */

	if ((argc != deep) && (argc != (deep+1)))
		return RESULT_SHOWUSAGE;

	if (deep < argc)
	{
		info.spi_tdm_channel = atoi(argv[deep]);
	}

	spi_remove_channel(&info);

	return RESULT_SUCCESS;
}
/* -------------------------------------------------------------------------- */
static int vman_spi_stat(int fd, int argc, char *argv[])
{
	struct s_spi_stat_info	info={0};
	int deep = 2;
	
	if ((argc != deep) && (argc != (deep+1)))
		return RESULT_SHOWUSAGE;

	if (deep < argc)
	{
		info.spi_tdm_channel = atoi(argv[deep]);
	}

	if (!spi_get_stat(&info))
	{
		fprintf(stdout, "\n");
		fprintf(stdout, "         channel:%d\n", info.spi_tdm_channel);
		fprintf(stdout, "          in_use:%d\n", info.in_use);
		fprintf(stdout, "             tdm:%d\n", info.tdm);

		fflush(stdout);
	}

	return RESULT_SUCCESS;
}
#endif
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*------------------------------------- M A I N ------------------------------*/
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
int main(int argc, char *argv[])
{
	struct s_voice_man	*pvman = &vman;
	int 				alive;
	int 				res=0;
	int 				events=0;
	sigset_t 			sigs;
	struct pollfd 		readfds[32];
	int 				readfds_size;	
	int 				readfds_len;
	struct rtp_ext_session		*prtpe;
	struct list_head 			*pos,*tmp;	

	if (get_options(argc, argv) < 0) return 0;
	
	vman_default_config(&vman);
	
	vman_init(&vman);
	
	cli_init();
	cli_register_multiple(cli_vman, sizeof(cli_vman) / sizeof(cli_vman[0]));
	
	console_init();
	
	/* */
	sigemptyset(&sigs);
	sigaddset(&sigs, SIGHUP);
	sigaddset(&sigs, SIGTERM);
	sigaddset(&sigs, SIGINT);
	sigaddset(&sigs, SIGPIPE);
	sigaddset(&sigs, SIGWINCH);

	signal(SIGINT, __quit_handler);
	signal(SIGTERM, __quit_handler);
	signal(SIGPIPE, SIG_IGN);

	
	alive = 1;
	while (alive)
	{
		if (need_quit)
		{
			need_quit = 0;
			alive = 0;
		}
		
		readfds_size = sizeof(readfds)/sizeof(readfds[0]);
		readfds_len = 0;		
		
		if (fd_console_std_in > -1)
		{
			readfds[readfds_len].fd = fd_console_std_in;
			readfds[readfds_len].events = POLLIN;
			readfds_len++;
		}
		
		if (pvman->vad.hdsp > -1)
		{
			readfds[readfds_len].fd = pvman->vad.hdsp;
			readfds[readfds_len].events = POLLIN;
			readfds_len++;
		}		
		
		list_for_each_safe(pos, tmp, &pvman->rtp_ext_list)
		{
			prtpe= list_entry(pos, struct rtp_ext_session, list);
			if ((prtpe->h_rtp_ext > 0) && (readfds_len < readfds_size))
			{
				readfds[readfds_len].fd = prtpe->h_rtp_ext;
				readfds[readfds_len].events = POLLIN;
				readfds_len++;
			}
		}

		res = poll(readfds, readfds_len, 100 /* timeout */);

		if (res < 0)
		{
			fprintf(stderr, "poll failed: %s\n", strerror(errno));
		}
		else
		{
			if (res == 0)
			{
				/* time out */
			}
			else
			{
				/* events rtp extension session */
				list_for_each_safe(pos, tmp, &pvman->rtp_ext_list)
				{
					prtpe= list_entry(pos, struct rtp_ext_session, list);
					events = get_events(readfds, readfds_len, prtpe->h_rtp_ext);
					if (events & POLLIN) 
					{
						prtpe->poll_count++;
						rtp_ext_session_read(prtpe);
					}
				}
				
				/* event console client */
				if (fd_console_std_in > -1) 
				{	
					events = get_events(readfds, readfds_len, fd_console_std_in);
					if (events & POLLIN) 
					{
						rl_callback_read_char();
					}
				}
				
				/* events DSP VADs */
				if (pvman->vad.hdsp > -1)
				{
					events = get_events(readfds, readfds_len, pvman->vad.hdsp);
					if (events & POLLIN) 
					{
						dsp_read(&pvman->vad);
					}
				}
			}
			
			/* TO */
			list_for_each_safe(pos, tmp, &pvman->rtp_ext_list)
			{
				prtpe= list_entry(pos, struct rtp_ext_session, list);
				rtp_ext_session_process(prtpe, NULL);
			}
		}
	}
	
	console_close();
	vman_close(&vman);
	return 0;
}

