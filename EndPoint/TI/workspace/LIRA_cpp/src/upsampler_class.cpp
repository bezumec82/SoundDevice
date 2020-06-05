#include "upsampler_class.hpp"

/*!
 * @param out_pckg_bufs     - list of buffers to hold processed audio data
 * @param in_que            - queue from USB - alaw 8kHz 8bit
 * @param out_que           - queue to MCASP - I2S 16kHz 16(32)bit
 * @param in_smpl_rate      - sample rate of producer
 * @param out_smpl_rate     - sample rate of consumer
 */
upsampler::upsampler    (
    std::vector < void * >& out_pckg_bufs,
    queue_type&  in_que,
    queue_type& out_que,
    double&      in_smpl_rate,
    double&     out_smpl_rate
    ) :
    out_pckg_bufs    (out_pckg_bufs),
    in_que           (in_que),
    out_que          (out_que),
    in_smpl_rate     (in_smpl_rate),
    out_smpl_rate    (out_smpl_rate)
{
    allocate_sinc_tbl_33x32();
    allocate_sinc_tbl_31x32();
    prepare_soxr();
}
/*----------------------------------------------------------------------------*/

upsampler::~upsampler()
{
    this->stop();
    out_pckg_bufs.clear();
    deallocate_sinc_tbl_33x32();
    deallocate_sinc_tbl_31x32();
    FREE_MEM(this->in_buf_soxr_connector);
    FREE_MEM(this->out_buf_soxr_connector);
    soxr_delete(this->soxr);
    PRINTF(RED, "Upsampler is destroyed.\n");
}
/*----------------------------------------------------------------------------*/

/*!
 * Memory facility should be external to this class
 * to prevent program disrupt in case of class destruction.
 */
void upsampler::  alloc_out_pckg_buf(std::vector < void * >& out_pckg_bufs)
{
    for(int i = 0; i < AUDIO_DATA_BUF_AMMNT; i++)
    {
        int32_t * out_buf_ptr = (int32_t * )malloc(PAGE_SIZE);
        TEST_MALLOC_RES_N_PANISH(out_buf_ptr);
        out_pckg_bufs.push_back(out_buf_ptr);
    }
}

void upsampler::dealloc_out_pckg_buf(std::vector < void * >& out_pckg_bufs)
{
    for(auto iter : out_pckg_bufs){ FREE_MEM(iter); }
}
/*----------------------------------------------------------------------------*/

void upsampler::start(void)
{
    stop_aproc_thread = false;
    aproc_thread = std::thread(&upsampler::upsampler_thread_func, this);
}

void upsampler::stop(void)
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

/* For 8 bit linear audio input it is possible to create sinc table
 * for the full amplitude range : -128 ... 127.
 * Presently such approach is unused. */
void upsampler::allocate_sinc_tbl_256x33x32(void)
{
    //Create sinc table. Access : ->width(256) -> raw(33) ->col(32)
    this->sinc_tbl_256x33x32 = (double ***)malloc(RANGE * sizeof(double));
    if  (!sinc_tbl_256x33x32) { FPRINTF; exit(-EXIT_FAILURE); }
    for (int amp_idx = 0; amp_idx < RANGE; amp_idx++)
    {
        double ** raw = (double **)malloc(USB_IN_BUF_SIZE_4ms_WORDS_INDCD * sizeof(double));
        if (!raw) { FPRINTF; exit(-EXIT_FAILURE); }
        for(int out_idx = 0; out_idx < USB_IN_BUF_SIZE_4ms_WORDS_INDCD; out_idx++)
        {
            double * col = (double *)malloc(USB_IN_BUF_SIZE_4ms_WORDS * sizeof(double));
            if (!col) { FPRINTF; exit(-EXIT_FAILURE); }
            for(int in_idx = 0; in_idx < USB_IN_BUF_SIZE_4ms_WORDS; in_idx++)
            {
                double sinc_arg =
                                  M_PI *
                                  (
                                    double((out_idx + 1)
                                  * double(USB_IN_BUF_SIZE_4ms_WORDS)
                                  / double(USB_IN_BUF_SIZE_4ms_WORDS_INDCD))
                                  - double(in_idx + 1)
                                  );
                double amp = double(amp_idx + IN_SIGNAL_MIN);

                if (sinc_arg==0) { col[in_idx] = amp; }
                else             { col[in_idx] = amp * double(sin(sinc_arg)/sinc_arg); }
            } //end for(int in_idx
            raw[out_idx] = col;
        } //end for(int out_idx
        this->sinc_tbl_256x33x32[amp_idx] = raw;
    } //end for (int amp_idx
}

