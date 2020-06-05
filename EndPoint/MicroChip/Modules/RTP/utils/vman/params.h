/*
 * $Id: params.h,v 1.1 2006/08/31 15:11:14 root Exp $
 */

#ifndef __PARAMS_H
#define __PARAMS_H

struct record record;
struct play play;
struct install install;
struct param_dsp_mem dsp_read_param;
struct param_dsp_mem dsp_write_param;
struct mux_conn mux_conn;
struct mux_msg_write mux_msg_write;
struct upload_dsp_image upload_dsp_image_param;
struct test_dsp_param test_dsp_param;
struct dump_dsp dump_dsp;
struct mux_read_mem mux_mem;
struct dump_mux_dm dump_mux_dm;

struct rtp_session rtp;

struct _mux_tx_mode mtx_mode;
struct mux_rx_add mrx_add;
struct mux_rx_remove mrx_remove;
struct mux_rx_clear mrx_clear;

struct playtone ptone;

char *dump_cm_filter;

struct param app_params[] = {
	{
	 param_func,
	 0, "connect",
	 2,
	 mux_connect,
	 &mux_conn,
	 {
	  {option_int, "dest channel", &mux_conn.dest},
	  {option_int, "source channel", &mux_conn.src},
	  },
	 },
	{
	 param_func,
	 0, "disconnect",
	 1,
	 mux_disconnect,
	 0,
	 {
	  {option_int, "channel"},
	  },
	 },
	{
	 param_func,
	 0, "clear",
	 1,
	 mux_clear,
	 0,
	 {
	  { option_string, "region" },
	 }
	},
	{
	 param_func,
	 'v', "version",
	 0,
	 vman_version},

	{
	 param_func,
	 0, "dumpcm",
	 0,
	 mux_list,
	 0,
		{
			{option_string, "filters"},
		}
	 },

	{
	 param_func,
	 'h', "help",
	 0,
	 vman_usage,
	 },
	{
	 param_func,
	 'r', "record",
	 2,
	 record_file,
	 &record,
	 {
	  {option_string, "codec id", &record.codec_id},
	  {option_int, "source channel", &record.channel},
	  {option_string, "output file name", &record.filename},
	  {option_int, "maximum file size", &record.max_size},
	  }
	 },
	{
	 param_func,
	 'p', "play",
	 2,
	 play_file,
	 &play,
	 {
	  {option_string, "codec id", &play.codec_id},
	  {option_int, "destination channel", &play.channel},
	  {option_string, "input file name", &play.filename},
	  }
	 },
	{
	 param_func,
	 'i', "install",
	 2,
	 install_codec,
	 &install,
	 {
	  {option_int, "dsp number", &install.dsp},
	  {option_string, "codec id", &install.codec_id},
	  }
	 },
	{
	 param_func,
	 'u', "uninstall",
	 1,
	 uninstall_codec,
	 0,
	 {
	  {option_int, "dsp number"},
	  }
	 },
	{
	 param_func | DEFAULT_PARAM,
	 'l', "listdsp",
	 0,
	 list_dsp,
	 },
	{
	 param_func,
	 'c', "clock",
	 0,
	 voice_set_clock_source,
	 0,
	 {
	  {option_string, "clock source"}
	  }
	 },
	{
	 param_func,
	 'w', "write",
	 3,
	 write_dsp_mem,
	 &dsp_write_param,
	 {
	  {option_int, "dsp number", &dsp_write_param.number},
	  {option_int, "dsp memory address", &dsp_write_param.address},
	  {option_int, "dsp data", &dsp_write_param.data},
	  {option_int, "dsp overlay number", &dsp_write_param.overlay_page},
	  }
	 },
	{
	 param_func,
	 0, "reset",
	 1,
	 reset_dsp,
	 0,
	 {
	  {option_int, "dsp number"},
	  }
	 },
	{
	 param_func,
	 0, "upload",
	 2,
	 upload_dsp_image,
	 &upload_dsp_image_param,
	 {
	  {option_int, "dsp number", &upload_dsp_image_param.dsp},
	  {option_string, "image filename", &upload_dsp_image_param.image_name},
	  },
	 },

	{
	 param_func,
	 0, "vtest",
	 4,
	 vtest_dsp,
	 &test_dsp_param,
	 {
	  {option_int, "dsp number", &test_dsp_param.dsp},
	  {option_string, "mode", &test_dsp_param.mode_str},
	  {option_string, "input filename", &test_dsp_param.input_filename},
	  {option_string, "output filename", &test_dsp_param.output_filename},
	  {option_string, "crc filename", &test_dsp_param.crc_filename},
	  },
	 },

