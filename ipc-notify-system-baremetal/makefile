MCU_PLUS_SDK_PATH?=$(COM_TI_MCU_PLUS_SDK_AM64X_INSTALL_DIR)

$(info checking environment variables...)
ifeq ($(MCU_PLUS_SDK_PATH),)
 $(error MCU_PLUS_SDK_PATH is undefined)
endif

include $(MCU_PLUS_SDK_PATH)/imports.mak
include $(MCU_PLUS_SDK_PATH)/devconfig/devconfig.mak

ifeq ($(OS), Windows_NT)
	PYTHON=python
else
	PYTHON=python3
endif

PROFILE?=release

MULTI_CORE_BOOTIMAGE_PATH=$(abspath .)
MULTI_CORE_BOOTIMAGE_NAME:=ipc_notify_system.$(PROFILE).appimage
MULTI_CORE_BOOTIMAGE_NAME_SIGNED:=$(MULTI_CORE_BOOTIMAGE_NAME).signed
MULTI_CORE_BOOTIMAGE_NAME_XIP:=ipc_notify_system.$(PROFILE).appimage_xip
MULTI_CORE_BOOTIMAGE_NAME_HS:=$(MULTI_CORE_BOOTIMAGE_NAME).hs
MULTI_CORE_BOOTIMAGE_NAME_HS_FS:=$(MULTI_CORE_BOOTIMAGE_NAME).hs_fs

CORE_0=--script ../ipc-notify-r5fss0_0-baremetal/example.syscfg --context r5fss0-0 --output ../ipc-notify-r5fss0_0-baremetal/ti-arm-clang/generated
CORE_1=--script ../ipc-notify-r5fss0_1-baremetal/example.syscfg --context r5fss0-1 --output ../ipc-notify-r5fss0_1-baremetal/ti-arm-clang/generated
CORE_2=--script ../ipc-notify-r5fss1_0-baremetal/example.syscfg --context r5fss1-0 --output ../ipc-notify-r5fss1_0-baremetal/ti-arm-clang/generated
CORE_3=--script ../ipc-notify-r5fss1_1-baremetal/example.syscfg --context r5fss1-1 --output ../ipc-notify-r5fss1_1-baremetal/ti-arm-clang/generated
CORE_4=--script ../ipc-notify-m4fss0_0-baremetal/example.syscfg --context m4fss0-0 --output ../ipc-notify-m4fss0_0-baremetal/ti-arm-clang/generated

CORES = \
	$(CORE_4) \
    $(CORE_3) \
    $(CORE_2) \
    $(CORE_1) \
    $(CORE_0) \

all: syscfg
	$(MAKE) -C ../ipc-notify-r5fss0_0-baremetal/ti-arm-clang/ all
	$(MAKE) -C ../ipc-notify-r5fss0_1-baremetal/ti-arm-clang/ all
	$(MAKE) -C ../ipc-notify-r5fss1_0-baremetal/ti-arm-clang/ all
	$(MAKE) -C ../ipc-notify-r5fss1_1-baremetal/ti-arm-clang/ all
	$(MAKE) -C ../ipc-notify-m4fss0_0-baremetal/ti-arm-clang/ all

	$(MAKE) $(MULTI_CORE_BOOTIMAGE_NAME)
ifeq ($(DEVICE_TYPE), HS)
	$(MAKE) $(MULTI_CORE_BOOTIMAGE_NAME_HS)
endif

clean:
	$(MAKE) -C ../ipc-notify-r5fss0_0-baremetal/ti-arm-clang/ clean
	$(MAKE) -C ../ipc-notify-r5fss0_1-baremetal/ti-arm-clang/ clean
	$(MAKE) -C ../ipc-notify-r5fss1_0-baremetal/ti-arm-clang/ clean
	$(MAKE) -C ../ipc-notify-r5fss1_1-baremetal/ti-arm-clang/ clean
	$(MAKE) -C ../ipc-notify-m4fss0_0-baremetal/ti-arm-clang/ clean
	$(RM) $(MULTI_CORE_BOOTIMAGE_NAME)
	$(RM) $(MULTI_CORE_BOOTIMAGE_NAME_SIGNED)
	$(RM) $(MULTI_CORE_BOOTIMAGE_NAME_HS)
	$(RM) $(MULTI_CORE_BOOTIMAGE_NAME_HS_FS)
	$(RM) $(MULTI_CORE_BOOTIMAGE_NAME_XIP)

