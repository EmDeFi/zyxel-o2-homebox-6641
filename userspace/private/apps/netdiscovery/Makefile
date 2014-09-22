EXE = netdiscovery
all: dynamic


dynamic: generic_exe_install

clean:
	rm -f netdiscovery


netdiscovery:
	cp netdiscovery_$(PROFILE)_save netdiscovery

CURR_DIR := $(shell pwd)
BUILD_DIR := $(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR := $(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common
