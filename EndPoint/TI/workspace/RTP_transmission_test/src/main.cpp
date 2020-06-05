#include "main.h"

using namespace jrtplib;

#define BUFFER_MAX_SIZE                 512
void Print_hex_data(
        void * data, /* data to print out */
        uint32_t data_count_bytes, /* how many bytes to print */
        const char * caller_name, /* usually __FUNCTION__ */
        uint32_t line_num /* line number */
        ) {
    uint32_t divider = 0;
    uint32_t buf_cntr = 0;
    int word_cntr = 0;
    if ((!data_count_bytes) || (data_count_bytes > BUFFER_MAX_SIZE)) {
        printf(RED"%-20.20s %-20.20s #%-5i: ERROR: Can't print HEX data.\r\n"
        "%49s Wrong data size: %i.\r\n" NORM,
        __FILE__, __FUNCTION__, __LINE__,
                "", data_count_bytes);
        return;
    }

    /*
     * 65 bytes header + 51 first spacing = 116 bytes
     * Output per line:
     * 50 spaces + 0xFF 0xFF 0xFF 0xFF\r\n
     * 50          5    5    5    5   3
     * 72 bytes per line
     */
    uint32_t lines_count = data_count_bytes / 4 + (data_count_bytes % 4 ? 1 : 0);
    uint32_t bytes_to_print = 116 + (lines_count + 1) * 72; /* One line in reserve */
    char buf[bytes_to_print];
    memset(buf, 0x00, bytes_to_print);

    buf_cntr = sprintf(&buf[0],
    BOLD"%-20.20s %-20.20s #%-5i: Hex data:\r\n"
    "%-50i",
    __FILE__, caller_name, line_num,
            word_cntr++);

    uint8_t * hex_stream = (uint8_t *) data;
    while (data_count_bytes) {
        if (*hex_stream < 0x0f) {
            buf_cntr += sprintf(&buf[buf_cntr], "0x0%x ", *hex_stream);
            if (buf_cntr > bytes_to_print) {
                printf(RED"%-20.20s %-20.20s #%-5i: STACK OVERFLOW IN FUNCTION!!!.\r\n" NORM,
                __FILE__, __FUNCTION__, __LINE__);
            }
        } else {
            buf_cntr += sprintf(&buf[buf_cntr], "0x%x ", *hex_stream);
            if (buf_cntr > bytes_to_print) {
                printf(RED"%-20.20s %-20.20s #%-5i: STACK OVERFLOW IN FUNCTION!!!.\r\n" NORM,
                __FILE__, __FUNCTION__, __LINE__);
            }
        }
        hex_stream++;
        data_count_bytes--;
        divider++;
        if (!(divider % 4)) { /* Start new line from 50 spaces */
            buf_cntr += sprintf(&buf[buf_cntr], "\r\n%-50i", word_cntr++);
            if (buf_cntr > bytes_to_print) {
                printf(RED"%-20.20s %-20.20s #%-5i: STACK OVERFLOW IN FUNCTION!!!.\r\n" NORM,
                __FILE__, __FUNCTION__, __LINE__);
            }
        }
    }
    printf("%s" NORM, &buf[0]);
    if (lines_count == 1) printf("\r\n");
    printf("\r\n");
}
/*----------------------------------------------------------------------------*/

#if (0)
// This function checks if there was a RTP error. If so, it displays an error
// message and exists.
void checkerror(int rtperr)
{
    if (rtperr < 0)
    {
        std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
        exit(-1);
    }
}
// The main routine
int main(int argc, char * argv[])
{
    (void)argc;
    (void)argv;

    RTPSession                  session;
    uint16_t                    portbase = 6000;
    uint16_t                    destport = 6000;
    int                         status;
    int                         pckg_num = 10;
    RTPUDPv4TransmissionParams  transParams;
    RTPSessionParams            sessParams;

    PRINTF(NORM, "PJRTP version : %s.\r\n",
           RTPLibraryVersion::GetVersion().GetVersionString().c_str());
    char * ipaddr = (char *)malloc(NI_MAXHOST);
    Get_IP_addr("eth0", ipaddr);
    uint32_t destip = inet_addr(ipaddr);
    if (destip == INADDR_NONE)
    {
        PRINT_ERR("Bad IP address specified.\r\n");
        return -1;
    }

    // The inet_addr function returns a value in network byte order,
    // but we need the IP address in host byte order,
    // so we use a call to 'ntohl'.
    destip = ntohl(destip);
    // Now, we'll create a RTP session, set the destination,
    // send some packets and poll for incoming data.

    // IMPORTANT: The local timestamp unit MUST be set, otherwise
    // RTCP Sender Report info will be calculated wrong.
    // In this case, we'll be sending 10 samples each second, so we'll
    // put the timestamp unit to (1.0/10.0)
    sessParams.SetOwnTimestampUnit(1.0/8000.0);
    sessParams.SetAcceptOwnPackets(true);
    transParams.SetPortbase(portbase);

    status = session.Create(sessParams,&transParams);
    checkerror(status);

    RTPIPv4Address addr(destip,destport);
    status = session.AddDestination(addr);
    checkerror(status);
    for (int i = 1 ; i <= pckg_num ; i++)
    {
        PRINTF(CYN, "Sending packet %d/%d\n", i, pckg_num);
        status = session.SendPacket(
                (void *)"1234567890",   //payload
                strlen("1234567890"),   //length
                0,                      //payload type
                false,                  //mark
                10                      //timestamp increment
                );
        checkerror(status);
        session.BeginDataAccess();
        // check incoming packets
        if (session.GotoFirstSourceWithData())
        {
            do
            {
                RTPPacket *pack;
                while ((pack = session.GetNextPacket()) != NULL)
                {
                    // You can examine the data here
                    printf("Got packet !\n");

                    // we don't longer need the packet, so
                    // we'll delete it
                    session.DeletePacket(pack);
                } //end while ...
            } while (session.GotoNextSourceWithData());
        } //end if ...
        session.EndDataAccess();

#ifndef RTP_SUPPORT_THREAD
        status = session.Poll();
        checkerror(status);
#endif // RTP_SUPPORT_THREAD

        RTPTime::Wait(RTPTime(1,0));
    } //end for ...

    session.BYEDestroy(RTPTime(10,0),0,0);
    return 0;
}
#else