scrub:
	$(MAKE) -C ../ipc-notify-r5fss0_0-baremetal/ti-arm-clang/ scrub
	$(MAKE) -C ../ipc-notify-r5fss0_1-baremetal/ti-arm-clang/ scrub
	$(MAKE) -C ../ipc-notify-r5fss1_0-baremetal/ti-arm-clang/ scrub
	$(MAKE) -C ../ipc-notify-r5fss1_1-baremetal/ti-arm-clang/ scrub
	$(MAKE) -C ../ipc-notify-m4fss0_0-baremetal/ti-arm-clang/ scrub
	
ifeq ($(OS),Windows_NT)
	$(RM) \*.appimage
	$(RM) \*.appimage.signed
	$(RM) \*.appimage.hs*
	$(RM) \*.appimage_xip
else
	$(RM) *.appimage
	$(RM) *.appimage.signed
	$(RM) \*.appimage.hs*
	$(RM) *.appimage_xip
endif

syscfg:
	@echo Generating SysConfig files ...
	$(SYSCFG_NODE) $(SYSCFG_CLI_PATH)/dist/cli.js --product $(SYSCFG_SDKPRODUCT) $(CORES)

syscfg-gui:
	$(SYSCFG_NWJS) $(SYSCFG_PATH) --product $(SYSCFG_SDKPRODUCT) --device AM64x_beta --part Default --package ALV $(CORES)

#
# Generation of multi-core boot image which can be loaded by Secondary Boot Loader (SBL)
#
ifeq ($(OS),Windows_NT)
EXE_EXT=.exe
endif
BOOTIMAGE_TEMP_OUT_FILE=temp_stdout_$(PROFILE).txt

BOOTIMAGE_CORE_ID_a53ss0-0 = 0
BOOTIMAGE_CORE_ID_r5fss0-0 = 4
BOOTIMAGE_CORE_ID_r5fss0-1 = 5
BOOTIMAGE_CORE_ID_r5fss1-0 = 6
BOOTIMAGE_CORE_ID_r5fss1-1 = 7
BOOTIMAGE_CORE_ID_m4fss0-0 = 14
SBL_RUN_ADDRESS=0x70000000
SBL_DEV_ID=55

MULTI_CORE_IMAGE_GEN = $(SYSCFG_NODE) $(MCU_PLUS_SDK_PATH)/tools/boot/multicoreImageGen/multicoreImageGen.js
OUTRPRC_CMD = $(SYSCFG_NODE) $(MCU_PLUS_SDK_PATH)/tools/boot/out2rprc/elf2rprc.js
APP_IMAGE_SIGN_CMD = $(MCU_PLUS_SDK_PATH)/tools/boot/signing/appimage_x509_cert_gen.py

MULTI_CORE_APP_PARAMS = \
    ../ipc-notify-r5fss0_0-baremetal/ti-arm-clang/ipc_notify_benchmark.$(PROFILE).rprc@$(BOOTIMAGE_CORE_ID_r5fss0-0) \
	../ipc-notify-r5fss0_1-baremetal/ti-arm-clang/ipc_notify_benchmark.$(PROFILE).rprc@$(BOOTIMAGE_CORE_ID_r5fss0-1) \
    ../ipc-notify-r5fss1_0-baremetal/ti-arm-clang/ipc_notify_benchmark.$(PROFILE).rprc@$(BOOTIMAGE_CORE_ID_r5fss1-0) \
	../ipc-notify-r5fss1_1-baremetal/ti-arm-clang/ipc_notify_benchmark.$(PROFILE).rprc@$(BOOTIMAGE_CORE_ID_r5fss1-1) \
	../ipc-notify-m4fss0_0-baremetal/ti-arm-clang/ipc_notify_benchmark.$(PROFILE).rprc@$(BOOTIMAGE_CORE_ID_m4fss0-0) \

