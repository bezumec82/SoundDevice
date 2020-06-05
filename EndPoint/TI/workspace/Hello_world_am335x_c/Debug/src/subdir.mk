################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Hello_world_am335x_c.c 

OBJS += \
./src/Hello_world_am335x_c.o 

C_DEPS += \
./src/Hello_world_am335x_c.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-poky-linux-gnueabi-gcc -I/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am335x/IDE_SUPPORT/sysroots/armv7ahf-neon-poky-linux-gnueabi/usr/include/c++/7.3.0/arm-poky-linux-gnueabi -I/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am335x/IDE_SUPPORT/sysroots/armv7ahf-neon-poky-linux-gnueabi/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -c -fmessage-length=0 -W -Wall -O2 -pipe -g -march=armv7-a -marm -mfpu=neon -mfloat-abi=hard -feliminate-unused-debug-symbols --sysroot=/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am335x/IDE_SUPPORT/sysroots/armv7ahf-neon-poky-linux-gnueabi -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