void upsampler::allocate_sinc_tbl_256x31x32(void)
{
    //Create sinc table. Access : ->width(256) -> raw(31) ->col(32)
    this->sinc_tbl_256x31x32 = (double ***)malloc(RANGE * sizeof(double));
    if  (!sinc_tbl_256x31x32) { FPRINTF; exit(-EXIT_FAILURE); }
    for (int amp_idx = 0; amp_idx < RANGE; amp_idx++)
    {
        double ** raw = (double **)malloc(USB_IN_BUF_SIZE_4ms_WORDS_REDCD * sizeof(double));
        if (!raw) { FPRINTF; exit(-EXIT_FAILURE); }
        for(int out_idx = 0; out_idx < USB_IN_BUF_SIZE_4ms_WORDS_REDCD; out_idx++)
        {
            double * col = (double *)malloc(USB_IN_BUF_SIZE_4ms_WORDS * sizeof(double));
            if (!col) { FPRINTF; exit(-EXIT_FAILURE); }
            for(int in_idx = 0; in_idx < USB_IN_BUF_SIZE_4ms_WORDS; in_idx++)
            {
                double sinc_arg =
                                  M_PI *
                                  (
                                    double((out_idx+1)
                                  * double(USB_IN_BUF_SIZE_4ms_WORDS)
                                  / double(USB_IN_BUF_SIZE_4ms_WORDS_REDCD))
                                  - double(in_idx+1)
                                  );
                double amp = double(amp_idx + IN_SIGNAL_MIN);

                if (sinc_arg==0) { col[in_idx] = amp; }
                else             { col[in_idx] = amp * double(sin(sinc_arg)/sinc_arg); }
            } //end for(int in_idx
            raw[out_idx] = col;
        } //end for(int out_idx
        this->sinc_tbl_256x31x32[amp_idx] = raw;
    } //end for (int amp_idx
}
/*----------------------------------------------------------------------------*/

/* Tables of sinc coefficients to use for data stretching */
void upsampler::allocate_sinc_tbl_33x32(void)
{
    //Create sinc table. Access : raw(33) -> col(32)
    this->sinc_tbl_33x32 = (int32_t **)malloc(USB_IN_BUF_SIZE_4ms_WORDS_INDCD * sizeof(int32_t));
    TEST_MALLOC_RES_N_PANISH(sinc_tbl_33x32);
    for(int out_idx = 0; out_idx < USB_IN_BUF_SIZE_4ms_WORDS_INDCD; out_idx++)
    {
        int32_t * col = (int32_t *)malloc(USB_IN_BUF_SIZE_4ms_WORDS * sizeof(int32_t));
        TEST_MALLOC_RES_N_PANISH(col);
        for(int in_idx = 0; in_idx < USB_IN_BUF_SIZE_4ms_WORDS; in_idx++)
        {
            double sinc_arg =
                              M_PI *
                              (
                                double((out_idx+1)
                              * double(USB_IN_BUF_SIZE_4ms_WORDS)
                              / double(USB_IN_BUF_SIZE_4ms_WORDS_INDCD))
                              - double(in_idx+1)
                              );
            if (sinc_arg==0) { col[in_idx] = 1<<TABLE_POW; }
            else             { col[in_idx] = (int32_t)( round((sin(sinc_arg)/sinc_arg) * pow(2.0, TABLE_POW)) ); }
        } //end for(int in_idx
        sinc_tbl_33x32[out_idx] = col;
    } //end for(int out_idx
#if (SHOW_SINC_TABLE)
    printf("Sinc table 33x32: \n");
    for(int out_idx = 0; out_idx < IN_BUF_SIZE_4ms_WORDS_INDCD; out_idx++)
    {
        printf("raw %-2i :",out_idx);
        for(int in_idx = 0; in_idx < IN_BUF_SIZE_4ms_WORDS; in_idx++)
        {
             printf("%-6i ", sinc_tbl_33x32[out_idx][in_idx]);
        }
        printf("\n");
    }
#endif
}

