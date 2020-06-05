#include "downsampler_class.hpp"

/*!
 * @param out_pckg_bufs - list of buffers to hold processed audio data
 * @param in_que            - queue of input packages from MCASP - I2S 16kHz 16(32)bit
 * @param out_que           - queue of downsampled data to USB - alaw 8kHz 8bit
 */
downsampler::downsampler    ( downsampler_data& ddata_ref ) :
    ddata_ref(ddata_ref)
{
    prepare_soxr();
}
/*----------------------------------------------------------------------------*/

downsampler::~downsampler()
{
    this->stop();
    FREE_MEM(this->in_buf_soxr_connector);
    FREE_MEM(this->out_buf_soxr_connector);
    soxr_delete(this->soxr);
    PRINTF(RED, "Downsampler is destroyed.\n");
}
/*----------------------------------------------------------------------------*/

soxr_error_t downsampler::prepare_soxr(void)
{
    soxr_error_t error;
    soxr_datatype_t itype        = (soxr_datatype_t)SOXR_INT16_I;
    soxr_datatype_t otype        = (soxr_datatype_t)SOXR_INT16_I;
    soxr_io_spec_t const io_spec = soxr_io_spec(itype, otype);
    unsigned long const q_recipe = SOXR_MQ;
    unsigned long const q_flags  = SOXR_ROLLOFF_MEDIUM;
    soxr_quality_spec_t q_spec   = soxr_quality_spec(q_recipe, q_flags);
    this->soxr                   = soxr_create(
                                              ddata_ref.input_rate,  /* Input sample-rate. */
                                              ddata_ref.output_rate, /* Output sample-rate. */
                                              1,                     /* Number of channels to be used. */
                                              &error,                /* To report any error during creation. */
                                              &io_spec,
                                              &q_spec,
                                              NULL);
    in_buf_soxr_connector  = (int16_t *)malloc(MCASP_IN_BUF_SIZE_4ms_WORDS * sizeof(int16_t));
    TEST_MALLOC_RES_N_PANISH(in_buf_soxr_connector);
    out_buf_soxr_connector = (int16_t *)malloc(USB_OUT_BUF_SIZE_4ms_WORDS * sizeof(int16_t));
    TEST_MALLOC_RES_N_PANISH(out_buf_soxr_connector);
    return error;
}
/*----------------------------------------------------------------------------*/

void downsampler::start(void)
{
    if (*ddata_ref.out_pckg_bufs.begin() == nullptr)
    {
        PRINT_ERR("Can't start downsampler thread."
                  "Out buffers not allocated.\n");
        return;
    }
    if(
       (ddata_ref.in_cond_var_ptr  == nullptr)
     ||(ddata_ref.in_mtx_ptr       == nullptr)
     ||(ddata_ref.out_cond_var_ptr == nullptr)
     ||(ddata_ref.out_mtx_ptr      == nullptr)
     ) {
        PRINT_ERR("Can't start downsampler thread."
                  "No synchronization mechanisms are provided.\n");
        return;
    }
    stop_aproc_thread = false;
    aproc_thread = std::thread(&downsampler::downsampler_thread_func, this);
}

void downsampler::stop(void)
{
    if (!stop_aproc_thread)
    {
        PRINTF(RED, "Stopping process thread.\n");
        stop_aproc_thread = true;
        aproc_thread.join();
    }
    else PRINT_ERR("Already stopped.\n");
}
/*----------------------------------------------------------------------------*/

inline void downsampler::block_mechanism(void)
{
    while (!stop_aproc_thread)
    {
        std::unique_lock<std::mutex> uni_lock(* ddata_ref.in_mtx_ptr);
        if(ddata_ref.in_cond_var_ptr->wait_for(
                                    uni_lock,
                                    std::chrono::milliseconds(4),
                                    [&]{ return !ddata_ref.in_que.empty(); }
                                    )!=true)
        { continue; }
        else { break; }
    } //end while (!stop_aproc_thread)
}

