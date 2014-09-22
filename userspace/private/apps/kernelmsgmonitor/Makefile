EXE = kernelmsgmonitor
all: dynamic


dynamic: generic_exe_install

clean:
	rm -f kernelmsgmonitor


kernelmsgmonitor:
	cp kernelmsgmonitor_$(PROFILE)_save kernelmsgmonitor

CURR_DIR := $(shell pwd)
BUILD_DIR := $(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR := $(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common
