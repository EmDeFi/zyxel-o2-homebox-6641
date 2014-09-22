EXE = cmbs_tcx
dynamic: generic_exe_install

clean:
	rm -f cmbs_tcx


cmbs_tcx:
	cp cmbs_tcx_$(PROFILE)_save cmbs_tcx

CURR_DIR := $(shell pwd)
BUILD_DIR := $(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR := $(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common
