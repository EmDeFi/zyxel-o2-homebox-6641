ifeq ($(BUILD_STOLLMANN_CALLMNGR),)
$(error Please run "make PROFILE=<profile_name> callmngr" from your source code root tree)
endif

export PRJ_BASE := $(shell pwd)/callmngr
export RELEASE_REVISION := $(CALLMNGR_VERSION)
export CALLMNGR_BASE := $(PRJ_BASE)/$(RELEASE_REVISION)

all:
	cp -af $(CALLMNGR_BASE)/fs.install/* $(INSTALL_DIR)/
	@echo -e "The CallMngr+ setup successfully."

callmngr_install:

callmngr_cmsapi:

clean:

.PHONY: all clean
