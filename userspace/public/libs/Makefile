#****************************************************************************
#
# Copyright (c) 2006-2008 Broadcom Corporation
#
# 
# 
# Unless you and Broadcom execute a separate written software license 
# agreement governing use of this software, this software is licensed 
# to you under the terms of the GNU General Public License version 2 
# (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php, 
# with the following added to such license:
# 
#    As a special exception, the copyright holders of this software give 
#    you permission to link this software with independent modules, and 
#    to copy and distribute the resulting executable under terms of your 
#    choice, provided that you also meet, for each linked independent 
#    module, the terms and conditions of the license of that module. 
#    An independent module is a module which is not derived from this
#    software.  The special exception does not apply to any modifications 
#    of the software.  
# 
# Not withstanding the above, under no circumstances may you combine 
# this software in any way with any other Broadcom software provided 
# under a license other than the GPL, without Broadcom's express prior 
# written consent. 
#
#
#****************************************************************************

#
# Add new libraries to BASE_SUBDIRS or OTHER_SUBDIRS.
# Do not set MODULAR_SUBDIRS here.  It is set dynamically based on
# "hint" files in the filesystem.
#
BASE_SUBDIRS := cms_boardctl cms_util cms_msg

# [IreneWang@MSTC:TO2]$ Partial configuration and restore+ Upgrade OpenSSL from 0.9.7f to 0.9.8zd, 20121227 + 20150115
ifneq ($(strip $(BUILD_MSTC_TO2_GERMANY_CUSTOMIZATION)),)
ifneq ($(strip $(BUILD_OPENSSL_98ZD)),)
OTHER_SUBDIRS := openssl-0.9.8zd libpcap xml_parse_lib activated
else
OTHER_SUBDIRS := openssl libpcap xml_parse_lib activated
endif
else
OTHER_SUBDIRS := openssl libpcap 
endif

#__MSTC__, TengChang, for ProFTPD with SFTP
ifeq ($(strip $(BUILD_MSTC_SFTP_SCP)),y)
OTHER_SUBDIRS+= zlib-1.2.7
endif

OTHER_SUBDIRS+= activated

SUBDIRS = $(BASE_SUBDIRS) $(OTHER_SUBDIRS) $(MODULAR_SUBDIRS)


# === this is the default target ===
# Need to add a level of indirection here so that MODULAR_SUBDIRS can be
# populated in make.common before we build.
#
all : make_all_subdirs


#
# Set our CommEngine directory (by splitting the pwd into two words
# at /userspace and taking the first word only).
# Then include the common defines under CommEngine.
#
CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

include $(BUILD_DIR)/make.common

ifneq ($(strip $(BUILD_VODSL)),)
include $(BUILD_DIR)/make.voice
endif


#
# We need to declare these directory names as phony targets,
# otherwise make will think the directories themselves are
# the targets and will never execute the cmds.
# See GNU Make, 4.6 
#
.PHONY : $(SUBDIRS)
make_all_subdirs : $(SUBDIRS)


clean: generic_subdir_clean



##############################################################
# 
# Rules for inidividual libs
# libs can be compiled or not depending on the profile (which
# is modified via make menuconfig)
#
##############################################################


$(BASE_SUBDIRS) $(MODULAR_SUBDIRS): 
	$(MAKE) -C $@

# DO_BUILD_OPENSSL is set in make.common
ifneq ($(strip $(DO_BUILD_OPENSSL)),)
openssl: sanity_check
	(tar xkfj openssl.tar.bz2 2> /dev/null || true)
	$(MAKE) -j 1 -C openssl all
	$(MAKE) -j 1 -C openssl install
else
openssl: sanity_check
	@echo "skipping openssl (not configured)"
endif

# [IreneWang@MSTC:TO2]$ Upgrade OpenSSL from 0.9.7f to 0.9.8zd, 20150115
ifneq ($(strip $(BUILD_OPENSSL_98ZD)),)
openssl-0.9.8zd: sanity_check
	(tar xkz -f openssl-0.9.8zd.tar.gz 2> /dev/null || true)
	$(MAKE) -j 1 -C openssl-0.9.8zd all
	$(MAKE) -j 1 -C openssl-0.9.8zd install
else
openssl-0.9.8zd: sanity_check
	@echo "skipping openssl-0.9.8zd (not configured)"
endif

ifneq ($(strip $(BUILD_LIBPCAP)),)
libpcap: sanity_check
	ln -sf libpcap libpcap-1.1.1 
	(tar xkz -f libpcap.tar.gz 2> /dev/null || true)
	rm -f libpcap-1.1.1
	$(MAKE) -C libpcap libpcap.a
else
libpcap: sanity_check
	@echo "skipping libpcap (not configured)"
endif
# [IreneWang@MSTC:TO2]$ Partial configuration and restore, 20121227
ifneq ($(strip $(BUILD_MSTC_TO2_GERMANY_CUSTOMIZATION)),)
xml_parse_lib: sanity_check
	$(MAKE) -j 1 -C xml_parse_lib all
else
xml_parse_lib: sanity_check
	@echo "skipping xml_parse_lib (not configured)"
endif

#__MSTC__, TengChang, for ProFTPD with SFTP
zlib-1.2.7: sanity_check
	$(MAKE) -C zlib-1.2.7
	$(MAKE) -C zlib-1.2.7 install

#WemJuin@MSTC:share library for actication status
activated:
	$(MAKE) -C $@ all

