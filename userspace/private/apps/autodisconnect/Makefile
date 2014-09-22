EXE = autodisconnect
all: dynamic


dynamic: generic_exe_install

clean:
	rm -f autodisconnect


autodisconnect:
	cp autodisconnect_$(PROFILE)_save autodisconnect

CURR_DIR := $(shell pwd)
BUILD_DIR := $(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR := $(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common
