/*! @file codec.c */

#include "driver.h"

extern void mixer_mute_all(vocoder_t *);

struct list_head vocoders;
struct semaphore vocoders_lock;

unsigned int caps_from_pt(unsigned short rx_pt, unsigned short tx_pt)
{
    int i;
    unsigned int caps = 0;
    unsigned short pt = rx_pt;
    for (i=0; i<2; ++i) {
        switch (pt) {
            case PAYLOAD_TYPE_G711U_MCC:    caps |= CAP_MCC;
            case PAYLOAD_TYPE_G711U:        caps |= CAP_G711U;  break;
            case PAYLOAD_TYPE_G723:         caps |= CAP_G723;   break;
            case PAYLOAD_TYPE_G711A_MCC:    caps |= CAP_MCC;
            case PAYLOAD_TYPE_G711A:        caps |= CAP_G711;   break;
            case PAYLOAD_TYPE_G729:         caps |= CAP_G729;   break;
            case PAYLOAD_TYPE_LIN16:        caps |= CAP_LIN16;  break;
        }
        if (i==0) caps <<= 16;
        pt = tx_pt;
    }
    return caps;
}
/*--- End of function ----------------------------------------------------------------------------*/
static unsigned int mask_from_id(unsigned short codec_id)
{
    unsigned int mask = 0;
    switch (codec_id) {
        case CODEC_ID_G711:     mask = CAP_G711;    break;
        case CODEC_ID_G723:     mask = CAP_G723;    break;
        case CODEC_ID_G729:     mask = CAP_G729;    break;
        case CODEC_ID_LIN16:    mask = CAP_LIN16;   break;          
        default:                mask = 0xFFFF;      break;
    }
    return (mask | (mask << 16));
}
/*--- End of function ----------------------------------------------------------------------------*/
static unsigned int caps_from_id(unsigned short codec_id)
{
    unsigned int caps = 0;
    switch (codec_id) {
        case CODEC_ID_G711:     caps = CAP_G711;    break;
        case CODEC_ID_G723:     caps = CAP_G723;    break;
        case CODEC_ID_G729:     caps = CAP_G729;    break;
        case CODEC_ID_LIN16:    caps = CAP_LIN16;   break;
        default:
            break;
    }
    return (caps | (caps << 16));
}
/*--- End of function ----------------------------------------------------------------------------*/
char* codec_name_from_id( unsigned short codec_id )
{
    switch( codec_id )
    {
        case CODEC_ID_G711:     return "G.711";     break;
        case CODEC_ID_G723:     return "G.723";     break;
        case CODEC_ID_G729:     return "G.729";     break;
        case CODEC_ID_MIXER:    return "Mixer";     break;
        case CODEC_ID_CMIXER:   return "cMixer";    break;
        case CODEC_ID_LIN16:    return "LIN.16";    break;
        default:                return "Unknown";   break;
    }
    return "Unknown";
}
/*--- End of function ----------------------------------------------------------------------------*/
int codec_id_valid( unsigned short codec_id )
{
    return ((codec_id == CODEC_ID_G711)     ||
            (codec_id == CODEC_ID_G723)     ||
            (codec_id == CODEC_ID_MIXER)    ||
            (codec_id == CODEC_ID_CMIXER)   ||
            (codec_id == CODEC_ID_LIN16));
}


/*--- End of function ----------------------------------------------------------------------------*/
size_t dummy_read_enc_frame(struct channel* c,  int *payloadtype, void * buffer, size_t buffer_size)
{
    return buffer_size;
}
/*--- End of function ----------------------------------------------------------------------------*/
size_t dummy_write_enc_frame(struct channel* c, int payloadtype, void * buffer, size_t buffer_size)
{
    return buffer_size;
}
/*--- End of function ----------------------------------------------------------------------------*/
size_t dummy_read_dec_frame(struct channel* c,  int *payloadtype, void * buffer, size_t buffer_size)
{
    return buffer_size;
}
/*--- End of function ----------------------------------------------------------------------------*/
size_t dummy_write_dec_frame(struct channel* c, int payloadtype, void * buffer, size_t buffer_size)
{
    return buffer_size;
}
/*--- End of function ----------------------------------------------------------------------------*/
size_t dummy_frame_size(struct channel* c)
{
    return CHANNEL_FRAME_SIZE;
}
/*--- End of function ----------------------------------------------------------------------------*/
size_t dummy_decode_frame(struct channel* c, void * inbuffer, size_t inbuffer_size, void * outbuffer, size_t outbuffer_size)
{
    return (inbuffer_size > outbuffer_size) ? outbuffer_size : inbuffer_size;
}
/*--- End of function ----------------------------------------------------------------------------*/
size_t dummy_encode_frame(struct channel*c, void * inbuffer, size_t inbuffer_size, void * outbuffer, size_t outbuffer_size)
{
    return (inbuffer_size > outbuffer_size) ? outbuffer_size : inbuffer_size;
}
/*--- End of function ----------------------------------------------------------------------------*/


