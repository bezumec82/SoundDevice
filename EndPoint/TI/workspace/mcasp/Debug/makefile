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
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: mcasp

# Tool invocations
mcasp: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	arm-poky-linux-gnueabi-g++ -L/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am335x/IDE_SUPPORT/sysroots/armv7ahf-neon-poky-linux-gnueabi/lib -L/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am335x/IDE_SUPPORT/sysroots/armv7ahf-neon-poky-linux-gnueabi/usr/lib -t --verbose --sysroot=/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am335x/IDE_SUPPORT/sysroots/armv7ahf-neon-poky-linux-gnueabi -o "mcasp" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(C_DEPS)$(CPP_DEPS) mcasp
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