/*!
 * This function not just calculates CRC
 * but also settles its value to the last two bytes
 * @param data             - pointer to the data
 * @param data_byte_length - data bytes length exclude CRC bytes
 */
uint16_t downsampler::calc_and_settle_CRC(uint8_t * data, size_t data_byte_length)
{
    uint16_t crc_calc            = 0xAAAA;
    uint16_t package_word_length = 0;
    uint16_t package_word        = 0; // Container for assembled words
    uint16_t data_byte_counter   = 0; // Counter for rendered data bytes
    uint8_t odd_flag = 0;

    if (!data) {
        PRINT_ERR("'NULL' passed as the parameter.\r\n");
        return 0;
    }
    /* Calculate data words in package */
    package_word_length = data_byte_length / 2;
    if (data_byte_length % 2 == 1) { /* If data quantity in package is odd */
        package_word_length++;
        odd_flag = 1;
    }
    /* Calculate CRC */
    for (uint16_t counter = 0; counter < package_word_length; counter++) {
        package_word = ((uint16_t) (*data)); // Settle lower bits of 'package_word'
        data++;
        data_byte_counter++;

        if (data_byte_counter < data_byte_length) { // If data count is odd do not settle last byte in 'package_word'
            package_word = package_word | (((uint16_t) (*data)) << (sizeof(uint8_t) << 3)); // Settle upper bits of 'package_word'
            data++;
            data_byte_counter++;
        }
        crc_calc = crc_calc ^ package_word;
        crc_calc = ((crc_calc << 1) & 0xFFFE) | ((crc_calc >> 15) & 0x0001);
    } //end for
    /* Add zero byte at the end if data count is odd */
    if (odd_flag) {
        *data = 0x00;
        data++;
    }
    /* Settle calculated CRC in data stream */
    /* Protocol LITTLE-endian -> LSB first */
    *data = (uint8_t) (crc_calc & 0x00ff); // Fill low bits of 'crc_calc'
    data++;
    *data = (uint8_t) ((crc_calc & 0xff00) >> 8); // Fill high bits of 'crc_calc'

#if DEBUG_LI_GW
    PRINT(GRN, "Calculated CRC: %x.\r\n", crc_calc);
#endif
    return crc_calc;
}
/*----------------------------------------------------------------------------*/

#define LI_GW_HDR_LNGTH                                 14
#define LI_GW_CRC_LNGTH                                 2
#define LI_GW_SYNC                                      0xE4E5AB2D
#define LI_GW_MSG_ID_ALAW_DATA                          0x2000
#define LI_GW_FLAG_PROTO_VER                            (0b01<<14)
#define LI_GW_FLAG_SET_STREAM_IDX(mcasp_idx, ts_idx)    (mcasp_idx<<13)|(ts_idx<<12)
#define LI_GW_FLAG_DEV_NUM(dev_num)                     ((dev_num&0xf)<<8)

/*!
 * @param pckg_buf          - pointer to the start of the buffer
 * !!!audiodata should be placed in offset of 'LI_GW_HDR_LNGTH'
 * @param adata_len         - size of audiodata
 * @param mcasp_idx         - index of MCASP controller
 * @param ts_idx            - index of the timeslot (for I2S 0 or 1)
 */
static uint32_t send_audio_pckg_cntr = 0;
void downsampler::LI_GW_pckgr   (
                                uint8_t * pckg_buf,
                                size_t    adata_len
                                )
{
     struct LI_GW_header header =
     {
            .msgStart  = (uint32_t) LI_GW_SYNC,             /* bytes 0..3  */
            .msgLength = (uint16_t)( (adata_len >> 1)
                                   + (adata_len & 0b1) ),   /* bytes 4-5   */
            .msgSeqNum = send_audio_pckg_cntr++,            /* bytes 6..9  */
            .msgID     = (uint16_t) LI_GW_MSG_ID_ALAW_DATA, /* bytes 10-11 */
            .msgFlags  = (uint16_t)(LI_GW_FLAG_PROTO_VER    /* bytes 12-13 */
                       | LI_GW_FLAG_SET_STREAM_IDX(ddata_ref.mcasp_idx, ddata_ref.ts_idx)
                       | LI_GW_FLAG_DEV_NUM(ddata_ref.dev_num)
                       | ddata_ref.dev_num ) /* device type code */    };
    memcpy(pckg_buf, &header, sizeof(struct LI_GW_header));
    calc_and_settle_CRC(pckg_buf, sizeof(struct LI_GW_header) + adata_len);
}
/*----------------------------------------------------------------------------*/

