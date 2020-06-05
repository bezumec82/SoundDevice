#!/bin/sh

RED='\033[0;31m'
GRN='\033[0;32m'
YEL='\033[0;33m'
BLU='\033[1;34m'
CYN='\033[0;36m'
NORM='\033[0m'


if [ ! -f /lib/ld-linux.so.3 ] 
then
    echo -e ${RED}"Creating adequate link to dynamic linker"${NORM}
    ln -s /lib/ld-linux-armhf.so.3 /lib/ld-linux.so.3
else
    echo -e ${GRN}"Link to dynamic linker exists"${NORM}
fi



