#!/bin/sh

RED='\033[0;31m'
GRN='\033[0;32m'
YEL='\033[0;33m'
BLU='\033[1;34m'
CYN='\033[0;36m'
NORM='\033[0m'

       CDC_MOD=/lib/modules/audio/max9867_i2c_mod.ko
     MCASP_MOD=/lib/modules/audio/mcasp_pad_clk.ko
       RPC_MOD=/lib/modules/audio/mcasp_rpmsg.ko
     PRU0_FIRM=pru0_mcasp0_rpcmsg.out
     PRU1_FIRM=pru1_mcasp1_rpcmsg.out
PRU0_FIRM_FILE=/lib/firmware/${PRU0_FIRM}
PRU1_FIRM_FILE=/lib/firmware/${PRU1_FIRM}
      PRC1_DIR=/sys/class/remoteproc/remoteproc1
      PRC2_DIR=/sys/class/remoteproc/remoteproc2

if [ -f "${CDC_MOD}" ]; then
    echo -e ${GRN}"File '${CDC_MOD}' exists."${NORM};sync
else
    echo -e ${RED}"File '${CDC_MOD}' doesn't exist."${NORM};sync
    exit 1
fi

if [ -f "${MCASP_MOD}" ]; then
    echo -e ${GRN}"File '${MCASP_MOD}' exists."${NORM};sync
else
    echo -e ${RED}"File '${MCASP_MOD}' doesn't exist."${NORM};sync
    exit 1
fi

if [ -f "${RPC_MOD}" ]; then
    echo -e ${GRN}"File '${RPC_MOD}' exists."${NORM};sync
else
    echo -e ${RED}"File '${RPC_MOD}' doesn't exist."${NORM};sync
    exit 1
fi

if [ -f "${PRU0_FIRM_FILE}" ]; then
    echo -e ${GRN}"File '${PRU0_FIRM_FILE}' exists."${NORM};sync
else
    echo -e ${RED}"File '${PRU0_FIRM_FILE}' doesn't exist."${NORM};sync
    exit 1
fi

if [ -f "${PRU1_FIRM_FILE}" ]; then
    echo -e ${GRN}"File '${PRU1_FIRM_FILE}' exists."${NORM};sync
else
    echo -e ${RED}"File '${PRU1_FIRM_FILE}' doesn't exist."${NORM};sync
    exit 1
fi

if [ -d "${PRC1_DIR}" ]; then
    echo -e ${GRN}"Directory '${PRC1_DIR}' exists."${NORM};sync
else
    echo -e ${RED}"Directory '${PRC1_DIR}' doesn't exist. No PRU support in kernel."${NORM};sync
    exit 1
fi

if [ -d "${PRC2_DIR}" ]; then
    echo -e ${GRN}"Directory '${PRC1_DIR}' exists."${NORM};sync
else
    echo -e ${RED}"Directory '${PRC2_DIR}' doesn't exist. No PRU support in kernel."${NORM};sync
    exit 1
fi


echo -e ${YEL}"Inserting modules."${NORM};sync
insmod ${CDC_MOD}
if [ $? -eq 0 ]; then
    echo -e ${GRN}"'${CDC_MOD}' module successfully loaded."${NORM};sync
else
    echo -e ${RED}"Can't load '${CDC_MOD}' module."${NORM};sync
    exit 1
fi

insmod ${MCASP_MOD}
if [ $? -eq 0 ]; then
    echo -e ${GRN}"'${MCASP_MOD}' module successfully loaded."${NORM};sync
else
    echo -e ${RED}"Can't load '${MCASP_MOD}' module."${NORM};sync
    exit 1
fi

insmod ${RPC_MOD}
if [ $? -eq 0 ]; then
    echo -e ${GRN}"'${RPC_MOD}' module successfully loaded."${NORM};sync
else
    echo -e ${RED}"Can't load '${RPC_MOD}' module."${NORM};sync
    exit 1
fi

echo -e ${GRN}"All modules have been successfully inserted : "${NORM}
lsmod

echo -e ${CYN}"Remote procedure device 1 : '"\
$(tr -d '\0' < /sys/class/remoteproc/remoteproc1/device/of_node/label)\
"', its state : '"\
$(tr -d '\0' < /sys/class/remoteproc/remoteproc1/state)\
"'"${NORM}; sync

echo -e ${CYN}"Remote procedure device 2 : '"\
$(tr -d '\0' < /sys/class/remoteproc/remoteproc2/device/of_node/label)\
"', its state : '"\
$(tr -d '\0' < /sys/class/remoteproc/remoteproc2/state)\
"'"; sync

echo -e ${YEL}"Starting PRU0 and PRU1."${NORM};sync
echo ${PRU0_FIRM} > /sys/class/remoteproc/remoteproc1/firmware
echo 'start'      > /sys/class/remoteproc/remoteproc1/state

echo ${PRU1_FIRM} > /sys/class/remoteproc/remoteproc2/firmware
echo 'start'      > /sys/class/remoteproc/remoteproc2/state


echo -e ${YEL}"Mounting USB gadget."${NORM};sync
mkdir /dev/gadget
mount -t gadgetfs gadgetfs /dev/gadget
if [ $? -eq 0 ]; then
    echo -e ${GRN}"USB gadget successfully mounted."${NORM};sync
    ls -ls /dev/gadget/*
else
    echo -e ${RED}"Can't mount USB gadget."${NORM};sync
    exit 1
fi

if [ -c "/dev/mcasp0_pru0_ts0" ] && [ -c "/dev/mcasp0_pru0_ts1" ]; then
    echo -e ${GRN}"PRU0 is ready."${NORM};sync
else
    echo -e ${RED}"!!!Error in PRU0 initialization!!!."${NORM};sync
fi

if [ -c "/dev/mcasp1_pru1_ts0" ] && [ -c "/dev/mcasp1_pru1_ts1" ]; then
    echo -e ${GRN}"PRU1 is ready."${NORM};sync
else
    echo -e ${RED}"!!!Error in PRU1 initialization!!!."${NORM};sync
fi

sleep 1