/* Tables of sinc coefficients to use for data shrinking */
void upsampler::allocate_sinc_tbl_31x32(void)
{
    //Create sinc table. Access : raw(31) -> col(32)
    this->sinc_tbl_31x32 = (int32_t **)malloc(USB_IN_BUF_SIZE_4ms_WORDS_REDCD * sizeof(int32_t));
    TEST_MALLOC_RES_N_PANISH(sinc_tbl_31x32);
    for(int out_idx = 0; out_idx < USB_IN_BUF_SIZE_4ms_WORDS_REDCD; out_idx++)
    {
        int32_t * col = (int32_t *)malloc(USB_IN_BUF_SIZE_4ms_WORDS * sizeof(int32_t));
        TEST_MALLOC_RES_N_PANISH(col);
        for(int in_idx = 0; in_idx < USB_IN_BUF_SIZE_4ms_WORDS; in_idx++)
        {
            double sinc_arg =
                              M_PI *
                              (
                                double((out_idx+1)
                              * double(USB_IN_BUF_SIZE_4ms_WORDS)
                              / double(USB_IN_BUF_SIZE_4ms_WORDS_REDCD))
                              - double(in_idx+1)
                              );
            if (sinc_arg==0) { col[in_idx] = 1<<15; }
            else             { col[in_idx] = int( (sin(sinc_arg)/sinc_arg) * pow(2.0, 15) ); }
        } //end for(int in_idx
        this->sinc_tbl_31x32[out_idx] = col;
    } //end for(int out_idx
#if (SHOW_SINC_TABLE)
    printf("Sinc table 32x31: \n");
    for(int out_idx = 0; out_idx < IN_BUF_SIZE_4ms_WORDS_REDCD; out_idx++)
    {
        printf("raw %-2i :",out_idx);
        for(int in_idx = 0; in_idx < IN_BUF_SIZE_4ms_WORDS; in_idx++)
        {
             printf("%-6i ", sinc_tbl_31x32[out_idx][in_idx]);
        }
        printf("\n");
    }
#endif
}
/*----------------------------------------------------------------------------*/

void upsampler::deallocate_sinc_tbl_33x32(void)
{
    for(int out_idx = 0; out_idx < USB_IN_BUF_SIZE_4ms_WORDS_INDCD; out_idx++)
    {
        int32_t * col = sinc_tbl_33x32[out_idx];
        FREE_MEM(col);
    }
    FREE_MEM(sinc_tbl_33x32);
}

void upsampler::deallocate_sinc_tbl_31x32(void)
{
    for(int out_idx = 0; out_idx < USB_IN_BUF_SIZE_4ms_WORDS_REDCD; out_idx++)
    {
        int32_t * col = sinc_tbl_31x32[out_idx];
        FREE_MEM(col);
    }
    FREE_MEM(sinc_tbl_31x32);
}
/*----------------------------------------------------------------------------*/

soxr_error_t upsampler::prepare_soxr(void)
{
    soxr_error_t error;
    soxr_datatype_t itype        = (soxr_datatype_t)SOXR_INT16_I;
    soxr_datatype_t otype        = (soxr_datatype_t)SOXR_INT16_I;
    soxr_io_spec_t const io_spec = soxr_io_spec(itype, otype);
    unsigned long const q_recipe = SOXR_MQ;
    unsigned long const q_flags  = SOXR_ROLLOFF_MEDIUM;
    soxr_quality_spec_t q_spec   = soxr_quality_spec(q_recipe, q_flags);
    this->soxr                   = soxr_create(
                                              8000., 16000., 1,   /* Input rate, output rate, # of channels. */
                                              &error,             /* To report any error during creation. */
                                              &io_spec,
                                              &q_spec,
                                              NULL);
    this->in_buf_soxr_connector  = (int16_t *)malloc(USB_IN_BUF_SIZE_4ms_WORDS  * sizeof(int16_t));
    TEST_MALLOC_RES_N_PANISH(this->in_buf_soxr_connector);
    this->out_buf_soxr_connector = (int16_t *)malloc(MCASP_OUT_BUF_SIZE_4ms_WORDS * sizeof(int16_t));
    TEST_MALLOC_RES_N_PANISH(this->out_buf_soxr_connector);
    return error;
}
/*----------------------------------------------------------------------------*/

