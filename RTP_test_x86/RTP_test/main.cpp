#include <QCoreApplication>
#include "main.h"

using namespace jrtplib;

void Get_IP_addr(
        const char * iface_name,
        char * ret_val
        ) {
    struct ifaddrs *    ifaddr;
    struct ifaddrs *    ifa;
    int32_t             family;
    int32_t             retval;
    int32_t             n;
    char                IP_addr[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        FPRINTF;
        exit(EXIT_FAILURE);
    }

    /* Walk through linked list, maintaining head pointer so we can free list later */
    for (ifa = ifaddr, n = 0;
            ifa != NULL;
            ifa = ifa->ifa_next, n++) {

        if (ifa->ifa_addr == NULL)
            continue;
        family = ifa->ifa_addr->sa_family;
        /* For an AF_INET* interface address, display the address */
        if (family == AF_INET) {
            retval = getnameinfo(ifa->ifa_addr,
                    sizeof(struct sockaddr_in),
                    IP_addr,
                    NI_MAXHOST,
                    NULL,
                    0,
                    NI_NUMERICHOST
                    );
            if (retval != 0) {
                PRINT_ERR("'getnameinfo' failed: %s\n", gai_strerror(retval));
                return;
            }
#if DEBUG_IP
            PRINTF(GRN, "IP address of '%-5s' : %s.\r\n", ifa->ifa_name, IP_addr);
#endif
            if  (
            !memcmp (
                    iface_name,
                    ifa->ifa_name,
                    strlen(ifa->ifa_name)
                    )
                ) {
                freeifaddrs(ifaddr);
                strcpy(ret_val, IP_addr);
                return;
            }
        } //end if (family == AF_INET)
    } //end for ...

    PRINT_ERR("IP address for device '%s' didn't found.\r\n", iface_name);
    freeifaddrs(ifaddr);
    return; /* IP address didn't found */
}
/*----------------------------------------------------------------------------*/

#define RTP_P_FACTOR        160
#define MIXER_PAYLOAD       8
#define PCKGS_TO_SEND       10

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
    RTPSession *                    mRTPSession             = new RTPSession();
    RTPSessionParams *              mRTPSessionParams       = new RTPSessionParams();
    RTPUDPv4TransmissionParams *    mRTPTransmissionParams  = new RTPUDPv4TransmissionParams();

    uint16_t destport = 8000;
    char * ipaddr = (char *)malloc(NI_MAXHOST);
    TEST_MALLOC(ipaddr);
    Get_IP_addr("eth0", ipaddr);
    uint32_t destip = inet_addr(ipaddr);
    if (destip == INADDR_NONE)
    {
        PRINT_ERR("Bad IP address specified.\r\n");
        return -1;
    }
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

    uint8_t silencebuffer[160];
    for (int i = 0 ; i < 160 ; i++)
        silencebuffer[i] = i;

    for (int i = 1 ; i <= PCKGS_TO_SEND ; i++)
    {
        PRINTF(CYN, "Sending packet %d/%d\n", i, PCKGS_TO_SEND);
        status = mRTPSession->SendPacket(silencebuffer, 160);
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
                        std::cout << "Got packet with extended sequence number "
                                  << packet->GetExtendedSequenceNumber()
                                  << " from SSRC " << packet->GetSSRC()
                                  << std::endl;
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
    //a.exec();
}
