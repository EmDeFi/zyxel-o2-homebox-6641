#
# Makefile for USB Core files and filesystem
#

usbcore-objs	:= usb.o hub.o hcd.o urb.o message.o driver.o \
			config.o file.o buffer.o sysfs.o endpoint.o \
			devio.o notify.o generic.o quirks.o

ifeq ($(CONFIG_PCI),y)
	usbcore-objs	+= hcd-pci.o
endif

ifeq ($(CONFIG_USB_DEVICEFS),y)
	usbcore-objs	+= inode.o devices.o
endif

obj-$(CONFIG_USB)	+= usbcore.o

ifeq ($(CONFIG_USB_DEBUG),y)
EXTRA_CFLAGS += -DDEBUG
endif

# MSTC , rewrite "RaynorChung: Ssupport USB LED " to compiler flag by Andylin 20120521
ifneq ($(strip $(ENABLE_USB_STORAGE_LED)),)
EXTRA_CFLAGS += -DENABLE_USB_STORAGE_LED
endif

ifneq ($(strip $(CONFIG_TELEFONICA)),)
EXTRA_CFLAGS += -DTELEFONICA
endif

ifneq ($(strip $(CONFIG_BUILD_WWAN)),)
EXTRA_CFLAGS += -DMTS_TELEFONICA_WWAN
endif
