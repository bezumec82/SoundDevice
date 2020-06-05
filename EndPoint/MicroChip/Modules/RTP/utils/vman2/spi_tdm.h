#ifndef _SPI_TDM_H_
#define _SPI_TDM_H_

struct s_spi_channel_info
{
	int spi_tdm_channel;
	int tdm;
};

struct s_spi_stat_info
{
	int spi_tdm_channel;
	int in_use; 
	int tdm;
};

int spi_add_channel(struct s_spi_channel_info *c);
int spi_remove_channel(struct s_spi_channel_info *c);
int spi_get_stat(struct s_spi_stat_info *s);

#endif /* _SPI_TDM_H_ */