inline void upsampler::block_mechanism(void)
{
    while (!stop_aproc_thread)
    {
        if((in_cond_var_ptr != nullptr)&&(in_mtx_ptr != nullptr))
        {
            std::unique_lock<std::mutex> uni_lock(* in_mtx_ptr);
            if(in_cond_var_ptr->wait_for(
                                        uni_lock,
                                        std::chrono::milliseconds(4),
                                        [&]{ return  !in_que.empty(); }
                                        )!=true)
            {
                continue;
            }
            else
            {
                break;
            }

        }
        else /* Synchronization is not used */
        {
            if(in_que.empty()) /* Nothing to do */
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            else
            {
                break;
            } //end if(in_que.empty())
        }
    } //end while (!stop_aproc_thread)
}

void upsampler::upsampler_thread_func(void)
{
    size_t    in_buf_cntr_b      = 0;
    uint8_t * in_buf_ptr         = nullptr;
    size_t    in_buf_size        = 0;
    auto      out_pckg_buf_iter = out_pckg_bufs.begin();

    /* Buffers for sample rate correction. It is local to this method. */
#if (1)
    int16_t * soxr_in_buf_ptr    = NULL;
    int16_t * corrected_data_buf = (int16_t *)malloc(USB_IN_BUF_SIZE_4ms_WORDS_INDCD * sizeof(int16_t));
    int16_t * shift_buf          = (int16_t *)malloc(USB_IN_BUF_SIZE_4ms_WORDS       * sizeof(int16_t));
    int16_t * accumulator_buf    = (int16_t *)malloc(USB_IN_BUF_SIZE_4ms_WORDS       * sizeof(int16_t));
    int16_t * tmp_buf            = (int16_t *)malloc(USB_IN_BUF_SIZE_4ms_WORDS       * sizeof(int16_t));
    int16_t * linear_buf         = (int16_t *)malloc(USB_IN_BUF_SIZE_4ms_WORDS       * sizeof(int16_t));

    TEST_MALLOC_RES_N_PANISH(corrected_data_buf);
    TEST_MALLOC_RES_N_PANISH(shift_buf);
    TEST_MALLOC_RES_N_PANISH(accumulator_buf);
    TEST_MALLOC_RES_N_PANISH(tmp_buf);
    TEST_MALLOC_RES_N_PANISH(linear_buf);
#endif

    uint32_t acc_buf_cntr_w        = 0;
    double   data_rate_delta_smpls = 0.0;
    uint32_t corr_period_pckgs     = 0;
    uint32_t pckg_cntr             = 0;

    while (!stop_aproc_thread)
    {
        /* Synchronization/blocking mechanism */
        block_mechanism();

        /* Acquire data from FIFO */
    #if (1)
        if(in_mtx_ptr != nullptr)
        {
            std::lock_guard<std::mutex> lock(* in_mtx_ptr); /* Lock on queue */
            in_buf_ptr  = (uint8_t *)in_que.front().first;
            in_buf_size =            in_que.front().second;
        }
        else
        {
            in_buf_ptr  = (uint8_t *)in_que.front().first;
            in_buf_size =            in_que.front().second;
        }
        if ((!in_buf_ptr)||(!in_buf_size))
        {
            PRINT_ERR("Empty buffer.\n");
            goto pop_in_que;
        }
    #endif

        /* Re-pack from incoming 20ms 8000x8 to 4ms 8000x8
         * convert from 8 bit alaw to linear
         * correct samplerate
         * resample to 16000x16
         * put to output queue */
        in_buf_cntr_b = 0;
        while ( (in_buf_cntr_b < in_buf_size)
              &&(in_buf_cntr_b < PAGE_SIZE) ) /* Each buf at the provider side should have such size */
        {
            /* Operations with accumulator buffer */
        #if (1)
            if ( (acc_buf_cntr_w == USB_IN_BUF_SIZE_4ms_WORDS)  /* If accumulator is full */
              && (data_rate_delta_smpls < 0.0) )            /* And delta is negative */
            {
                soxr_in_buf_ptr = &accumulator_buf[0];
                acc_buf_cntr_w = 0;
                goto resample; /* Send it to resample, instead of data in order */
            }
            if ( (acc_buf_cntr_w == 0)                      /* If accumulator is empty */
              && (data_rate_delta_smpls > 0.0) )            /* And delta is positive */
            {
                /* de-alaw one package in accumulator buffer */
                for(; acc_buf_cntr_w < USB_IN_BUF_SIZE_4ms_WORDS; acc_buf_cntr_w++, in_buf_cntr_b++)
                {
                    accumulator_buf[acc_buf_cntr_w] = \
                            G711EncoderDecoder::alaw2linear(in_buf_ptr[in_buf_cntr_b]);
                }
                continue; /* Go for the next package*/
            }
        #endif

            /* Decode alaw 8bit to linear 16bit */
        #if (1)
            for(int i = 0; i < USB_IN_BUF_SIZE_4ms_WORDS; i++)
            {
                linear_buf[i] = G711EncoderDecoder::alaw2linear(in_buf_ptr[in_buf_cntr_b + i]);
            }
        #endif

            /* Take next data from the input queue */
            in_buf_cntr_b += USB_IN_BUF_SIZE_4ms_BYTES;

            /* Gather/refresh data about sample rates inconsistency */
        #if (1)
            if (data_rate_delta_smpls == 0.0)
            {
                if ((in_smpl_rate  != 0.0)&&(out_smpl_rate != 0.0)) /* Wait for data */
                {
                    data_rate_delta_smpls = in_smpl_rate - out_smpl_rate/2.0;
                    PRINT_COND(SHOW_CORRECTION, CYN, "Delta %f.\n", data_rate_delta_smpls);
                    if(data_rate_delta_smpls < double(-CORRECTION_THREASHOLD))
                    {
                        data_rate_delta_smpls = double(-CORRECTION_THREASHOLD); /* Correct each package */
                    }
                    if(data_rate_delta_smpls > double(CORRECTION_THREASHOLD))
                    {
                        data_rate_delta_smpls = double(CORRECTION_THREASHOLD); /* Correct each package */
                    }
                    if (data_rate_delta_smpls!=0.0)
                    {
                        corr_period_pckgs = uint32_t(double(CORRECTION_PER_PCKG * PCKGS_PER_SEC)
                                                   /(std::abs(data_rate_delta_smpls)));
                        if(!corr_period_pckgs)                { corr_period_pckgs = 1; } /* Correct each package */
                        if(corr_period_pckgs > PCKGS_PER_SEC) { corr_period_pckgs = PCKGS_PER_SEC; }
                    }
                    else
                    {
                        corr_period_pckgs = 0;
                    }
                    pckg_cntr = corr_period_pckgs;
                    PRINT_COND(SHOW_CORRECTION, MAG, "Corr. period %i pckgs. Delta %f.\n",
                            corr_period_pckgs, data_rate_delta_smpls);
                }
                else
                {
                    data_rate_delta_smpls = 0.0;
                }
            }
        #endif

            /* Fill shift buffer */
        #if (1)
            if ( (data_rate_delta_smpls == 0.0)   /* Suppose sample rates are perfectly consistent */
               &&(acc_buf_cntr_w        == 0) ) /* No corrections were previously made */
            {
                soxr_in_buf_ptr = linear_buf;
            }
            else /* In any other situation fill shift buffer */
            {
                /* Fill the first part of shift buffer from the accumulator */
                memcpy( &shift_buf      [0],
                        &accumulator_buf[0],
                        acc_buf_cntr_w * sizeof(int16_t));
                /* Fill the second part of shift buffer from the incoming data */
                memcpy( &shift_buf [             acc_buf_cntr_w],
                        &linear_buf[0],
                        (USB_IN_BUF_SIZE_4ms_WORDS - acc_buf_cntr_w) * sizeof(int16_t));
                /* Refill accumulator by the last data */
                memcpy( &accumulator_buf[0],
                        &linear_buf     [USB_IN_BUF_SIZE_4ms_WORDS - acc_buf_cntr_w],
                                                                 acc_buf_cntr_w * sizeof(int16_t));
                /* If correction is not necessary, but accumulator is not empty */
                if (data_rate_delta_smpls == 0.0) { soxr_in_buf_ptr = &shift_buf[0]; }
            }
        #endif

            /* Input is less than necessary */
        #if (1)
            if (data_rate_delta_smpls < 0.0)
            {
                if (!pckg_cntr) /* It is time to make correction */
                {
                    pckg_cntr = corr_period_pckgs;
                    data_rate_delta_smpls += 1.0; /* Data refresh */
                    if (data_rate_delta_smpls > 0.0){ data_rate_delta_smpls = 0.0; }
                    if (out_que.size() > MAX_OUT_QUEUE_SIZE) /* Too many data was generated */
                    {
                        PRINT_COND(SHOW_CORRECTION, NORM, "Stop generating.\n");
                        data_rate_delta_smpls = 0.0; /* Accelerate data refresh */
                        soxr_in_buf_ptr = &shift_buf[0];
                    }
                    else /* Correction operation */
                    {
                        if (out_que.empty()) /* Not enough data */
                            if(corr_period_pckgs > 0) { corr_period_pckgs--; } /* Decrease correction period */
                        data2sinc_rebuilder (
                                            &shift_buf[0],      USB_IN_BUF_SIZE_4ms_WORDS,
                                            corrected_data_buf, USB_IN_BUF_SIZE_4ms_WORDS_INDCD /* Ask for more */
                                            );
                        /* Accumulate excessive data */
                        memcpy  ( /* Save accumulator */
                                &tmp_buf[0],
                                &accumulator_buf[0],
                                acc_buf_cntr_w * sizeof(int16_t)
                                );
                        memcpy  ( /* Place new data TO accumulator */
                                &accumulator_buf[0],
                                &corrected_data_buf[USB_IN_BUF_SIZE_4ms_WORDS_INDCD - CORRECTION_PER_PCKG],
                                CORRECTION_PER_PCKG * sizeof(int16_t)
                                );
                        memcpy  ( /* Restore accumulator */
                                &accumulator_buf[CORRECTION_PER_PCKG],
                                &tmp_buf[0],
                                acc_buf_cntr_w * sizeof(int16_t)
                                );
                        acc_buf_cntr_w += CORRECTION_PER_PCKG;
                        soxr_in_buf_ptr = &corrected_data_buf[0];
                    }
                }
                else /* No correction was made - just shift */
                {
                    soxr_in_buf_ptr = &shift_buf[0];
                    pckg_cntr--;
                } //end if (!pckg_cntr)
            } //end if (data_rate_delta_smpls < 0)
        #endif

            /* Input is more than necessary */
        #if (1)
            if (data_rate_delta_smpls > 0.0)
            {   if (!pckg_cntr) /* It is time to make correction */
                {
                    data_rate_delta_smpls -= 1.0;
                    if (data_rate_delta_smpls < 0.0){ data_rate_delta_smpls = 0.0; }
                    pckg_cntr = corr_period_pckgs;
                    if (out_que.empty()) /* Too many data was shrunk */
                    {
                        PRINT_COND(SHOW_CORRECTION, BLU, "Stop shrinking.\n");
                        data_rate_delta_smpls = 0.0; /* Accelerate data refresh */
                        soxr_in_buf_ptr = &shift_buf[0];
                    }
                    else /* Correction operation */
                    {
                        if(out_que.size() > MAX_OUT_QUEUE_SIZE) /* Increase shrinking */
                            if(corr_period_pckgs > 0) { corr_period_pckgs--; } /* Decrease correction period */
                        data2sinc_rebuilder (
                                            &shift_buf[0],      USB_IN_BUF_SIZE_4ms_WORDS,
                                            corrected_data_buf, USB_IN_BUF_SIZE_4ms_WORDS_REDCD /* Ask for less*/
                                            );
                        /* Add data from accumulator */
                        memcpy  ( /* Save accumulator buffer */
                                &tmp_buf[0],
                                &accumulator_buf[0],
                                acc_buf_cntr_w * sizeof(int16_t)
                                );
                        memcpy  ( /* Take data from accumulator */
                                &corrected_data_buf[USB_IN_BUF_SIZE_4ms_WORDS_REDCD],
                                &accumulator_buf[0],
                                CORRECTION_PER_PCKG * sizeof(int16_t)
                                );
                        acc_buf_cntr_w -= CORRECTION_PER_PCKG;
                        memcpy  ( /* Restore accumulator */
                                &accumulator_buf[0],
                                &tmp_buf[CORRECTION_PER_PCKG], /* Skip consumed part */
                                acc_buf_cntr_w * sizeof(int16_t)
                                );
                        soxr_in_buf_ptr   = &corrected_data_buf[0];
                    }
                }
                else /* No correction was made - just shift */
                {
                    soxr_in_buf_ptr = &shift_buf[0];
                    pckg_cntr--;
                } //end if (!pckg_cntr)
            } //end if (data_rate_delta_smpls > 0.0)
        #endif
resample:
            soxr_wrapper(soxr_in_buf_ptr, (int32_t *)* out_pckg_buf_iter); /* 32w -> 64w */

            /* Add buffer to out queue */
        #if (1)
            if (out_mtx_ptr != nullptr)
            {
                std::lock_guard<std::mutex> lock(* out_mtx_ptr); /* Lock on queue */
                out_que.emplace( (void *) * out_pckg_buf_iter, MCASP_OUT_BUF_SIZE_4ms_BYTES );
            }
            else
            {
                out_que.emplace( (void *) * out_pckg_buf_iter, MCASP_OUT_BUF_SIZE_4ms_BYTES );
            }
            if(out_cond_var_ptr != nullptr) { out_cond_var_ptr->notify_one(); }
        #endif
            /* Set next buffer further */
        #if (1)
            out_pckg_buf_iter++;
            if (out_pckg_buf_iter == out_pckg_bufs.end())
            {
                out_pckg_buf_iter = out_pckg_bufs.begin();
            }
        #endif
        } //end while ((in_buf_cntr_b ...

pop_in_que:
        if (in_mtx_ptr != nullptr)
        {
            std::lock_guard<std::mutex> lock(* in_mtx_ptr);
            in_que.pop();
        }
        else
        {
            in_que.pop();
        }
    } //end while (!stop_aproc_thread)
    PRINTF(RED, "Audio processing thread is stopped.\n");
    FREE_MEM(corrected_data_buf);
    FREE_MEM(shift_buf);
    FREE_MEM(accumulator_buf);
    FREE_MEM(tmp_buf);
    FREE_MEM(linear_buf);
}
/*----------------------------------------------------------------------------*/

