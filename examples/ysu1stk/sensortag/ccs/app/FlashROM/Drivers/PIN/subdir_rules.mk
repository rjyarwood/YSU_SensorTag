################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Drivers/PIN/PINCC26XX.obj: C:/ti/tirtos_cc13xx_cc26xx_2_20_01_08/products/tidrivers_cc13xx_cc26xx_2_20_01_10/packages/ti/drivers/pin/PINCC26XX.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" --cmd_file="C:/ti/simplelink/ble_sdk_2_02_01_18/examples/ysu1stk/sensortag/ccs/app/../../iar/stack/../../../../../src/config/build_components.opt" --cmd_file="C:/ti/simplelink/ble_sdk_2_02_01_18/examples/ysu1stk/sensortag/ccs/app/../../iar/stack/build_config.opt" --cmd_file="C:/ti/simplelink/ble_sdk_2_02_01_18/examples/ysu1stk/sensortag/ccs/app/../../iar/stack/../../ccs/config/ccs_compiler_defines.bcfg"  -mv7M3 --code_state=16 --abi=eabi -me -O4 --opt_for_speed=0 --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/examples/ysu1stk/sensortag/ccs/app/utils" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/examples/ysu1stk/sensortag/ccs/app/prof" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/examples/ysu1stk/sensortag/ccs/app/prof" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/examples/ysu1stk/sensortag/ccs/app/ysu_sensortag" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/examples/ysu1stk/sensortag/ccs/app/app" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/examples/ysu1stk/sensortag/ccs/app" --include_path="C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/inc" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/icall/inc" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/profiles/roles/cc26xx" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/profiles/roles" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/profiles/dev_info" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/profiles/simple_profile/cc26xx" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/profiles/simple_profile" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/common/cc26xx" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/components/heapmgr" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/controller/cc26xx/inc" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/components/hal/src/target/_common" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/target" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/components/hal/src/target/_common/cc26xx" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/components/hal/src/inc" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/components/osal/src/inc" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/components/services/src/sdata" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/components/services/src/saddr" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/components/icall/src/inc" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/inc" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/rom" --include_path="C:/ti/tirtos_cc13xx_cc26xx_2_20_01_08/products/cc13xxware_2_04_02_17240" -g --c99 --gcc --define=YSU1 --define=GATT_TI_UUID_128_BIT --define=USER_DESCRIPTION --define=YSU --define=BOARD_DISPLAY_EXCLUDE_UART --define=CC1350_LAUNCHXL --define=CC13XX --define=Display_DISABLE_ALL --define=HEAPMGR_SIZE=0 --define=ICALL_MAX_NUM_ENTITIES=12 --define=ICALL_MAX_NUM_TASKS=7 --define=POWER_SAVING --define=USE_ICALL --define=xBOARD_DISPLAY_EXCLUDE_LCD --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL --diag_warning=225 --diag_suppress=48 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="Drivers/PIN/$(basename $(<F)).d_raw" --obj_directory="Drivers/PIN" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