	{
	 param_func,
	 'r', "read",
	 3,
	 dump_dsp_mem,
	 &dump_dsp,
	 {
	  {option_int, "dsp number", &dump_dsp.dsp},
	  {option_int, "dsp memory address", &dump_dsp.address},
	  {option_int, "dump size", &dump_dsp.count},
	  {option_int, "dsp overlay number", &dump_dsp.overlay},
	  },
	 },
	{
	 param_func,
	 0, "dumpdm",
	 1,
	 dump_mux_data_memory,
	 &dump_mux_dm,
	 {
	  {option_int, "mux data memory address", &dump_mux_dm.address},
	  {option_int, "dump size", &dump_mux_dm.count},
	  },


	 },
	{
	 param_func,
	 0, "mrd",
	 1,
	 mux_read_mem,
	 &mux_mem,
	 {
	  {option_int, "channel", &mux_mem.channel},
	  {option_set_flag, "data mem", &mux_mem.mem_type},
	  },
	 },
	{
	 param_func,
	 0, "mrc",
	 1,
	 mux_read_mem,
	 &mux_mem,
	 {
	  {option_int, "channel", &mux_mem.channel},
	  {option_reset_flag, "connection mem", &mux_mem.mem_type},
	  },
	 },
	{
	 param_func,
	 0, "rtp",
	 3,
	 rtp_session,
	 &rtp,
	 {
	  {option_string, "payload", &rtp.payload},
	  {option_string, "destination", &rtp.dest},
	  {option_int, "channel", &rtp.channel},
//	  {option_int, "vad", &rtp.vad},
//	  {option_int, "gain", &rtp.gain},
	  {option_string, "attr", &rtp.attr},
	  {option_int, "pf", &rtp.pf},
	  },
	 },


	{
	 param_func,
	 0, "muxlist",
	 0,
	 mux_enum_devs},

/*
	{
	 param_func,
	 0, "rtpinfo",
	 0,
	 do_channelinfo},

	{
	 param_func,
	 0, "muxmode",
	 2,
	 mux_tx_mode,
	 &mtx_mode,
	 {
	  {option_int, "dsp number", &mtx_mode.dev_no},
	  {option_int, "conf number", &mtx_mode.conf_no},
	  {option_string, "flag1", &mtx_mode.flag1},
	  {option_string, "flag2", &mtx_mode.flag2},
	  {option_string, "flag3", &mtx_mode.flag3},
	  },
	 },
*/
	{
	 param_func,
	 0, "muxadd",
	 4,
	 mux_rx_add,
	 &mrx_add,
	 {
	  {option_int, "dsp number", &mrx_add.dev_no},
	  {option_int, "conf number", &mrx_add.conf_no},
	  {option_int, "channel", &mrx_add.chn},
	  {option_int, "gain", &mrx_add.gain},
	  },
	 },

	{
	 param_func,
	 0, "muxremove",
	 3,
	 mux_rx_remove,
	 &mrx_remove,
	 {
	  {option_int, "dsp number", &mrx_remove.dev_no},
	  {option_int, "conf number", &mrx_remove.conf_no},
	  {option_int, "channel", &mrx_remove.chn},
	  }
	 },

	{
	 param_func,
	 0, "muxclear",
	 2,
	 mux_rx_clear,
	 &mrx_clear,
	 {
	  {option_int, "dsp number", &mrx_clear.dev_no},
	  {option_int, "conf number", &mrx_clear.conf_no},
	  },
	 },

	{
	 param_func,
	 0, "sysclk",
	 0,
	 show_sysclk_status,
	 },

	{
	 param_func,
	 0, "mrx",
	 1,
	 mux_read_msg,
	 0,
	 {
	  {option_int, "channel number"},
	  },
	 },

	{
	 param_func,
	 0, "mtx",
	 2,
	 mux_write_msg,
	 &mux_msg_write,
	 {
	  {option_int, "channel number", &mux_msg_write.channel},
	  {option_int, "data", &mux_msg_write.data},
	  },
	 },

	{
	 param_func,
	 0, "bus",
	 1,
	 dsp_show_bus,
	 0,
	 {
	  {option_int, "dsp number"},
	  },
	 },

	{
	 param_func,
	 0, "readtone",
	 0,
	 read_dtmf,
	 },

	 {
	 param_func,
	 0, "cleartone",
	 1,
	 clear_dtmf,
	 0,
	{
	    {option_int, "timeslot"},
	  },
	 },

	{
	 param_func,
	 0, "playtone",
	 2,
	 write_dtmf,
	 &ptone,
	 {
	   {option_int, "channel", &ptone.timeslot},
	   {option_string, "string", &ptone.string},
	  },
	 },

	{
	 param_func,
	 0, "enable-panic",
	 0,
	 enable_panic,
	 },

	{
	 param_func,
	 0, "disable-panic",
	 0,
	 disable_panic,
	 },


//	{ param_opt,	 0, "dsp",	 1,	 0,	 0,	{ {option_int, "dsp number", &rtp.dsp} } }
};

#endif