/*!
 * @param in_buf_ptr    - 4ms data 8bit 8kHz linear from USB.
 * @param out_buf_ptr   - 4ms data 32bit(16 used) 16kHz linear for MCASP.
 */
soxr_error_t upsampler:: soxr_wrapper (int8_t * in_buf_ptr, int32_t * out_buf_ptr)
{
    if((!in_buf_ptr)||(!out_buf_ptr))
    {
        PRINT_ERR("NULL passed as the parameter.\n");
        return "error";
    }
    /* Convert 8 bit input to 16 bit data. */
    for (int i = 0; i < USB_IN_BUF_SIZE_4ms_WORDS; i++)
    {
        in_buf_soxr_connector[i] = (in_buf_ptr[i] << 8);
    }
    /* Resample. */
    soxr_error_t error = soxr_process(
            soxr, /* As returned by soxr_create. */
            /* Input (to be resampled): */
            in_buf_soxr_connector,
            USB_IN_BUF_SIZE_4ms_WORDS,
            NULL, /* To return actual # samples used. */
            /* Output (resampled): */
            out_buf_soxr_connector,
            MCASP_OUT_BUF_SIZE_4ms_WORDS,
            NULL);
    /* Re-pack 16bit output to 32bit array. */
    for (int i = 0; i < MCASP_OUT_BUF_SIZE_4ms_WORDS; i++)
    {
        out_buf_ptr[i] = out_buf_soxr_connector[i];
    }
    return error;
}
/*----------------------------------------------------------------------------*/

