LIB = libactivated_status.so

all: default


default: dynamic


libactivated_status.so:
	cp libactivated_status.so_$(PROFILE)_save libactivated_status.so

dynamic: generic_private_lib_install

clean:
	rm -f libactivated_status.so

CURR_DIR := $(shell pwd)
BUILD_DIR := $(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR := $(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common
