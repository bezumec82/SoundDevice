#ifndef __MCASP_H__
#define __MCASP_H__

#include "main.h"

#define BUF_QUANTUM             5
#define BUF_SIZE_4ms_WORDS          64
#define BUF_SIZE_4ms_BYTES          (uint32_t)(BUF_SIZE_4ms_WORDS * (sizeof (int32_t)))


#define SEND_BUF_SIZE_WORDS     (uint32_t)(BUF_SIZE_4ms_WORDS * BUF_QUANTUM)

#define SEND_BUF_SIZE_BYTES     (uint32_t)( SEND_BUF_SIZE_WORDS * (sizeof (int32_t)) )
#define RECV_BUF_SIZE_BYTES     SEND_BUF_SIZE_BYTES

#define SIN_FREQ                1000
#define FS_FREQ                 16000
#define SIN_PERIOD              (double)(FS_FREQ / SIN_FREQ)

#define SINE_TEST               false

#define PRU0                    0
#define PRU1                    1
#define MCASP0                  0
#define MCASP1                  1
#define MCASP8                  8
#define TS0                     1
#define TS1                     0

class mcasp_ts {
private:
    std::ostringstream file_name;
public:
    int fd;
    int32_t *   send_buf;
    uint32_t    send_buf_byte_size;

    int32_t *   recv_buf;
    uint32_t    recv_buf_byte_size;

    mcasp_ts( uint8_t , uint8_t , uint8_t );
    void write_sin( void );
    void read_mcasp   (int32_t * , uint32_t );
    void write_mcasp  (int32_t * , uint32_t );
    ~mcasp_ts( void );
};

#endif
