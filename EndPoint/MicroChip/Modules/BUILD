export SAM_LNX_PTH=/media/constantine/RAID/SoundDevice/SoundDevice_endPoint/SAMA5D27_linux
export SAM_TCH_PTH=${SAM_LNX_PTH}/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabi
export CROSS_COMPILE=${SAM_TCH_PTH}/bin/arm-linux-gnueabi-
export CC=${CROSS_COMPILE}gcc

export MOD_PTH=${SAM_LNX_PTH}/Modules/RTP
export ORIG_PTH=${SAM_LNX_PTH}/RESERVE/uClinux2014R2-interlink/src/il/drv

export LINUX_DIR=${SAM_LNX_PTH}/linux-at91



### Compare ###
ls ${MOD_PTH}/hal/vmux.c
meld ${ORIG_PTH}/hal/vmux.c ${MOD_PTH}/hal/vmux.c &
