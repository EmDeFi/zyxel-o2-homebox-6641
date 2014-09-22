
ifeq ($(CONFIG_MIPS_BRCM),y)

# Linux
obj-y		+= boardparms.o

ifneq ($(strip $(VOXXXLOAD)),)
obj-y		+= boardparms_voice.o
endif

ifeq ($(strip $(BUILD_MSTC_FW_WITH_DIFFERENT_HW)), y)
EXTRA_CFLAGS += -DBUILD_MSTC_FW_WITH_DIFFERENT_HW
endif

ifeq ($(strip $(BUILD_MSTC_TO2_GERMANY_CUSTOMIZATION)),y)
EXTRA_CFLAGS += -DMSTC_TO2_GERMANY_CUSTOMIZATION
endif

ifeq ($(strip $(BUILD_HPNA)), y)
EXTRA_CFLAGS += -DBUILD_HPNA
endif

ifeq ($(strip $(BUILD_5G)), y)
EXTRA_CFLAGS += -DBUILD_5G
endif

ifeq ($(strip $(BUILD_MSTC_FWUP_FROM_FLASH)), y)
EXTRA_CFLAGS += -DMSTC_FWUP_FROM_FLASH
endif

ifneq ($(strip $(CONFIG_BUILD_MTS_NAND_FLASH_SUPPORT)),)
EXTRA_CFLAGS += -DMTS_NAND_FLASH_SUPPORT
endif

ifneq ($(strip $(CONFIG_BUILD_MTS_LED_CONTROL)),)
EXTRA_CFLAGS += -DMTS_LED_CONTROL
endif

# Yi-Jun@MSTC, For voice board ID profiling support. #
ifneq ($(strip $(BRCM_VOICE_BOARD_ID)),)
EXTRA_CFLAGS += -DBRCM_VOICE_BOARD_ID=\"$(BRCM_VOICE_BOARD_ID)\"
endif

ifneq ($(strip $(CONFIG_MTS_WPS_ERR_LED_SUPPORTED)),)
EXTRA_CFLAGS += -DMTS_WPS_ERR_LED_SUPPORTED
endif

EXTRA_CFLAGS += -DCONFIG_BCM9$(BRCM_CHIP) -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)
-include $(TOPDIR)/Rules.make

endif