/*!
 * @param in_buf_ptr    - 4ms 16bit 8000Hz input from USB->alaw (32w).
 * @param out_buf_ptr   - 4ms 32(16used)bit 16000Hz output for MCASP (64w).
 */
soxr_error_t upsampler:: soxr_wrapper (int16_t * in_buf_ptr, int32_t * out_buf_ptr)
{
    if((!in_buf_ptr)||(!out_buf_ptr))
    {
        PRINT_ERR("NULL passed as the parameter.\n");
        return "error";
    }
    /* Resample. */
    soxr_error_t error = soxr_process(
            soxr, /* As returned by soxr_create. */
            /* Input (to be resampled): */
            in_buf_ptr,
            USB_IN_BUF_SIZE_4ms_WORDS,
            NULL, /* To return actual # samples used. */
            /* Output (resampled): */
            out_buf_soxr_connector,
            MCASP_OUT_BUF_SIZE_4ms_WORDS,
            NULL);
    /* Re-pack 16bit output to 32bit array */
    for (int i = 0; i < MCASP_OUT_BUF_SIZE_4ms_WORDS; i++)
    {
        out_buf_ptr[i] = out_buf_soxr_connector[i];
    }
    return error;
}
/*----------------------------------------------------------------------------*/

void upsampler::data2sinc_rebuilder   (
                                            int8_t * in_buf,  size_t in_buf_size,
                                            int8_t * out_buf, size_t out_buf_size
                                            )
{
#if SINC_PROCESSING
    double *** sinc_array = NULL;
    if      (in_buf_size > out_buf_size) { sinc_array = this->sinc_tbl_256x31x32; }
    else if (in_buf_size < out_buf_size) { sinc_array = this->sinc_tbl_256x33x32; }
    else { PRINT_ERR("Nothing to do.\n"); return; }
    for (int out_idx = 0; out_idx < int(out_buf_size); out_idx++)
    {
        double sum = 0.0;
        for(int in_idx = 0; in_idx < int(in_buf_size); in_idx++)
        {
             int amp_idx = in_buf[in_idx]-IN_SIGNAL_MIN;
             if ((amp_idx < 0)||(amp_idx >= RANGE)) { PRINT_ERR("Input value %d out of range.\n", in_buf[in_idx]); }
             sum += sinc_array[amp_idx][out_idx][in_idx];
        }
        out_buf[out_idx] = (int8_t)round(sum);
    }
#else
    if      (in_buf_size > out_buf_size)
    {
        memcpy (&out_buf[0], &in_buf[0], out_buf_size);
    }
    else if (in_buf_size < out_buf_size)
    {
        memcpy (&out_buf[0], &in_buf[0], in_buf_size);
        out_buf[out_buf_size-1] = in_buf[in_buf_size-1];
    }
#endif
#if (SHOW_CORRECTION_SEVERE)
    printf("in_signal  = [");
    for(int in_idx = 0; in_idx < int(in_buf_size); in_idx++)
    {
        if (in_idx == int(in_buf_size-1))
            printf("%-3i];", in_buf[in_idx]);
        else
            printf("%-3i,", in_buf[in_idx]);

    }
    printf("\nout_result = [");
    for (int out_idx = 0; out_idx < int(out_buf_size); out_idx++)
    {
        if (out_idx == int(out_buf_size-1))
            printf("%-3i];", out_buf[out_idx]);
        else
            printf("%-3i,", out_buf[out_idx]);
    }
    printf("\n\n");
#endif
}

