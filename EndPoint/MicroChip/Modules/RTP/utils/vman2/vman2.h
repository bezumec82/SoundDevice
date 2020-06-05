#ifndef __VMAN2_H_
#define __VMAN2_H_

#include "list.h"
#include "rtp.h"
#include "rtp_ext.h"
#include "dsp.h"
#include "mux.h"

#ifdef BFIN_SPI_TDM
#include "spi_tdm.h"
#endif

struct s_codec_types
{
    int id;
    char* name;
    void* dev;
};

struct s_voice_man
{
	int 							rtp_list_lenght;
	struct list_head				rtp_list;
	
	int 							rtp_ext_list_lenght;
	struct list_head				rtp_ext_list;
	
	int 							dsp_list_lenght;
	struct list_head				dsp_list;
	
	struct list_head				channel_list;
	
	struct dsp_vad_info				vad;
};

#endif /* __VMAN2_H_ */


