#ifndef __MAIN_H__
#define __MAIN_H__

#include <iostream>
#include "stdint.h"
#include <fcntl.h>
#include <vector>
#include <unordered_map>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <unistd.h>
#include <bitset>

using namespace std;

#include "ssc_class.h"

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

/* Additional line */
#define PRINTF_NL(color, text, ...)                                         \
                    printf(color"%49s " text "" NORM,"", ##__VA_ARGS__);

#define PRINT_ERR(text, ...)                                            \
        printf(RED"%-20.20s %-20.20s #%-5i: ERROR: " text "" NORM,      \
               __FILENAME__, __FUNCTION__, __LINE__,                    \
            ##__VA_ARGS__);

#define PRINT_WARN(text, ...)                                           \
        printf(YEL"%-20.20s %-20.20s #%-5i: WARNING: " text "" NORM,    \
               __FILENAME__, __FUNCTION__, __LINE__,                    \
            ##__VA_ARGS__);

#define FPRINTF                                                         \
        fprintf(stderr, RED"%-20.20s %-20.20s #%-5i: ERROR: '%s'.\r\n"  \
                    "%49s errno : %i.\r\n" NORM,                        \
                    __FILENAME__, __FUNCTION__, __LINE__,               \
                    strerror(errno) , "", errno );

#define FPRINTF_EXT(text, ...)                                          \
        fprintf(stderr, RED"%-20.20s %-20.20s #%-5i: ERROR: '%s'.\r\n"  \
                    "%49s errno : %i.\r\n"                              \
                    "%49s " text "" NORM,                               \
                    __FILENAME__, __FUNCTION__, __LINE__, strerror(errno),  \
                    "", errno ,                                         \
                    "", ##__VA_ARGS__ );


#define DISABLE     0
#define ENABLE      1

#define DEBUG_SSC           ENABLE
#define DEBUG_SSC_SEVERE    DISABLE

#define SAFE_VECTOR_TRAVERSING  ENABLE

#endif /* __MAIN_H__ */
