#include "main.hpp"

periodic_send::periodic_send()
{
    // create a timer
    qtimer = new QTimer(this);
    // setup signal and slot
    connect(qtimer, SIGNAL(timeout()), this, SLOT(timer_slot()));
    /* Fill buffers */
    for(int i = 0; i < AUDIOBUFS_NUM; i++)
    {
        uint8_t * out_buf_ptr = (uint8_t * )malloc(SEND_BUF_SIZE_BYTES);
        out_buf_list.push_back(out_buf_ptr);
    }
    adata_buf_iter = out_buf_list.begin();
    /* Find USB device */
    liraEnumerator::usbEnum aLiraScanner(L"LIRA");
    aLiraScanner.doScan();
    pUSBDeviceDSC = aLiraScanner.Find(liraEnumerator::speaker);
    if (pUSBDeviceDSC)
    {
        std::cout << "Found USB device : \n"
                  << "  Device name       : " << pUSBDeviceDSC->device_name.toStdString()           << '\n'
                  << "  Device type       : " << std::hex << pUSBDeviceDSC->device_type << std::dec << '\n'
                  << "  Device revision   : " << pUSBDeviceDSC->device_rev                          << '\n';

        std::cout << "Opening " << pUSBDeviceDSC->device_name.toStdString() << '\n';
        mDevice = open(pUSBDeviceDSC->device_name.toStdString().c_str(),O_RDWR|O_NONBLOCK);
        if (mDevice==-1) std::cout << "Can't open " << pUSBDeviceDSC->device_name.toStdString() << '\n';
        /* Start timer */
        qtimer->start(AUDIO_QUANTUM_MS);
    }
    else
    {
        for (std::list< uint8_t * >::iterator iter=out_buf_list.begin(); iter != out_buf_list.end(); ++iter)
            if (*iter!=NULL) { free(*iter); }
        out_buf_list.clear();
    }
}

periodic_send::~periodic_send()
{
    if(mDevice >= 0) close(mDevice);
    for (std::list< uint8_t * >::iterator iter=out_buf_list.begin(); iter != out_buf_list.end(); ++iter)
        if (*iter!=NULL) { free(*iter); }
    out_buf_list.clear();
}


#define LI_GW_FLAG_PROTO_VER                            (0b01<<14)
#define LI_GW_FLAG_SET_STREAM_IDX(mcasp_idx, ts_idx)    ((mcasp_idx<<13)|(ts_idx<<12))
#define LI_GW_FLAG_DEV_NUM(dev_num)                     ((dev_num&0xf)<<8)

#define MCSEC_CYCLE         1
static int mcsec           =MCSEC_CYCLE;
static int base            =20;
static int damper          =10;
void periodic_send::timer_slot()
{
#if (1)
    damper--;
    if (damper == 0)
    {
        damper = 10;
        if (base == AUDIO_QUANTUM_MS)
            mcsec--;
        if (base == AUDIO_QUANTUM_MS-1)
            mcsec++;
    }
    if((mcsec == 0)||(mcsec == 1000))
    {
        if (base == AUDIO_QUANTUM_MS-1)
        {
            mcsec = MCSEC_CYCLE;
            base = AUDIO_QUANTUM_MS;
        }
        else
        {
            mcsec = 1000 - MCSEC_CYCLE;
            base = AUDIO_QUANTUM_MS-1;
        }
    }
    QThread::usleep(mcsec);
#endif
    qtimer->start(base);
    /* Send 20ms audio pckg */

    for (int j = 0;  j < SEND_BUF_SIZE_WORDS; j++ )
    {
        sample_num += 1.0;
        int16_t val_in = (int16_t)(SIN_AMPLITUDE * sin ( (sample_num / SIN_PERIOD_WORDS) * 2 * M_PI ) );
        uint8_t val_out = G711EncoderDecoder::linear2alaw(val_in);
        (* adata_buf_iter)[j] = val_out;
    }
#if (0)
    int ret = write(mDevice, * adata_buf_iter, SEND_BUF_SIZE_BYTES);
    if ( ret != SEND_BUF_SIZE_BYTES )
    { std::cout << "Can't write to " << pUSBDeviceDSC->device_name.toStdString() << '\n'; goto exit; }
#else
    uint8_t msgPckg[SEND_BUF_SIZE_BYTES + LI_GW_ZERO_LENGTH] = {0};
    unsigned int msgPckgLen = sizeof(msgPckg);
    unsigned short  msgFlags = LI_GW_FLAG_SET_STREAM_IDX(0,1)
                             | LI_GW_FLAG_DEV_NUM(0)
                             | LI_GW_FLAG_PROTO_VER;
    LiraProto.dataToPort( MSG_ID_ALAW_DATA_2,
                        * adata_buf_iter,
                        SEND_BUF_SIZE_BYTES,
                        msgFlags,
                        &msgPckg[0],
                        msgPckgLen
                        );
    int ret = write(mDevice, &msgPckg[0], msgPckgLen);
    if ( ret != msgPckgLen )
    { std::cout << "Can't write to " << pUSBDeviceDSC->device_name.toStdString() << '\n'; goto exit; }
#endif
    adata_buf_iter++;
    if (adata_buf_iter == out_buf_list.end()) { adata_buf_iter = out_buf_list.begin(); }
    return;
exit :
    qtimer->stop();
}


int main(int argc, char *argv[])
{  
    QCoreApplication a(argc, argv);
    periodic_send ps;
    a.exec();
}
