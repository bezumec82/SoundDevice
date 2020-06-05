#include "main.h"

/* Global variables */
static bool keep_running = true; /* Program killer flag */
/*----------------------------------------------------------------------------*/

void signal_hndlr(int signal) {
    switch (signal)
    {
        case SIGINT:
        {
            PRINTF(YEL, "SIGINT - external interrupt.\n");
            break;
        }
        case SIGTERM:
        {
            PRINTF(YEL, "SIGTERM - termination request.\n");
            break;
        }
        case SIGQUIT:
        {
            PRINTF(YEL, "SIGQUIT - quit request.\n");
            break;
        }
        case SIGTSTP:
        {
            PRINTF(YEL, "SIGTSTP - stop request.\n");
            break;
        }
        default:
        {
            PRINT_ERR("Unknown signal.\n");
            break;
        }
    }
    keep_running = false;
}

#define FULL_PACK   true
/* Data chain :
 * USB -> DSP -> MCASP */
int main()
{
    std::vector < void * > mcasp100_recv_pckg_buf_vec;
    std::vector < void * > mcasp101_recv_pckg_buf_vec;
    std::vector < void * > mcasp110_recv_pckg_buf_vec;
    std::vector < void * > mcasp111_recv_pckg_buf_vec;

    std::vector < void * > dwnsmplr100_out_pckg_buf_vec;
    std::vector < void * > dwnsmplr101_out_pckg_buf_vec;
    std::vector < void * > dwnsmplr110_out_pckg_buf_vec;
    std::vector < void * > dwnsmplr111_out_pckg_buf_vec;
    /*------------------------------------------------------------------------*/

    /* Initialize USB */
    std::vector < void * >     usb_recv_pckg_buf_vec; /* List of buffers to receive USB packages */
    usb_gadget::alloc_recv_pckg_buf  (usb_recv_pckg_buf_vec);
    usb_gadget * usb = new usb_gadget(usb_recv_pckg_buf_vec, usb_streams_vector);
    /*------------------------------------------------------------------------*/

    /* Initialize upsamplers */
    std::vector < void * > usmplr000_out_pckg_buf_vec;
    std::vector < void * > usmplr001_out_pckg_buf_vec;
    std::vector < void * > usmplr010_out_pckg_buf_vec;
    std::vector < void * > usmplr011_out_pckg_buf_vec;

    upsampler::alloc_out_pckg_buf(usmplr000_out_pckg_buf_vec);
    upsampler::alloc_out_pckg_buf(usmplr001_out_pckg_buf_vec);
    upsampler::alloc_out_pckg_buf(usmplr010_out_pckg_buf_vec);
    upsampler::alloc_out_pckg_buf(usmplr011_out_pckg_buf_vec);

    upsampler * usmplr_000_ptr =
                                                                    new upsampler(usmplr000_out_pckg_buf_vec,
                                  usb_streams_vector.at(TO_STREAM_IDX(DIR_FROM_USB,MCASP0,TS0)).que,
                                  TO_MCASP_DATA(                                   MCASP0,TS0) .in_que,
                                  usb_streams_vector.at(TO_STREAM_IDX(DIR_FROM_USB,MCASP0,TS0)).r_stat.recv_sample_rate,
                                  TO_MCASP_DATA(                                   MCASP0,TS0) .sample_rate);
    usmplr_000_ptr-> set_in_sync( usb_streams_vector.at(TO_STREAM_IDX(DIR_FROM_USB,MCASP0,TS0)).cond_var_ptr,
                                  usb_streams_vector.at(TO_STREAM_IDX(DIR_FROM_USB,MCASP0,TS0)).mtx_ptr);
    usmplr_000_ptr->set_out_sync( TO_MCASP_DATA(                                   MCASP0,TS0) .in_cond_var_ptr,
                                  TO_MCASP_DATA(                                   MCASP0,TS0) .in_mtx_ptr);
    upsampler * usmplr_001_ptr =
                                                                   new upsampler(usmplr001_out_pckg_buf_vec,
                                  usb_streams_vector.at(TO_STREAM_IDX(DIR_FROM_USB,MCASP0,TS1)).que,
                                  TO_MCASP_DATA(                                   MCASP0,TS1) .in_que,
                                  usb_streams_vector.at(TO_STREAM_IDX(DIR_FROM_USB,MCASP0,TS1)).r_stat.recv_sample_rate,
                                  TO_MCASP_DATA(                                   MCASP0,TS1) .sample_rate);
    usmplr_001_ptr-> set_in_sync( usb_streams_vector.at(TO_STREAM_IDX(DIR_FROM_USB,MCASP0,TS1)).cond_var_ptr,
                                  usb_streams_vector.at(TO_STREAM_IDX(DIR_FROM_USB,MCASP0,TS1)).mtx_ptr);
    usmplr_001_ptr->set_out_sync( TO_MCASP_DATA(                                   MCASP0,TS1) .in_cond_var_ptr,
                                  TO_MCASP_DATA(                                   MCASP0,TS1) .in_mtx_ptr);
    upsampler * usmplr_010_ptr =
                                                                   new upsampler(usmplr010_out_pckg_buf_vec,
                                  usb_streams_vector.at(TO_STREAM_IDX(DIR_FROM_USB,MCASP1,TS0)).que,
                                  TO_MCASP_DATA(                                   MCASP1,TS0) .in_que,
                                  usb_streams_vector.at(TO_STREAM_IDX(DIR_FROM_USB,MCASP1,TS0)).r_stat.recv_sample_rate,
                                  TO_MCASP_DATA(                                   MCASP1,TS0) .sample_rate);
    usmplr_010_ptr-> set_in_sync( usb_streams_vector.at(TO_STREAM_IDX(DIR_FROM_USB,MCASP1,TS0)).cond_var_ptr,
                                  usb_streams_vector.at(TO_STREAM_IDX(DIR_FROM_USB,MCASP1,TS0)).mtx_ptr);
    usmplr_010_ptr->set_out_sync( TO_MCASP_DATA(                                   MCASP1,TS0) .in_cond_var_ptr,
                                  TO_MCASP_DATA(                                   MCASP1,TS0) .in_mtx_ptr);

    upsampler * usmplr_011_ptr =
                                                                   new upsampler(usmplr011_out_pckg_buf_vec,
                                  usb_streams_vector.at(TO_STREAM_IDX(DIR_FROM_USB,MCASP1,TS1)).que,
                                  TO_MCASP_DATA(                                   MCASP1,TS1) .in_que,
                                  usb_streams_vector.at(TO_STREAM_IDX(DIR_FROM_USB,MCASP1,TS1)).r_stat.recv_sample_rate,
                                  TO_MCASP_DATA(                                   MCASP1,TS1) .sample_rate);
    usmplr_011_ptr-> set_in_sync( usb_streams_vector.at(TO_STREAM_IDX(DIR_FROM_USB,MCASP1,TS1)).cond_var_ptr,
                                  usb_streams_vector.at(TO_STREAM_IDX(DIR_FROM_USB,MCASP1,TS1)).mtx_ptr);
    usmplr_011_ptr->set_out_sync( TO_MCASP_DATA(                                   MCASP1,TS1) .in_cond_var_ptr,
                                  TO_MCASP_DATA(                                   MCASP1,TS1) .in_mtx_ptr);
    /*------------------------------------------------------------------------*/

    /* Initialize downsamplers */
    downsampler::alloc_out_pckg_buf(dwnsmplr100_out_pckg_buf_vec);
    downsampler::alloc_out_pckg_buf(dwnsmplr101_out_pckg_buf_vec);
    downsampler::alloc_out_pckg_buf(dwnsmplr110_out_pckg_buf_vec);
    downsampler::alloc_out_pckg_buf(dwnsmplr111_out_pckg_buf_vec);

    struct downsampler_data ddata_100 =
    {
        .out_pckg_bufs      =                                            dwnsmplr100_out_pckg_buf_vec,
        .in_que             = TO_MCASP_DATA(                                 MCASP0,TS0) .out_que,          /* <- from MCASP */
        .out_que            = usb_streams_vector.at(TO_STREAM_IDX(DIR_TO_USB,MCASP0,TS0)).que,              /* -> to USB     */
        .in_cond_var_ptr    = TO_MCASP_DATA(                                 MCASP0,TS0) .out_cond_var_ptr, /* <- from MCASP */
        .out_cond_var_ptr   = usb_streams_vector.at(TO_STREAM_IDX(DIR_TO_USB,MCASP0,TS0)).cond_var_ptr,     /* -> to USB     */
        .in_mtx_ptr         = TO_MCASP_DATA(                                 MCASP0,TS0) .out_mtx_ptr,      /* <- from MCASP */
        .out_mtx_ptr        = usb_streams_vector.at(TO_STREAM_IDX(DIR_TO_USB,MCASP0,TS0)).mtx_ptr,          /* -> to USB     */
        .mcasp_idx          =                                                MCASP0,
        .ts_idx             =                                                       TS0,
        .dev_num            = 0x0,
        .input_rate         = 16000.0,
        .output_rate        = 8000.0
    };
    struct downsampler_data ddata_101 =
    {
        .out_pckg_bufs      =                                            dwnsmplr101_out_pckg_buf_vec,
        .in_que             = TO_MCASP_DATA(                                 MCASP0,TS1) .out_que,          /* <- from MCASP */
        .out_que            = usb_streams_vector.at(TO_STREAM_IDX(DIR_TO_USB,MCASP0,TS1)).que,              /* -> to USB     */
        .in_cond_var_ptr    = TO_MCASP_DATA(                                 MCASP0,TS1) .out_cond_var_ptr, /* <- from MCASP */
        .out_cond_var_ptr   = usb_streams_vector.at(TO_STREAM_IDX(DIR_TO_USB,MCASP0,TS1)).cond_var_ptr,     /* -> to USB     */
        .in_mtx_ptr         = TO_MCASP_DATA(                                 MCASP0,TS1) .out_mtx_ptr,      /* <- from MCASP */
        .out_mtx_ptr        = usb_streams_vector.at(TO_STREAM_IDX(DIR_TO_USB,MCASP0,TS1)).mtx_ptr,          /* -> to USB     */
        .mcasp_idx          =                                                MCASP0,
        .ts_idx             =                                                       TS1,
        .dev_num            = 0x0,
        .input_rate         = 16000.0,
        .output_rate        = 8000.0
    };
    struct downsampler_data ddata_110 =
    {
        .out_pckg_bufs      =                                            dwnsmplr110_out_pckg_buf_vec,
        .in_que             = TO_MCASP_DATA(                                 MCASP1,TS0) .out_que,          /* <- from MCASP */
        .out_que            = usb_streams_vector.at(TO_STREAM_IDX(DIR_TO_USB,MCASP1,TS0)).que,              /* -> to USB     */
        .in_cond_var_ptr    = TO_MCASP_DATA(                                 MCASP1,TS0) .out_cond_var_ptr, /* <- from MCASP */
        .out_cond_var_ptr   = usb_streams_vector.at(TO_STREAM_IDX(DIR_TO_USB,MCASP1,TS0)).cond_var_ptr,     /* -> to USB     */
        .in_mtx_ptr         = TO_MCASP_DATA(                                 MCASP1,TS0) .out_mtx_ptr,      /* <- from MCASP */
        .out_mtx_ptr        = usb_streams_vector.at(TO_STREAM_IDX(DIR_TO_USB,MCASP1,TS0)).mtx_ptr,          /* -> to USB     */
        .mcasp_idx          =                                                MCASP1,
        .ts_idx             =                                                       TS0,
        .dev_num            = 0x0,
        .input_rate         = 16000.0,
        .output_rate        = 8000.0
    };
    struct downsampler_data ddata_111 =
    {
        .out_pckg_bufs      =                                            dwnsmplr111_out_pckg_buf_vec,
        .in_que             = TO_MCASP_DATA(                                 MCASP1,TS1) .out_que,          /* <- from MCASP */
        .out_que            = usb_streams_vector.at(TO_STREAM_IDX(DIR_TO_USB,MCASP1,TS1)).que,              /* -> to USB     */
        .in_cond_var_ptr    = TO_MCASP_DATA(                                 MCASP1,TS1) .out_cond_var_ptr, /* <- from MCASP */
        .out_cond_var_ptr   = usb_streams_vector.at(TO_STREAM_IDX(DIR_TO_USB,MCASP1,TS1)).cond_var_ptr,     /* -> to USB     */
        .in_mtx_ptr         = TO_MCASP_DATA(                                 MCASP1,TS1) .out_mtx_ptr,      /* <- from MCASP */
        .out_mtx_ptr        = usb_streams_vector.at(TO_STREAM_IDX(DIR_TO_USB,MCASP1,TS1)).mtx_ptr,          /* -> to USB     */
        .mcasp_idx          =                                                MCASP1,
        .ts_idx             =                                                       TS1,
        .dev_num            = 0x0,
        .input_rate         = 16000.0,
        .output_rate        = 8000.0
    };

    downsampler * dwnsplr_100_ptr =  new downsampler(ddata_100);
    downsampler * dwnsplr_101_ptr =  new downsampler(ddata_101);
    downsampler * dwnsplr_110_ptr =  new downsampler(ddata_110);
    downsampler * dwnsplr_111_ptr =  new downsampler(ddata_111);

    /* Initialize MCASPs */
    mcasp_ts::alloc_recv_pckg_buf(mcasp100_recv_pckg_buf_vec);
    mcasp_ts::alloc_recv_pckg_buf(mcasp101_recv_pckg_buf_vec);
    mcasp_ts::alloc_recv_pckg_buf(mcasp110_recv_pckg_buf_vec);
    mcasp_ts::alloc_recv_pckg_buf(mcasp111_recv_pckg_buf_vec);

    mcasp_ts * pru0_mcasp0_ts0 = new mcasp_ts(mcasp100_recv_pckg_buf_vec,
                                              TO_MCASP_DATA(MCASP0, TS0));
    mcasp_ts * pru0_mcasp0_ts1 = new mcasp_ts(mcasp101_recv_pckg_buf_vec,
                                              TO_MCASP_DATA(MCASP0, TS1));
    mcasp_ts * pru0_mcasp1_ts0 = new mcasp_ts(mcasp110_recv_pckg_buf_vec,
                                              TO_MCASP_DATA(MCASP1, TS0));
    mcasp_ts * pru0_mcasp1_ts1 = new mcasp_ts(mcasp111_recv_pckg_buf_vec,
                                              TO_MCASP_DATA(MCASP1, TS1));

    /* Start all */
    usmplr_000_ptr->start();
    usmplr_001_ptr->start();
    usmplr_010_ptr->start();
    usmplr_011_ptr->start();

    dwnsplr_100_ptr->start();
    //dwnsplr_101_ptr->start();
    dwnsplr_110_ptr->start();
    //dwnsplr_111_ptr->start();

    pru0_mcasp0_ts0->start();
    pru0_mcasp0_ts1->start();
    pru0_mcasp1_ts0->start();
    pru0_mcasp1_ts1->start();

    /* Register signals */
    std::signal(SIGINT,  signal_hndlr);
    std::signal(SIGTERM, signal_hndlr);
    std::signal(SIGQUIT, signal_hndlr);
    std::signal(SIGTSTP, signal_hndlr);

    while (keep_running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    delete usb;

    delete usmplr_000_ptr;
    delete usmplr_001_ptr;
    delete usmplr_010_ptr;
    delete usmplr_011_ptr;

    delete dwnsplr_100_ptr;
    delete dwnsplr_101_ptr;
    delete dwnsplr_110_ptr;
    delete dwnsplr_111_ptr;

    delete pru0_mcasp0_ts0;
    delete pru0_mcasp0_ts1;
    delete pru0_mcasp1_ts0;
    delete pru0_mcasp1_ts1;

    usb_gadget::dealloc_recv_pckg_buf(  usb_recv_pckg_buf_vec);

    upsampler::dealloc_out_pckg_buf(usmplr000_out_pckg_buf_vec);
    upsampler::dealloc_out_pckg_buf(usmplr001_out_pckg_buf_vec);
    upsampler::dealloc_out_pckg_buf(usmplr010_out_pckg_buf_vec);
    upsampler::dealloc_out_pckg_buf(usmplr011_out_pckg_buf_vec);

    mcasp_ts::dealloc_recv_pckg_buf(mcasp100_recv_pckg_buf_vec);
    mcasp_ts::dealloc_recv_pckg_buf(mcasp101_recv_pckg_buf_vec);
    mcasp_ts::dealloc_recv_pckg_buf(mcasp110_recv_pckg_buf_vec);
    mcasp_ts::dealloc_recv_pckg_buf(mcasp111_recv_pckg_buf_vec);

    downsampler::dealloc_out_pckg_buf(dwnsmplr100_out_pckg_buf_vec);
    downsampler::dealloc_out_pckg_buf(dwnsmplr101_out_pckg_buf_vec);
    downsampler::dealloc_out_pckg_buf(dwnsmplr110_out_pckg_buf_vec);
    downsampler::dealloc_out_pckg_buf(dwnsmplr111_out_pckg_buf_vec);
}
