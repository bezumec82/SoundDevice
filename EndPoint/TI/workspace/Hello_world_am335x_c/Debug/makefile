################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Hello_world_am335x_c

# Tool invocations
Hello_world_am335x_c: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	arm-poky-linux-gnueabi-gcc -L/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am335x/IDE_SUPPORT/sysroots/armv7ahf-neon-poky-linux-gnueabi/lib -L/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am335x/IDE_SUPPORT/sysroots/armv7ahf-neon-poky-linux-gnueabi/usr/lib -t --verbose --sysroot=/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am335x/IDE_SUPPORT/sysroots/armv7ahf-neon-poky-linux-gnueabi -o "Hello_world_am335x_c" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) Hello_world_am335x_c
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
