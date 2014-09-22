LIB = libdect_mrd.so

all: default


default: dynamic


libdect_mrd.so:
	cp libdect_mrd.so_$(PROFILE)_save libdect_mrd.so

dynamic: generic_private_lib_install

clean:
	rm -f libdect_mrd.so

CURR_DIR := $(shell pwd)
BUILD_DIR := $(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR := $(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common