#define FTDI_HDR                                        uint16_t(0x6031)
#define FTDI_HDR_LNGTH                                  sizeof(uint16_t)
#define FTDI_CHUNK_SIZE                                 (64 - sizeof(uint16_t))
/*!
 * This function wraps input data in FTDI header.
 * It is not allocates any memory - it packs data further in the same buffer.
 * @param pckg_buf  - start of data to pack
 * @param pckg_len  - input/output parameter
 * @return          - pointer to the FTDI package
 */
uint8_t * downsampler::FTDI_packager(uint8_t * pckg_buf, size_t& pckg_len_ref)
{
    uint8_t * out_buf = pckg_buf + PAGE_SIZE/2; /* Each buf is allocated as PAGE_SIZE */
    uint32_t out_buf_cntr = 0;
    uint32_t i = 0;
    for (i = 0; i <= (pckg_len_ref/FTDI_CHUNK_SIZE); i++)
    {
        *((uint16_t *)&out_buf[out_buf_cntr]) = FTDI_HDR;
        out_buf_cntr += FTDI_HDR_LNGTH;
        memcpy(&out_buf[out_buf_cntr], pckg_buf, FTDI_CHUNK_SIZE);
        pckg_buf += FTDI_CHUNK_SIZE;
        out_buf_cntr += FTDI_CHUNK_SIZE;
        if(out_buf_cntr > PAGE_SIZE/2)
        {
            PRINT_ERR("Too many data : %d", out_buf_cntr);
            return pckg_buf;
        }
    }//end for
    pckg_len_ref = pckg_len_ref + i * FTDI_HDR_LNGTH;
    return out_buf;
}
/*----------------------------------------------------------------------------*/