MULTI_CORE_APP_PARAMS_XIP = \
    ../ipc-notify-r5fss0_0-baremetal/ti-arm-clang/ipc_notify_benchmark.$(PROFILE).rprc_xip@$(BOOTIMAGE_CORE_ID_r5fss0-0) \
	../ipc-notify-r5fss0_1-baremetal/ti-arm-clang/ipc_notify_benchmark.$(PROFILE).rprc_xip@$(BOOTIMAGE_CORE_ID_r5fss0-1) \
    ../ipc-notify-r5fss1_0-baremetal/ti-arm-clang/ipc_notify_benchmark.$(PROFILE).rprc_xip@$(BOOTIMAGE_CORE_ID_r5fss1-0) \
    ../ipc-notify-r5fss1_1-baremetal/ti-arm-clang/ipc_notify_benchmark.$(PROFILE).rprc_xip@$(BOOTIMAGE_CORE_ID_r5fss1-1) \
    ../ipc-notify-m4fss0_0-baremetal/ti-arm-clang/ipc_notify_benchmark.$(PROFILE).rprc_xip@$(BOOTIMAGE_CORE_ID_m4fss0-0) \
	
MULTI_CORE_BOOTIMAGE_DEPENDENCY = \
    ../ipc-notify-r5fss0_0-baremetal/ti-arm-clang/ipc_notify_benchmark.$(PROFILE).rprc \
	../ipc-notify-r5fss0_1-baremetal/ti-arm-clang/ipc_notify_benchmark.$(PROFILE).rprc \
    ../ipc-notify-r5fss1_0-baremetal/ti-arm-clang/ipc_notify_benchmark.$(PROFILE).rprc \
    ../ipc-notify-r5fss1_1-baremetal/ti-arm-clang/ipc_notify_benchmark.$(PROFILE).rprc \
	../ipc-notify-m4fss0_0-baremetal/ti-arm-clang/ipc_notify_benchmark.$(PROFILE).rprc \
	
$(MULTI_CORE_BOOTIMAGE_DEPENDENCY):

$(MULTI_CORE_BOOTIMAGE_NAME): $(MULTI_CORE_BOOTIMAGE_DEPENDENCY)
	@echo  Boot multi-core image: $@ ...
	$(MULTI_CORE_IMAGE_GEN) --devID $(SBL_DEV_ID) --out $(MULTI_CORE_BOOTIMAGE_NAME) $(MULTI_CORE_APP_PARAMS) >> $(BOOTIMAGE_TEMP_OUT_FILE)
	$(MULTI_CORE_IMAGE_GEN) --devID $(SBL_DEV_ID) --out $(MULTI_CORE_BOOTIMAGE_NAME_XIP) $(MULTI_CORE_APP_PARAMS_XIP) >> $(BOOTIMAGE_TEMP_OUT_FILE)
	$(RM) $(BOOTIMAGE_TEMP_OUT_FILE)
	@echo  Boot multi-core image: $(MULTI_CORE_BOOTIMAGE_PATH)/$@ Done !!!
	@echo  .
	$(PYTHON) $(APP_IMAGE_SIGN_CMD) --bin $(MULTI_CORE_BOOTIMAGE_NAME) --authtype 1 --key $(APP_SIGNING_KEY) --output $(MULTI_CORE_BOOTIMAGE_NAME_HS_FS)
	@echo  Boot multi-core image: $(MULTI_CORE_BOOTIMAGE_PATH)/$(MULTI_CORE_BOOTIMAGE_NAME).hs_fs Done !!!
	@echo  .

$(MULTI_CORE_BOOTIMAGE_NAME_HS): $(MULTI_CORE_BOOTIMAGE_NAME)
ifeq ($(DEVICE_TYPE),HS)
# Sign the appimage using appimage signing script
ifeq ($(ENC_ENABLED),no)
	@echo Boot image signing: Encryption is disabled.
	$(PYTHON) $(APP_IMAGE_SIGN_CMD) --bin $(MULTI_CORE_BOOTIMAGE_NAME) --authtype 1 --key $(APP_SIGNING_KEY) --output $(MULTI_CORE_BOOTIMAGE_NAME_HS)
else
	@echo Boot image signing: Encryption is enabled.
	$(PYTHON) $(APP_IMAGE_SIGN_CMD) --bin $(MULTI_CORE_BOOTIMAGE_NAME) --authtype 1 --key $(APP_SIGNING_KEY) --enc y --enckey $(APP_ENCRYPTION_KEY) --output $(MULTI_CORE_BOOTIMAGE_NAME_HS)
	$(RM) $(MULTI_CORE_BOOTIMAGE_NAME)-enc
endif
	@echo  Boot multi-core image: $(MULTI_CORE_BOOTIMAGE_PATH)/$@ Done !!!
	@echo  Boot multi-core image: $(MULTI_CORE_BOOTIMAGE_PATH)/$(MULTI_CORE_BOOTIMAGE_NAME_HS) Done !!!
	@echo  .
endif
