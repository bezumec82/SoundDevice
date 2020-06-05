################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
clk_cfg.obj: ../clk_cfg.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: PRU Compiler'
	"/home/constantine/ti/ccsv8/tools/compiler/ti-cgt-pru_2.3.1/bin/clpru" -v3 --include_path="/home/constantine/ti/ccsv8/ccs_base/pru/include" --include_path="/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/ccs_workspace/pru_mcasp_rpcmsg" --include_path="/home/constantine/ti/ccsv8/tools/compiler/ti-cgt-pru_2.3.1/include" --include_path="/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am57x/SDK/example-applications/pru-icss-5.3.0/include" --include_path="/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am57x/SDK/example-applications/pru-icss-5.3.0/include/am572x_2_0" --define=pru0 --define=am3359 -g --diag_warning=225 --diag_wrap=off --display_error_number --endian=little --hardware_mac=on --preproc_with_compile --preproc_dependency="clk_cfg.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: PRU Compiler'
	"/home/constantine/ti/ccsv8/tools/compiler/ti-cgt-pru_2.3.1/bin/clpru" -v3 --include_path="/home/constantine/ti/ccsv8/ccs_base/pru/include" --include_path="/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/ccs_workspace/pru_mcasp_rpcmsg" --include_path="/home/constantine/ti/ccsv8/tools/compiler/ti-cgt-pru_2.3.1/include" --include_path="/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am57x/SDK/example-applications/pru-icss-5.3.0/include" --include_path="/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am57x/SDK/example-applications/pru-icss-5.3.0/include/am572x_2_0" --define=pru0 --define=am3359 -g --diag_warning=225 --diag_wrap=off --display_error_number --endian=little --hardware_mac=on --preproc_with_compile --preproc_dependency="main.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

mcasp_init.obj: ../mcasp_init.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: PRU Compiler'
	"/home/constantine/ti/ccsv8/tools/compiler/ti-cgt-pru_2.3.1/bin/clpru" -v3 --include_path="/home/constantine/ti/ccsv8/ccs_base/pru/include" --include_path="/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/ccs_workspace/pru_mcasp_rpcmsg" --include_path="/home/constantine/ti/ccsv8/tools/compiler/ti-cgt-pru_2.3.1/include" --include_path="/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am57x/SDK/example-applications/pru-icss-5.3.0/include" --include_path="/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am57x/SDK/example-applications/pru-icss-5.3.0/include/am572x_2_0" --define=pru0 --define=am3359 -g --diag_warning=225 --diag_wrap=off --display_error_number --endian=little --hardware_mac=on --preproc_with_compile --preproc_dependency="mcasp_init.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

pads_cfg_am335x.obj: ../pads_cfg_am335x.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: PRU Compiler'
	"/home/constantine/ti/ccsv8/tools/compiler/ti-cgt-pru_2.3.1/bin/clpru" -v3 --include_path="/home/constantine/ti/ccsv8/ccs_base/pru/include" --include_path="/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/ccs_workspace/pru_mcasp_rpcmsg" --include_path="/home/constantine/ti/ccsv8/tools/compiler/ti-cgt-pru_2.3.1/include" --include_path="/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am57x/SDK/example-applications/pru-icss-5.3.0/include" --include_path="/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am57x/SDK/example-applications/pru-icss-5.3.0/include/am572x_2_0" --define=pru0 --define=am3359 -g --diag_warning=225 --diag_wrap=off --display_error_number --endian=little --hardware_mac=on --preproc_with_compile --preproc_dependency="pads_cfg_am335x.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

pads_cfg_am5728.obj: ../pads_cfg_am5728.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: PRU Compiler'
	"/home/constantine/ti/ccsv8/tools/compiler/ti-cgt-pru_2.3.1/bin/clpru" -v3 --include_path="/home/constantine/ti/ccsv8/ccs_base/pru/include" --include_path="/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/ccs_workspace/pru_mcasp_rpcmsg" --include_path="/home/constantine/ti/ccsv8/tools/compiler/ti-cgt-pru_2.3.1/include" --include_path="/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am57x/SDK/example-applications/pru-icss-5.3.0/include" --include_path="/media/constantine/Work/SoundDevice/SoundDevice_endPoint/TI/am57x/SDK/example-applications/pru-icss-5.3.0/include/am572x_2_0" --define=pru0 --define=am3359 -g --diag_warning=225 --diag_wrap=off --display_error_number --endian=little --hardware_mac=on --preproc_with_compile --preproc_dependency="pads_cfg_am5728.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


