################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/constantine/ti/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-gcc" -c -mcpu=cortex-a8 -march=armv7-a -mtune=cortex-a8 -marm -mfloat-abi=hard -Dam3358 -DevmAM335x -DSOC_AM335x -I"/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/ccs_workspace/rtos_template_app_am335x_a8" -I"/home/constantine/ti/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/arm-none-eabi/include" -Og -g -gdwarf-3 -gstrict-dwarf -Wall -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

build-459552623:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-459552623-inproc

build-459552623-inproc: ../main.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"/home/constantine/ti/xdctools_3_50_07_20_core/xs" --xdcpath="/home/constantine/ti/bios_6_75_01_05/packages;/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am335x/RTOS_SDK/pdk_am335x_1_0_13/packages;" xdc.tools.configuro -o configPkg -t gnu.targets.arm.A8F -p ti.platforms.evmAM3359 -r release -c "/home/constantine/ti/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-459552623 ../main.cfg
configPkg/compiler.opt: build-459552623
configPkg/: build-459552623


