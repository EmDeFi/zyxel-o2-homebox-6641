#
# Makefile for USB Class drivers
# (one step up from the misc category)
#

# __MTS__, RaynorChung: Ssupport USB LED   
EXTRA_CFLAGS += -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD) -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)

# [IreneWang@MSTC:TO2]$ Maintain the LPD printer queueName and deviceNode, 20130801
ifneq ($(strip $(BUILD_MSTC_TO2_GERMANY_CUSTOMIZATION)),)
EXTRA_CFLAGS += -DMSTC_TO2_GERMANY_CUSTOMIZATION
endif

ifneq ($(strip $(MTS_TELEFONICA_WWAN)),)
EXTRA_CFLAGS += -DMTS_TELEFONICA_WWAN
endif

# MSTC , rewrite "RaynorChung: Ssupport USB LED " to compiler flag by Andylin 20120521
ifneq ($(strip $(ENABLE_USB_STORAGE_LED)),)
EXTRA_CFLAGS += -DENABLE_USB_STORAGE_LED
endif

#/* MSTC, USB PORT SWAP by Andylin 20120509*/
ifneq ($(strip $(USB_PORT_SWAP)),)
EXTRA_CFLAGS += -DUSB_PORT_SWAP
endif

obj-$(CONFIG_USB_ACM)		+= cdc-acm.o
obj-$(CONFIG_USB_PRINTER)	+= usblp.o
obj-$(CONFIG_USB_WDM)		+= cdc-wdm.o
obj-$(CONFIG_USB_TMC)		+= usbtmc.o
