# Makefile to compile a single device-tree

TI_PTH			:= /media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI
AM335_PTH		:= ${TI_PTH}/am335x
TCH_PTH			:= ${TI_PTH}/gcc-linaro-7.2.1-2017.11-x86_64_arm-linux-gnueabihf/bin
KRNL_PTH		:= ${TI_PTH}/processor-sdk-linux
CC				:= ${TCH_PTH}/arm-linux-gnueabihf-
DTC_PTH 		:= ${KRNL_PTH}/scripts/dtc
DTS_PTH			:= ${TI_PTH}/DeviceTree
# Main .dts file without extension
MAIN_DTS_FILE	:= am335x-bone

# C preprocessor flags (for dts parsing)
DTS_CPP_FLAGS := -nostdinc -I include -undef -D__DTS__ -x assembler-with-cpp

# Targets
all: 	${MAIN_DTS_FILE}.dtb
		cp ${MAIN_DTS_FILE}.dtb /tftpboot/BBB/am335x-boneblack.dtb

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
clean:		clean_tmp
			rm -rf *.dtb
