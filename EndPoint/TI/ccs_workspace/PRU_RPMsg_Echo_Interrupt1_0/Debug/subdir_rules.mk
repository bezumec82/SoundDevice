################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: PRU Compiler'
	"" -v3 --include_path="/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am57x/SDK/example-applications/pru-icss-5.3.0/include/am572x_2_0" --include_path="/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am57x/SDK/example-applications/pru-icss-5.3.0/include" --include_path="/media/constantine/Work/LEMZ/SoundDevice/EndPoint/TI/ccs_workspace/PRU_RPMsg_Echo_Interrupt1_0" --include_path="/include" --include_path="/home/constantine/ti/ccs1010/ccs/ccs_base/pru/include" --define=pru0 --define=am4379 --define=icss1 --define=am5728 -g --diag_warning=225 --diag_wrap=off --display_error_number --endian=little --hardware_mac=on --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


