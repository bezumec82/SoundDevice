#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <cstdio>
#include <list>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <errno.h>
#include <sys/socket.h>
#include <syslog.h>
#include <netdb.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>      /* SIOCGIFFLAGS, SIOCADDRT */
#include <linux/if_link.h>
#include <arpa/inet.h>
#include <sys/un.h>         /* For 'struct sockaddr_un' */
#include <sys/socket.h>
#include <netinet/in.h>     /* For 'IPPROTO_IP' */


#include <QCoreApplication>
#include <QDebug>
#include <QTextStream>
#include <QFile>
#include <QIODevice>
#include <QTimer>
#include <QElapsedTimer>
#include <QThread>

#include <../../../../workspace/LIRA_cpp/src/g711.h>

#ifdef WIN32
    #include <windows.h>
#else
    #include <fcntl.h>
    #include <errno.h>
    #include <unistd.h>
    #include <poll.h>
#endif

#include <math.h>

#include "lirausb/liraProtocol.h"
#include "lirausb/usbEnum.h"

#define SIN_FREQ                1111.0
#define DISCRETIZATION          8000.0
#define SIN_PERIOD_WORDS        (double)(DISCRETIZATION / SIN_FREQ)
#define AUDIO_QUANTUM_MS        20
#define SEND_BUF_SIZE_WORDS     (int)   ((AUDIO_QUANTUM_MS * DISCRETIZATION) / 1000)
#define SEND_BUF_SIZE_BYTES     (int)   (SEND_BUF_SIZE_WORDS * (sizeof (uint8_t)))
#define SIN_AMPLITUDE           20000.0
#define AUDIOBUFS_NUM           2

#define MSG_ID_ALAW_DATA_2      0x1000
#define LI_GW_HDR_LNGTH         14
#define LI_GW_CRC_LNGTH         2
#define LI_GW_ZERO_LENGTH       (LI_GW_HDR_LNGTH + LI_GW_CRC_LNGTH)


class periodic_send : public QThread
{
    Q_OBJECT
    int                                mDevice    = -1;
    double                             sample_num = 0.0;
    std::list < uint8_t * >            out_buf_list;
    std::list < uint8_t * > ::iterator adata_buf_iter;

    liraEnumerator::USBDeviceDSC *     pUSBDeviceDSC;
    CliraProtocol                      LiraProto;
public:
    periodic_send();
    ~periodic_send();

    QTimer * qtimer;
public slots:
    void timer_slot();
};

/* Console output */
#define NORM                                        "\x1B[0m"  //normal
#define RED                                         "\x1B[31m" //red
#define GRN                                         "\x1B[32m" //green
#define YEL                                         "\x1B[33m" //yellow
#define BLU                                         "\x1B[34m" //blue
#define MAG                                         "\x1B[35m" //magenta
#define CYN                                         "\x1B[36m" //cyan
#define WHT                                         "\x1B[37m" //white
#define BOLD                                        "\e[1m"    //bold symbols
#define ITAL                                        "\e[3m"    //italic text
#define BLINK                                       "\e[5m"    //blink

#define __FILENAME__                                                        \
                    (strrchr(__FILE__, '/') ?                               \
                                            strrchr(__FILE__, '/') + 1 :    \
                                            __FILE__)

#define PRINTF(color, text, ...) \
        printf(color"%-20.20s %-20.20s #%-5i: " text "" NORM,               \
        __FILENAME__, __FUNCTION__, __LINE__ , ##__VA_ARGS__);              \


#define PRINT_ERR(text, ...)                                                \
        printf(RED"%-20.20s %-20.20s #%-5i: ERROR: " text "" NORM,          \
               __FILENAME__, __FUNCTION__, __LINE__,                        \
            ##__VA_ARGS__);

#define PRINT_WARN(text, ...)                                               \
        printf(YEL"%-20.20s %-20.20s #%-5i: WARNING: " text "" NORM,        \
               __FILENAME__, __FUNCTION__, __LINE__,                        \
            ##__VA_ARGS__);

#define FPRINTF                                                             \
        fprintf(stderr, RED"%-20.20s %-20.20s #%-5i: ERROR: '%s'.\r\n"      \
                    "%49s errno : %i.\r\n" NORM,                            \
                    __FILENAME__, __FUNCTION__, __LINE__,                   \
                    strerror(errno) , "", errno );

#define FPRINTF_EXT(text, ...)                                              \
        fprintf(stderr, RED"%-20.20s %-20.20s #%-5i: ERROR: '%s'.\r\n"      \
                    "%49s errno : %i.\r\n"                                  \
                    "%49s " text "" NORM,                                   \
                    __FILENAME__, __FUNCTION__, __LINE__, strerror(errno),  \
                    "", errno ,                                             \
                    "", ##__VA_ARGS__ );
#define TEST_MALLOC(ptr)                                    \
        if (!ptr){                                          \
            fprintf(stderr, RED"%s : malloc.\r\n"           \
                    "'errno' : %i. ERROR : %s\n" NORM,      \
                    __FUNCTION__, errno, strerror(errno));  \
           exit(-EXIT_FAILURE);                             \
        }                                                   \


#define DEBUG_IP        TRUE

#endif // MAIN_H
