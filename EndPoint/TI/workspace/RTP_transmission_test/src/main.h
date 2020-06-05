#ifndef __MAIN_H__
#define __MAIN_H__

#include <iostream>

#include "jrtplib3/rtpsession.h"
#include "jrtplib3/rtpudpv4transmitter.h"
#include "jrtplib3/rtpipv4address.h"
#include "jrtplib3/rtpsessionparams.h"
#include "jrtplib3/rtperrors.h"
#include "jrtplib3/rtplibraryversion.h"
#include "jrtplib3/rtppacket.h"

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

#include "IP_spplmntl.h"
#define TRUE        1
#define FALSE       0

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
                    printf(color"%-20.20s %-20.20s #%-5i: " text "" NORM,   \
                    __FILENAME__, __FUNCTION__, __LINE__ , ##__VA_ARGS__);  \

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
#define TEST_MALLOC(ptr) \
        if (!ptr){ \
            fprintf(stderr, RED"%s : malloc.\r\n" \
                    "'errno' : %i. ERROR : %s\n" NORM, __FUNCTION__, errno, strerror(errno)); \
           exit(EXIT_FAILURE); \
        } \


#define DEBUG_IP        TRUE

#endif /* __MAIN_H__ */
