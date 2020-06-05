# Makefile to compile a single device-tree

SAM_LNX_PTH := /media/constantine/RAID/SoundDevice/SoundDevice_endPoint/SAMA5D27_linux
SAM_TCH_PTH := ${SAM_LNX_PTH}/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabi
CC := ${SAM_TCH_PTH}/bin/arm-linux-gnueabi-
DTC_PTH := ${SAM_LNX_PTH}/linux-at91/scripts/dtc

# Main .dts file without extension
MAIN_DTS_FILE := at91-sama5d27_sound_device

# C preprocessor flags (for dts parsing)
DTS_CPP_FLAGS := -nostdinc -I include -undef -D__DTS__ -x assembler-with-cpp

# Targets
all: 	${MAIN_DTS_FILE}.dtb
		cp ${MAIN_DTS_FILE}.dtb /tftpboot/SoundDevice

# Device tree sources in the kernel deviate from the regular syntax, 
# by using the cpp preprocessor for includes and substitution.
${MAIN_DTS_FILE}.dts.tmp:   
							${CC}cpp -Wp,-MD,${MAIN_DTS_FILE}.dts.dep.tmp \
							${DTS_CPP_FLAGS} \
							-o ${MAIN_DTS_FILE}.dts.tmp ${MAIN_DTS_FILE}.dts


# Actual compilation using the temporal file as input
${MAIN_DTS_FILE}.dtb:   ${MAIN_DTS_FILE}.dts.tmp
						#${DTC_PTH}/\
						dtc -O dtb -o ${MAIN_DTS_FILE}.dtb -b 0 \
						-d ${MAIN_DTS_FILE}.dts.dep.tmp ${MAIN_DTS_FILE}.dts.tmp
						rm -rf *.tmp
# Clean only temporal files
clean_tmp:  
			rm -rf *.tmp

# Clean all temporal and outputs files
clean:      clean_tmp
			rm -rf *.dtb
