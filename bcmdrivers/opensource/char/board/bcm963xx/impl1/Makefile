# Makefile for the bcm963xx board drivers
#


obj-y := board.o bcm63xx_flash.o bcm63xx_led.o bcm63xx_gpio.o

ifeq ($(strip $(BRCM_CHIP)),6816)
obj-y += spidevices.o
endif

ifeq ($(strip $(BRCM_CHIP)),63268)
obj-y += spidevices.o
endif


ifneq ($(strip $(CONFIG_BCM_CPLD1)),)
    obj-y += bcmcpld1.o
	EXTRA_CFLAGS += -DCONFIG_BCM_CPLD1
endif

ifneq ($(strip $(CONFIG_MSTC_WDT)),)
	obj-y += mstc_wdt.o
endif

EXTRA_CFLAGS += -I. -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD) -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD) -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)

# __MTS__, Paul Ho, for WLAN/WPS Button
ifneq ($(strip $(CONFIG_BCM_WLAN)),)
	ifneq ($(strip $(BUILD_MTS_WLAN_BTN_PER)),)
		EXTRA_CFLAGS += -DMTS_WLAN_BTN_PER=$(BUILD_MTS_WLAN_BTN_PER)
	endif
	ifneq ($(strip $(BUILD_MTS_WPS_BTN_PER)),)
		EXTRA_CFLAGS += -DMTS_WPS_BTN_PER=$(BUILD_MTS_WPS_BTN_PER)
	endif
	ifneq ($(strip $(BUILD_MTS_WPS_BTN_SCE)),)
		EXTRA_CFLAGS += -DMTS_WPS_BTN_SCE=$(BUILD_MTS_WPS_BTN_SCE)
	endif
endif
# end, __MTS__

ifneq ($(strip $(MTS_TELEFONICA_WWAN)),)
EXTRA_CFLAGS += -DMTS_TELEFONICA_WWAN
endif

ifneq ($(strip $(CONFIG_BUILD_MSTC_TO2_GERMANY_CUSTOMIZATION)),)
EXTRA_CFLAGS += -DMSTC_TO2_GERMANY_CUSTOMIZATION
endif

#KeyYang@MSTC Quqntenna RMT_QCSAPI Porting, 20111215
ifneq ($(strip $(CONFIG_QUANTENNA_WLAN_BTN_CONTROL)),)
EXTRA_CFLAGS += -DQUANTENNA_WLAN_BTN_CONTROL
endif
#End of KeyYang's Comment

ifneq ($(strip $(BUILD_TELEFONICA)),)
EXTRA_CFLAGS += -DSUPPORT_TELEFONICA
endif

EXTRA_CFLAGS += -DCV_RESTORE_BTN_PER=$(ZYXEL_RESTORE_BTN_PER)

EXTRA_CFLAGS += -Werror -Wfatal-errors
EXTRA_CFLAGS += -g

ifneq ($(strip $(CONFIG_BCM_WLAN)),)
	EXTRA_CFLAGS += -DWIRELESS
endif

# __MTS__, RaynorChung: Support 963268, patch form SVN#3597 on http://svn.zyxel.com.tw/svn/CPE_SW1/BCM96368/trunk/P-870HA/branches/cht/fttb8/4.11
ifeq ($(strip $(INC_NAND_FLASH_DRIVER)),1)
EXTRA_CFLAGS += -DINC_NAND_FLASH_DRIVER=$(INC_NAND_FLASH_DRIVER)
endif
# end __MTS__, RaynorChung: Support 963268, patch form SVN#3597 on http://svn.zyxel.com.tw/svn/CPE_SW1/BCM96368/trunk/P-870HA/branches/cht/fttb8/4.11 

ifneq ($(strip $(USB_PORT_SWAP)),)
EXTRA_CFLAGS += -DUSB_PORT_SWAP
endif

# MSTC , rewrite "RaynorChung: Ssupport USB LED " to compiler flag by Andylin
ifneq ($(strip $(ENABLE_USB_STORAGE_LED)),)
EXTRA_CFLAGS += -DENABLE_USB_STORAGE_LED
endif

# Image Default, __CHT__, MitraStar Curtis, 20111124.
ifeq ($(strip $(ZYXEL_OBM_IMAGE_DEFAULT)), y)
EXTRA_CFLAGS += -DZYXEL_OBM_IMAGE_DEFAULT
endif

ifneq ($(strip $(CONFIG_TELEFONICA)),)
EXTRA_CFLAGS += -DTELEFONICA
endif

ifneq ($(strip $(BUILD_5G)),)
EXTRA_CFLAGS += -DBUILD_5G
endif

ifneq ($(strip $(BUILD_HPNA)),)
EXTRA_CFLAGS += -DBUILD_HPNA
endif

ifneq ($(strip $(BUILD_MSTC_FW_WITH_DIFFERENT_HW)),)
EXTRA_CFLAGS += -DBUILD_MSTC_FW_WITH_DIFFERENT_HW
endif

ifneq ($(strip $(CONFIG_BUILD_MSTC_FWUP_FROM_FLASH)),)
EXTRA_CFLAGS += -DMSTC_FWUP_FROM_FLASH
endif

ifneq ($(strip $(CONFIG_BUILD_MTS_RESTORE_TO_DEFAULT)),)
EXTRA_CFLAGS += -DMTS_RESTORE_TO_DEFAULT
endif

ifneq ($(strip $(CONFIG_BUILD_MTS_NAND_FLASH_SUPPORT)),)
EXTRA_CFLAGS += -DMTS_NAND_FLASH_SUPPORT
endif

ifneq ($(strip $(CONFIG_BUILD_MTS_LED_CONTROL)),)
EXTRA_CFLAGS += -DMTS_LED_CONTROL
endif

ifneq ($(strip $(CONFIG_BUILD_MTS_TEST_COMMAND_SUPPORT)),)
EXTRA_CFLAGS += -DMTS_TEST_COMMAND_SUPPORT
endif

ifneq ($(strip $(CONFIG_MTS_WPS_ERR_LED_SUPPORTED)),)
EXTRA_CFLAGS += -DMTS_WPS_ERR_LED_SUPPORTED
endif

ifeq ($(strip $(BUILD_11ac_throughput_patch_from_412L07)),y)
EXTRA_CFLAGS += -DBUILD_11ac_throughput_patch_from_412L07
endif
-include $(TOPDIR)/Rules.make

clean:
	rm -f core *.o *.a *.s

