#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h>
#include <errno.h>

#include "file.h"
#include "spi_tdm.h"
#include "spi-tdm-ioctl.h"

/* -------------------------------------------------------------------------- */
int spi_add_channel(struct s_spi_channel_info *c)
{
	struct s_spi_tdm_channel_info info = {0};
	int spi_ctl;
	int result = 0;
	
	spi_ctl = open_spi_tdm();
	if (spi_ctl < 0)
	{
		fprintf(stderr, "spi: error open spi_tdm '%s(%d)'\n", strerror(errno), errno);
		fflush(stderr);
	}
	else
	{
		info.spi_tdm_channel = c->spi_tdm_channel;
		info.tdm = c->tdm;

		result = ioctl(spi_ctl, SPI_TDM_ADD_CHANNEL, &info);
		if (result < 0)
		{
			fprintf(stderr, "spi: failed to add channel=%d, tdm=%d: %s\n", info.spi_tdm_channel, info.tdm, strerror(errno));
		}

		close(spi_ctl);
	}
	return result;
}
/* -------------------------------------------------------------------------- */
int spi_remove_channel(struct s_spi_channel_info *c)
{
	struct s_spi_tdm_channel_info info = {0};
	int spi_ctl;
	int result = 0;
	
	spi_ctl = open_spi_tdm();
	if (spi_ctl < 0)
	{
		fprintf(stderr, "spi: error open spi_tdm '%s(%d)'\n", strerror(errno), errno);
		fflush(stderr);
	}
	else
	{
		info.spi_tdm_channel = c->spi_tdm_channel;
		info.tdm = c->tdm;

		result = ioctl(spi_ctl, SPI_TDM_REMOVE_CHANNEL, &info);
		if (result < 0)
		{
			fprintf(stderr, "spi: failed to remove channel=%d: %s\n", info.spi_tdm_channel, strerror(errno));
		}

		close(spi_ctl);
	}
	return result;
}
/* -------------------------------------------------------------------------- */
int spi_get_stat(struct s_spi_stat_info *s)
{
	struct s_spi_tdm_stat_info info = {0};
	int spi_ctl;
	int result = 0;
	spi_ctl = open_spi_tdm();
	if (spi_ctl < 0)
	{
		fprintf(stderr, "spi: error open spi_tdm '%s(%d)'\n", strerror(errno), errno);
		fflush(stderr);
	}
	else
	{
		info.spi_tdm_channel = s->spi_tdm_channel;

		result = ioctl(spi_ctl, SPI_TDM_STAT_INFO, &info);

		if (result < 0)
		{
			fprintf(stderr, "spi: failed to get stat channel=%d: %s\n", info.spi_tdm_channel, strerror(errno));
		}
		else
		{
			s->in_use	= info.in_use;
			s->tdm		= info.tdm;
		}

		close(spi_ctl);
	}
	return result;
}
/* -------------------------------------------------------------------------- */