void downsampler::downsampler_thread_func(void)
{
    int32_t * in_buf_ptr        = nullptr;
    size_t    out_buf_cntr      = LI_GW_HDR_LNGTH;
    size_t    adata_cntr        = 0;
    auto      out_pckg_buf_iter = ddata_ref.out_pckg_bufs.begin();
    int16_t * linear_buf_ptr = (int16_t *)malloc(USB_OUT_BUF_SIZE_4ms_WORDS * sizeof(int16_t));
    while (!stop_aproc_thread)
    {
        /* Synchronization/blocking mechanism */
        block_mechanism();

        /* Acquire data from FIFO */
        {
            std::lock_guard<std::mutex> lock(* ddata_ref.in_mtx_ptr); /* Lock on queue */
            in_buf_ptr  = (int32_t *)ddata_ref.in_que.front().first;
        }

        /* Downsample data */
        soxr_wrapper (in_buf_ptr, linear_buf_ptr); /* 64w -> 32w */

        /* Code linear 16bit to 8 bit alaw */
    #if (1)
        for(int i = 0; i < USB_OUT_BUF_SIZE_4ms_WORDS; i++, out_buf_cntr++)
        {
            ((uint8_t *)* out_pckg_buf_iter)[out_buf_cntr] =
                    G711EncoderDecoder::linear2alaw(linear_buf_ptr[i]);
            adata_cntr++;
        }
    #endif
        /* 20ms of audiodata is collected - time to wake-up consumer */
    #if (1)
        if (adata_cntr >= USB_OUT_BUF_SIZE_20ms_BYTES)
        {
            LI_GW_pckgr (
                        (uint8_t *)* out_pckg_buf_iter,
                        adata_cntr /* Amount of audiodata */
                        );
            adata_cntr = 0;
            out_buf_cntr += LI_GW_CRC_LNGTH; /* Take CRC into account */
            uint8_t * out_buf_ptr = FTDI_packager((uint8_t *)* out_pckg_buf_iter, out_buf_cntr);

            /* Add buffer to out queue */
            {
                std::lock_guard<std::mutex> lock(* ddata_ref.out_mtx_ptr); /* Lock on queue */
                ddata_ref.out_que.emplace( (void *)out_buf_ptr, out_buf_cntr );
                if (ddata_ref.out_que.size() > MAX_TO_USB_QUEUE_SIZE)
                {
                    #if (SHOW_TO_USB_QUE_SHRINK)
                        printf(RED "mcasp%d ts%d : No reader. Queue size : %d.\n" NORM,
                        ddata_ref.mcasp_idx, ddata_ref.ts_idx, ddata_ref.out_que.size());
                    #endif
                    while (ddata_ref.out_que.size() > MAX_TO_USB_QUEUE_SIZE)
                        ddata_ref.out_que.pop();
                }
            }
            #if (0)
                printf("mcasp_idx : %d ts_idx : %d\n",
                        ddata_ref.mcasp_idx, ddata_ref.ts_idx);
            #endif
            out_buf_cntr = LI_GW_HDR_LNGTH; /* Reset pckg data counter */
            /* Set next buffer further */
            out_pckg_buf_iter++;
            if (out_pckg_buf_iter == ddata_ref.out_pckg_bufs.end())
            {
                out_pckg_buf_iter = ddata_ref.out_pckg_bufs.begin();
            }
            ddata_ref.out_cond_var_ptr->notify_one();
        } //end if (out_buf_cntr ...
    #endif
        {
            std::lock_guard<std::mutex> lock(* ddata_ref.in_mtx_ptr);
            ddata_ref.in_que.pop();
        }
    } //end while
}
/*----------------------------------------------------------------------------*/

/*!
 * @param in_buf_ptr    - 4ms data 32(16used)bit 16kHz from MCASP (64w).
 * @param out_buf_ptr   - 4ms data 16bit 8kHz for alaw->USB (32w).
 */
soxr_error_t downsampler:: soxr_wrapper (int32_t * in_buf_ptr, int16_t * out_buf_ptr)
{
    if((!in_buf_ptr)||(!out_buf_ptr))
    {
        PRINT_ERR("NULL passed as the parameter.\n");
        return "error";
    }
    /* Convert 32(16used)bit array to 16 bit soxr input */
    for (int i = 0; i < int(MCASP_IN_BUF_SIZE_4ms_WORDS); i++)
    {
        in_buf_soxr_connector[i] = in_buf_ptr[i];
    }
    /* Resample. */
    soxr_error_t error = soxr_process(
            soxr, /* As returned by soxr_create. */
            /* Input (to be resampled): */
            in_buf_soxr_connector,
            MCASP_IN_BUF_SIZE_4ms_WORDS,
            NULL, /* To return actual # samples used. */
            /* Output (resampled): */
            out_buf_ptr,
            USB_OUT_BUF_SIZE_4ms_WORDS,
            NULL);
    return error;
}
/*----------------------------------------------------------------------------*/

/*!
 * Memory facility should be external to this class
 * to prevent program disrupt in case of class destruction.
 */
void downsampler::  alloc_out_pckg_buf(std::vector < void * >& out_pckg_bufs)
{
    for(int i = 0; i < AUDIO_DATA_BUF_AMMNT; i++)
    {
        int32_t * out_buf_ptr = (int32_t * )malloc(PAGE_SIZE);
        TEST_MALLOC_RES_N_PANISH(out_buf_ptr);
        out_pckg_bufs.push_back(out_buf_ptr);
    }
}

void downsampler::dealloc_out_pckg_buf(std::vector < void * >& out_pckg_bufs)
{
    for(auto iter : out_pckg_bufs) { FREE_MEM(iter); }
    out_pckg_bufs.clear();
}
/*----------------------------------------------------------------------------*/
