#ifndef _SPI_TDM_H_
#define _SPI_TDM_H_

#include "workspace.h"

/*********************************************************
 *
 * spi tdm device stuff
 *
 *********************************************************/

#define SPI_TDM_PACKET_FACTOR	1
#define SPI_TDM_MAX_CHANNELS	8

#define SPI_TDM_FIRST_CHANNEL	0
#define SPI_TDM_LAST_CHANNEL	7

#define IN
#define OUT

struct	s_spi_tdm_params
{
	IN int	spi_tdm_channel;
	IN int packet_factor;
	IN unsigned short rx_pt;
	IN unsigned short tx_pt;
	IN int tdm;
	OUT void *context;
};

struct channel;

struct s_spi_tdm_device
{
	struct channel	*link;
	struct s_spi_tdm_params	param;
	
	struct list_head list;
	struct list_head free_list;
	int timeslot;
	int spi_tdm_channel;
	int in_use;
	
	int	pcm_frame_size;
	int	code_frame_size;

	void *context;

	char owner[20];
};

int spi_tdm_allocate(const struct s_spi_tdm_params *params, struct s_spi_tdm_device **spi_tdm);
void spi_tdm_release(struct s_spi_tdm_device *spi_tdm);
int spi_tdm_timeslot(struct s_spi_tdm_device *spi_tdm);
int spi_tdm_num_of_channels(void);
int spi_tdm_set_owner(struct s_spi_tdm_device *spi_tdm, char* owner);
int spi_tdm_get_owner(struct s_spi_tdm_device *spi_tdm, char* owner);
int spi_tdm_demux(int channels_count, int channel_size, void *pdemux);
int spi_tdm_mux(int channels_count, int channel_size, void *pmux);

#endif