void upsampler::data2sinc_rebuilder   (
                                            int16_t * in_buf,  size_t in_buf_size,
                                            int16_t * out_buf, size_t out_buf_size
                                            )
{
    int32_t ** sinc_array = NULL;
    if      (in_buf_size > out_buf_size) { sinc_array = this->sinc_tbl_31x32; }
    else if (in_buf_size < out_buf_size) { sinc_array = this->sinc_tbl_33x32; }
    else { PRINT_ERR("Nothing to do.\n"); return; }
    for (int out_idx = 0; out_idx < int(out_buf_size); out_idx++)
    {
        out_buf[out_idx] = 0; /* In case of this buf is reusable */
        for(int in_idx = 0; in_idx < int(in_buf_size); in_idx++)
        {
            out_buf[out_idx] += (sinc_array[out_idx][in_idx] * in_buf[in_idx])>>TABLE_POW;
        }
    }
#if (SHOW_CORRECTION_SEVERE)
    printf("in_signal  = [");
    for(int in_idx = 0; in_idx < int(in_buf_size); in_idx++)
    {
        if (in_idx == int(in_buf_size-1))
            printf("%-5i];", in_buf[in_idx]);
        else
            printf("%-5i,", in_buf[in_idx]);
    }
    printf("\nout_result = [");
    for (int out_idx = 0; out_idx < int(out_buf_size); out_idx++)
    {
        if (out_idx == int(out_buf_size-1))
            printf("%-5i];", out_buf[out_idx]);
        else
            printf("%-5i,", out_buf[out_idx]);
    }
    printf("\n\n");
#endif
}
/*----------------------------------------------------------------------------*/
