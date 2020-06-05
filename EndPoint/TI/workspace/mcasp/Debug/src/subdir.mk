################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/GPIO_spplmntl.cpp \
../src/main.cpp \
../src/mcasp_class.cpp 

OBJS += \
./src/GPIO_spplmntl.o \
./src/main.o \
./src/mcasp_class.o 

CPP_DEPS += \
./src/GPIO_spplmntl.d \
./src/main.d \
./src/mcasp_class.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-poky-linux-gnueabi-g++ -std=c++0x -I/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am335x/IDE_SUPPORT/sysroots/armv7ahf-neon-poky-linux-gnueabi/usr/include -I/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am335x/IDE_SUPPORT/sysroots/armv7ahf-neon-poky-linux-gnueabi/usr/include/c++/7.3.0 -I/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am335x/IDE_SUPPORT/sysroots/armv7ahf-neon-poky-linux-gnueabi/usr/include/c++/7.3.0/arm-poky-linux-gnueabi -O0 -g3 -Wall -c -fmessage-length=0 -W -Wall -O2 -pipe -g -march=armv7-a -marm -mfpu=neon -mfloat-abi=hard -feliminate-unused-debug-symbols --sysroot=/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am335x/IDE_SUPPORT/sysroots/armv7ahf-neon-poky-linux-gnueabi -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