#define RTP_P_FACTOR        160
#define MIXER_PAYLOAD       8
#define PCKGS_TO_SEND       10

int main(int argc, char * argv[])
{
    (void)argc;
    (void)argv;

    RTPSession *                    mRTPSession             = new RTPSession();
    RTPSessionParams *              mRTPSessionParams       = new RTPSessionParams();
    RTPUDPv4TransmissionParams *    mRTPTransmissionParams  = new RTPUDPv4TransmissionParams();

    uint16_t destport = 8000;
#if (0)
    char * ipaddr = (char *)malloc(NI_MAXHOST);
    TEST_MALLOC(ipaddr);
    Get_IP_addr("eth0", ipaddr);
    uint32_t destip = inet_addr(ipaddr);
    if (destip == INADDR_NONE)
    {
        PRINT_ERR("Bad IP address specified.\r\n");
        return -1;
    }
#endif
    char * ipaddr = "192.168.12.200";
    destip = ntohl(destip);

    PRINTF(NORM, "PJRTP version : %s.\r\n",
           RTPLibraryVersion::GetVersion().GetVersionString().c_str());

    mRTPSessionParams->SetOwnTimestampUnit(1.0/8000.0);
    mRTPSessionParams->SetAcceptOwnPackets(true);

    mRTPTransmissionParams->SetPortbase(8000);

    int status = mRTPSession->Create(* mRTPSessionParams, mRTPTransmissionParams);
    if (status) { PRINT_ERR("%s\r\n", RTPGetErrorString(status).c_str()); }

    mRTPSession->SetDefaultPayloadType(MIXER_PAYLOAD);
    mRTPSession->SetDefaultMark(false);
    mRTPSession->SetDefaultTimestampIncrement(RTP_P_FACTOR);

    //RTPTransmissionInfo * mETI = (RTPTransmissionInfo *) mRTPSession->GetTransmissionInfo();

    RTPIPv4Address addr(destip, destport);
    status = mRTPSession->AddDestination(addr);
    if (status) { PRINT_ERR("%s\r\n", RTPGetErrorString(status).c_str()); }

    uint8_t silencebuffer[RTP_P_FACTOR];
    for (int i = 0 ; i < RTP_P_FACTOR ; i++)
        silencebuffer[i] = i;

    for (int i = 1 ; i <= PCKGS_TO_SEND ; i++)
    {
        PRINTF(CYN, "Sending packet %d/%d\n", i, PCKGS_TO_SEND);
        status = mRTPSession->SendPacket(silencebuffer, RTP_P_FACTOR);
        if (status) { PRINT_ERR("%s\r\n", RTPGetErrorString(status).c_str()); }
        bool dataavailable=false;
        do {
            //Select received data
            mRTPSession->BeginDataAccess();
            PRINTF(GRN, "Receive RTP packages.\r\n");
            if (mRTPSession->GotoFirstSourceWithData())
            {
                do
                {
                    RTPPacket *packet;
                    while ((packet = mRTPSession->GetNextPacket()) != NULL)
                    {
#if (1)
                        PRINTF(MAG, "Got packet with extended sequence number %i from SSRC %i.\r\n",
                               packet->GetExtendedSequenceNumber(), packet->GetSSRC());
#else
                        PRINTF(CYN, "Package content :\r\n");
                        Print_hex_data  (
                                        (void *) packet->GetPayloadData(), /* data to print out */
                                        packet->GetPayloadLength(), /* how many bytes to print */
                                        __FUNCTION__, /* usually __FUNCTION__ */
                                        __LINE__ /* line number */
                                        );
#endif
                        mRTPSession->DeletePacket(packet);
                    }
                } while (mRTPSession->GotoNextSourceWithData());
            }
            mRTPSession->EndDataAccess();
            //clear ALL wait objects!!!
            RTPTime delay(0,0);
            mRTPSession->Poll();
            mRTPSession->WaitForIncomingData(delay,&dataavailable);
        } while(dataavailable);
    }

    delete mRTPSession;
    delete mRTPSessionParams;
    delete mRTPTransmissionParams;
}
#endif