size_t encoder_fifo_count(channel_t * c)
{
    return circular_buffer_lenght(&c->encoder_fifo);
}
/*--- End of function ----------------------------------------------------------------------------*/
size_t decoder_fifo_count(channel_t * c)
{
    return circular_buffer_lenght(&c->decoder_fifo);
}
/*--- End of function ----------------------------------------------------------------------------*/
int jb_monitor(channel_t * c)
{
    int dist = circular_buffer_lenght(&c->decoder_fifo);
    if (c->decoder_freeze < 0)
    {
        if (dist > c->decoder_upper_watermark)
        {
            /*DPRINTK("positive slip, dist=%d\n", dist);*/
            c->ofw++;
            circular_buffer_set_lenght(&c->decoder_fifo, c->decoder_lower_drop_watermark);
            dist = c->decoder_lower_drop_watermark;
        }
        else if (dist < c->decoder_lower_watermark) 
        {
            /*DPRINTK("negative slip, dist=%d\n", dist);*/
            c->ufw++;
            circular_buffer_set_lenght(&c->decoder_fifo, c->decoder_lower_drop_watermark);
            dist = c->decoder_lower_drop_watermark;
        }
    } else {
        circular_buffer_set_lenght(&c->decoder_fifo, c->decoder_freeze);
        c->decoder_freeze = -1;
    }
    return dist;
}
/*--- End of function ----------------------------------------------------------------------------*/
int jb_reset(channel_t * c)
{
    int dist = circular_buffer_lenght(&c->decoder_fifo);
    if (dist > c->decoder_lower_watermark)
    {
        circular_buffer_set_lenght(&c->decoder_fifo, c->decoder_lower_watermark);
        dist = c->decoder_lower_watermark;
    }
    return dist;
}
/*--- End of function ----------------------------------------------------------------------------*/
int vocoder_install(int dspno, int id)
{
    dsp_t dsp;
    int timeslots[64];
    int err;
    int n_chns;
    int v_size;
    int i;
    vocoder_t * v;
    channel_t * c;
    
    if (!codec_id_valid(id))
    {
        DPRINTK("unknown codec id=%d\n", id);
        return -EBUSY;
    }
    
    DPRINTK("installing codec %s on DSP%d ...\n", codec_name_from_id(id), dspno);
    DPRINTK("try to allocate DSP%d\n", dspno);

    err=dsp_allocate(dspno, &dsp, codec_name_from_id(id));
    if(err < 0) {
        DPRINTK("unable to allocate DSP%d, code=%d\n", dspno, err);
        return -EBUSY;   /* unable to allocate dsp */
    }

    virtual_codec_install(dsp /*, mod */);
    n_chns = read_channels_count(dsp);
    if (n_chns >= 32) {
        DPRINTK("incorrect channels number %d\n", n_chns);
        return -ENODEV;
    }

    err=dsp_alloc_tdm(dsp, n_chns, 0, 0, timeslots);
    if(err < 0) {
        dsp_free(dsp);
        DPRINTK("cannot allocate tdm slots, code=%d\n", err);
        return -ENXIO;
    }

    /* allocate memory for codec and channels */
    v_size=sizeof(vocoder_t)+sizeof(channel_t) * n_chns;
    v = kmalloc(v_size, GFP_ATOMIC);
    if(v==NULL) {
        virtual_codec_uninstall(dsp);
        return -ENOMEM;
    }
    memset(v, 0, v_size);

    DPRINTK("codec %s successfully installed on DSP%d\n", codec_name_from_id(id), dspno);

    v->id=id;
    v->dsp=dsp;
    v->tdm_offset=timeslots[0];
    v->n_chns = n_chns;
    v->channels=&v->chan_data[0];

    v->ops.name         = codec_name;

    v->read_enc_frame   = dummy_read_enc_frame;
    v->read_dec_frame   = dummy_read_dec_frame;
    v->write_enc_frame  = dummy_write_enc_frame;
    v->write_dec_frame  = dummy_write_dec_frame;
    v->decode_frame     = dummy_decode_frame;
    v->encode_frame     = dummy_encode_frame;

    switch (id) {
        case CODEC_ID_MIXER:
        case CODEC_ID_CMIXER:
            v->wd_restore = mixer_restore;
        break;
        
        case CODEC_ID_G711:
            v->read_enc_frame   = g711_read_enc_frame;
            v->read_dec_frame   = g711_read_dec_frame;
            v->write_enc_frame  = g711_write_enc_frame;
            v->write_dec_frame  = g711_write_dec_frame;
            v->frame_bytes      = g711_frame_bytes;
            v->decode_frame     = g711_decode_frame;
            v->encode_frame     = g711_encode_frame;
        break;
        
        case CODEC_ID_LIN16:
            v->read_enc_frame   = lin16_read_enc_frame;
            v->read_dec_frame   = lin16_read_dec_frame;
            v->write_enc_frame  = lin16_write_enc_frame;
            v->write_dec_frame  = lin16_write_dec_frame;
            v->frame_bytes      = lin16_frame_bytes;
            v->decode_frame     = lin16_decode_frame;
            v->encode_frame     = lin16_encode_frame;
        break;      

        case CODEC_ID_G723:
        case CODEC_ID_G729:
        break;
    }

    read_exchange_block(v);
    sema_init(&v->lock, 1);

    /* init channel lists */
    for(i=0; i < v->n_chns; ++i)
    {
        c = &v->channels[i];
        c->parent=v;
        c->slot=i;
        c->tdm = timeslots[i] + (v->dsp->bus_number * CHNS_PER_BUS);
        c->encoded_bytes = encoder_fifo_count;
        c->decoded_bytes = decoder_fifo_count;
        channel_init(c);
    }

    if (v->n_mixers) 
        mixer_mute_all(v);
    
//  create_proc_vocoder(v);
    /* add vocoder to list*/
    down(&vocoders_lock);
    list_add(&v->node, &vocoders);
    up(&vocoders_lock);
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

vocoder_t * vocoder_find(int dspno)
{
    struct list_head *tmp;
    vocoder_t * v;
    int ctr=0;
    list_for_each(tmp, &vocoders)
    {
        v=list_entry(tmp, vocoder_t, node);
        if(dsp_get_number(v->dsp)==dspno) return v;
        ++ctr;
    }
    return NULL;
}
/*--- End of function ----------------------------------------------------------------------------*/

vocoder_t * find_mixer_dev(int no)
{
    struct list_head *tmp;
    vocoder_t * v;
    down(&vocoders_lock);
    list_for_each(tmp, &vocoders) {
        v=list_entry(tmp, vocoder_t, node);
        if(dsp_get_number(v->dsp)==no)
        {
            up(&vocoders_lock);
            return v;
        }
    }
    up(&vocoders_lock);
    return NULL;
}
/*--- End of function ----------------------------------------------------------------------------*/

int vocoder_get_num_channels(int id)
{
    channel_t * c;
    vocoder_t * v;
    int i;
    int ctr=0;
    unsigned int capabilities = caps_from_id(id);
    unsigned int mask = mask_from_id(id);

    /* try to alloc preffered codec */
    for (i=TDM_DSP_START; i < TDM_MAP_SIZE; i++)
    {
        c = (channel_t *)tdm_map_context(i);
        if (c == 0) continue;
        v = c->parent;
        if( (v->id==id && (v->caps & mask)==capabilities) || (v->id==CODEC_ID_G711_MCC && id==CODEC_ID_G711_MCC) )
            ++ctr;
    }
    return ctr;
}
/*--- End of function ----------------------------------------------------------------------------*/

int vocoder_alloc_channel(int id)
{
    channel_t * c;
    vocoder_t * v;
    int i;
    unsigned int capabilities = caps_from_id(id);
    unsigned int mask = mask_from_id(id);

    /* try to alloc preffered codec */
    for (i=TDM_DSP_START; i < TDM_MAP_SIZE; i++) {
        c = (channel_t *)tdm_map_context(i);
        if (c == 0) continue;
        if (c->busy) continue;
        v = c->parent;
        if( (v->id==id && (v->caps & mask)==capabilities) || (v->id==CODEC_ID_G711_MCC && id==CODEC_ID_G711_MCC) )
            return (c->tdm);
    }
    
    /* find alloved codec */
    for (i=TDM_DSP_START; i < TDM_MAP_SIZE; i++) {
        c = (channel_t *)tdm_map_context(i);
        if (c == 0) continue;
        if (c->busy) continue;
        v = c->parent;
        if (v->caps & capabilities)
            return (c->tdm);
    }
    
    return (-1);
}
/*--- End of function ----------------------------------------------------------------------------*/

int vocoder_stat_channel(int channel, struct channel_stat_param *info)
{
    channel_t * c = NULL;
    int result = -1;
    if (info)
    {
        c = (channel_t *)tdm_map_context(channel);
        if (c)
        {
            info->decoder_fifo_stat.read_bytes      = c->decoder_fifo_stat.read_bytes;
            info->decoder_fifo_stat.read_frames     = c->decoder_fifo_stat.read_frames;
            info->decoder_fifo_stat.read_calls      = c->decoder_fifo_stat.read_calls;
            info->decoder_fifo_stat.write_bytes     = c->decoder_fifo_stat.write_bytes;
            info->decoder_fifo_stat.write_frames    = c->decoder_fifo_stat.write_frames;
            info->decoder_fifo_stat.write_calls     = c->decoder_fifo_stat.write_calls;
            info->decoder_fifo_stat.size            = decoder_fifo_count(c);

            info->encoder_fifo_stat.read_bytes      = c->encoder_fifo_stat.read_bytes;
            info->encoder_fifo_stat.read_frames     = c->encoder_fifo_stat.read_frames;
            info->encoder_fifo_stat.read_calls      = c->encoder_fifo_stat.read_calls;
            info->encoder_fifo_stat.write_bytes     = c->encoder_fifo_stat.write_bytes;
            info->encoder_fifo_stat.write_frames    = c->encoder_fifo_stat.write_frames;            
            info->encoder_fifo_stat.write_calls     = c->encoder_fifo_stat.write_calls;
            info->encoder_fifo_stat.size            = encoder_fifo_count(c);

            info->ofw = c->ofw;
            info->ufw = c->ufw;

            result = 0;
        }
    }
    return result;
}
/*--- End of function ----------------------------------------------------------------------------*/

int vocoder_remove(vocoder_t * v)
{
    int i;
//  remove_proc_vocoder(v);
    down(&vocoders_lock);
    list_del(&v->node);
    up(&vocoders_lock);

    DPRINTK("deleted from vocoders list, destroying channels\n");

    for(i=0; i < v->n_chns; ++i)
        channel_disconnect(&v->channels[i]);
    
    for(i=0; i < v->n_chns; ++i)
        channel_deinit(&v->channels[i]);

    DPRINTK("channels destroyed, uninstalling codec...\n");
    virtual_codec_uninstall(v->dsp);
    kfree(v);
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

int vocoder_uninstall(int dspno)
{
    vocoder_t * v;
    DPRINTK("uninstalling codec on dsp.%d\n", dspno);   
    v=vocoder_find(dspno);
    if(v==NULL) return -ENXIO;
    return vocoder_remove(v);
}
/*--- End of function ----------------------------------------------------------------------------*/

int vocoders_init(void *context)
{
    INIT_LIST_HEAD(&vocoders);
//  skb_queue_head_init(&tx_queue);
    sema_init(&vocoders_lock, 1);
    g711_table_init();
    init_mixers();
    DPRINTK("vocoders initialized.\n");
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

void vocoders_deinit(void *context)
{
    vocoder_t * v;
    struct list_head *tmp, *ntmp;
    
    cleanup_mixers();
    list_for_each_safe(tmp, ntmp, &vocoders)
    {
        v = list_entry(tmp, vocoder_t, node);
        vocoder_remove(v);
    }
}

EXPORT_SYMBOL(vocoder_remove);
EXPORT_SYMBOL(vocoder_install);
EXPORT_SYMBOL(vocoder_uninstall);
EXPORT_SYMBOL(vocoders_init);
EXPORT_SYMBOL(vocoders_deinit);
EXPORT_SYMBOL(vocoder_find);
EXPORT_SYMBOL(vocoder_get_num_channels);
EXPORT_SYMBOL(vocoder_alloc_channel);
EXPORT_SYMBOL(vocoder_stat_channel);
EXPORT_SYMBOL(caps_from_pt);
EXPORT_SYMBOL(codec_name_from_id);

/*--- End of the file ----------------------------------------------------------------------------*/
