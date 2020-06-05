#ifndef _SPI_TDM_IOCTL_H_
#define _SPI_TDM_IOCTL_H_

#define IN
#define OUT

#define SPI_TDM_BASE	'S'


/* ************************************************************ */
/*						S P I   T D M	    					*/
/* ************************************************************ */

/* spi-tdm-related structures and ioctls */
#define SPI_TDM_DEVNAME	"spi_tdm"

#define IN
#define OUT

struct s_spi_tdm_channel_info
{
	IN int spi_tdm_channel;
	IN int tdm;
};

struct s_spi_tdm_stat_info
{
	IN  int spi_tdm_channel;
	OUT int in_use; 
	OUT int tdm;
};

#define SPI_TDM_ADD_CHANNEL		_IOWR(SPI_TDM_BASE, 1, struct s_spi_tdm_channel_info)
#define SPI_TDM_REMOVE_CHANNEL	_IOWR(SPI_TDM_BASE, 2, struct s_spi_tdm_channel_info)
#define SPI_TDM_STAT_INFO		_IOWR(SPI_TDM_BASE, 3, struct s_spi_tdm_stat_info)

#endif /* _SPI_TDM_IOCTL_H_ */
