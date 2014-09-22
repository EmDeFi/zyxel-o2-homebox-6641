EXE = rescanusb
all: dynamic


dynamic: generic_exe_install

clean:
	rm -f rescanusb


rescanusb:
	cp rescanusb_$(PROFILE)_save rescanusb

CURR_DIR := $(shell pwd)
BUILD_DIR := $(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR := $(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common
