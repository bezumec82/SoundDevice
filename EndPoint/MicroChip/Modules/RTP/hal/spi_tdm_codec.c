#include "driver.h"

static vocoder_t * spi_tdm_vocoder = NULL;

/*------------------------------------------------------------------------------------------------*/
static int spi_tdm_channel_allocate(channel_t* c, int packet_factor)
{
    struct  spi_tdm_params params;

    params.context=c;
    params.channel_number=-1;

    int error = spi_tdm_allocate(&params, &c->spi_tdm);
    if(error<0) return error;

    c->tdm = spi_tdm_timeslot(c->spi_tdm);
    c->slot = spi_tdm_slot(c->spi_tdm);

    sprintf(c->devname, "spi-tdm-%d", c->slot);
    spi_tdm_set_owner(c->spi_tdm, c->devname);

    c->read_frame = read_frame;
    c->play_frame = play_frame;

    DPRINTK("tdm=%d, capabilities=%08x\n", c->tdm, c->parent->caps);
    c->encoded_bytes = encoder_fifo_count;
    register_channel(c->tdm, c, c->parent->ops);
}
/*--- End of function ----------------------------------------------------------------------------*/

static void spi_tdm_channel_uninstall(channel_t* c)
{
    unregister_channel(c->tdm);
    spi_tdm_release(c->spi_tdm);
}
/*--- End of function ----------------------------------------------------------------------------*/

/*! @{ @brief Initialization facilities */
static int spi_tdm_channel_install(void)
{
    int n_chns = spi_tdm_num_of_channels();
    
    /* allocate memory for codec and channels */
    int v_size = sizeof(vocoder_t) + sizeof(channel_t) * n_chns;
    vocoder_t * vocoder_ptr = kmalloc(v_size, GFP_ATOMIC);
    if(vocoder_ptr==NULL) return -ENOMEM;
    memset(vocoder_ptr, 0, v_size);

    vocoder_ptr->id         =   CODEC_ID_LIN16;
    vocoder_ptr->caps       =   CAP_LIN16;
    vocoder_ptr->caps       |=  (v->caps << 16);
    vocoder_ptr->n_chns     =   n_chns;
    vocoder_ptr->channels   =   &v->chan_data[0];
    vocoder_ptr->ops.name   =   codec_name;
    vocoder_ptr->read_frame =   lin16_read_frame;
    vocoder_ptr->play_frame =   lin16_play_frame;

    /* init channel lists */
    int i;
    for(i=0; i < vocoder_ptr->n_chns; ++i)
    {
        channel_t* c= &v->channels[i];
        c->parent=v;
        spi_tdm_channel_allocate(c, SPI_TDM_PACKET_FACTOR);
    }
    sema_init(&vocoder_ptr->lock, 1);
    /* add vocoder to list*/
    spi_tdm_vocoder = vocoder_ptr;
    DPRINTK("spi tdm init channels\n");
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

/*! TDM initialization function.
 * Practically this is wrapper aroud #spi_tdm_channel_install.
 * @param context - Not used in this routine.
 *                  Structure #hal_context will be passed by #init_startup.
 */
int spi_tdm_init(void *context)
{
    spi_tdm_channel_install();
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

void spi_tdm_deinit(void *context)
{
    vocoder_t* v = spi_tdm_vocoder;
    int i;
    for(i=0; i < v->n_chns; ++i)
        spi_tdm_channel_uninstall(&v->channels[i]);
    kfree(v);
    DPRINTK("spi tdm channels uninstall.\n");
}
/*! @} -------------------------------------------------------------------------------------------*/

/*--- End of the file ----------------------------------------------------------------------------*/



