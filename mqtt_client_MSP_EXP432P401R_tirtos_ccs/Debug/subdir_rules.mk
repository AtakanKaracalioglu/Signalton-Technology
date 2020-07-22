################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs920/ccs/tools/compiler/ti-cgt-arm_18.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/gokce/workspace_cc3120/mqtt_client_MSP_EXP432P401R_tirtos_ccs" --include_path="C:/ti/simplelink_sdk_wifi_plugin_2_40_00_22/source" --include_path="C:/ti/simplelink_msp432p4_sdk_3_40_00_05/source/third_party/CMSIS/Include" --include_path="C:/ti/simplelink_sdk_wifi_plugin_2_40_00_22/source/third_party/mbedtls/include/" --include_path="C:/ti/simplelink_msp432p4_sdk_3_40_00_05/source/ti/posix/ccs" --include_path="C:/ti/ccs920/ccs/tools/compiler/ti-cgt-arm_18.12.3.LTS/include" --advice:power=none --define=__MSP432P401R__ --define=DeviceFamily_MSP432P401x -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


