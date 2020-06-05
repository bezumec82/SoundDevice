#ifndef _BFIN_SPORT_EX_H_
#define _BFIN_SPORT_EX_H_

#include <asm/atomic.h>
#include <asm/blackfin.h>
#include <uapi/asm/bfin_sport.h>
#include <asm/bfin_sport3.h>
#include <asm/dma.h>
#include <asm/cacheflush.h>
#include <asm/portmux.h>

#define BFIN_SUBBUFFER_COUNT		4
#define AUTOBUFFER_COUNT			4 /* 2,4 */
#define AUTOBUFFER_DELAY			(AUTOBUFFER_COUNT/2)


#define SPORT_PACKET_FACTOR			1
#define CHANNELS_MAX				8
#define FRAME_SIZE					(160*2) /* nsamples * sizeof(samples) */
#define FRAME16K_SIZE				(320*2) /* nsamples * sizeof(samples) */
#define PROFILE_ID					0


struct sport_stream_config_profile
{
        int                             freq;
        int                             frame_size;
        struct sport_stream_config      config;
};

int sport_init(void* context);
void sport_deinit(void* context);

#endif /* _BFIN_SPORT_EX_H_ */
