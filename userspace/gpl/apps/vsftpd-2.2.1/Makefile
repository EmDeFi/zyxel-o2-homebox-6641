# Makefile for systems with GNU tools
#CC 	=	gcc
INSTALL	=	install
IFLAGS  = -idirafter dummyinc
#CFLAGS = -g
CFLAGS	=	-O2 -Wall -W -Wshadow #-pedantic -Werror -Wconversion

#LIBS    =       `./vsf_findlibs.sh` -lcrypt
LIBS    =        -lcrypt
LIBS    +=       -lcms_msg $(CMS_COMMON_LIBS)
LINK	=	-Wl,-rpath,


ifneq ($(strip $(BUILD_MSTC_TO2_GERMANY_CUSTOMIZATION)),)
CFLAGS += -DMSTC_TO2_GERMANY_CUSTOMIZATION
endif


OBJS	=	main.o utility.o prelogin.o ftpcmdio.o postlogin.o privsock.o \
		tunables.o ftpdataio.o secbuf.o ls.o \
		postprivparent.o logging.o str.o netstr.o sysstr.o strlist.o \
    banner.o filestr.o parseconf.o secutil.o \
    ascii.o oneprocess.o twoprocess.o privops.o standalone.o hash.o \
    tcpwrap.o ipaddrparse.o access.o features.o readwrite.o opts.o \
    ssl.o sslslave.o ptracesandbox.o ftppolicy.o sysutil.o sysdeputil.o

all: vsftpd vsftp_install

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common

ALLOWED_INCLUDE_PATHS := -I$(BUILD_DIR)/userspace/public/include  \
                         -I$(BUILD_DIR)/userspace/public/include/$(OALDIR)

ALLOWED_LIB_DIRS := /lib:/lib/gpl:/lib/public

.c.o:
	$(CC) -c $*.c $(CFLAGS) $(IFLAGS)

vsftpd: $(OBJS)
	$(CC) -o vsftpd $(OBJS) $(LINK) $(LIBS) $(LDFLAGS) $(CMS_LIB_RPATH) $(CMS_LIB_PATH) $(CMS_LIB_PATH)


install:
	if [ -x /usr/local/sbin ]; then \
		$(INSTALL) -m 755 vsftpd /usr/local/sbin/vsftpd; \
	else \
		$(INSTALL) -m 755 vsftpd /usr/sbin/vsftpd; fi
	if [ -x /usr/local/man ]; then \
		$(INSTALL) -m 644 vsftpd.8 /usr/local/man/man8/vsftpd.8; \
		$(INSTALL) -m 644 vsftpd.conf.5 /usr/local/man/man5/vsftpd.conf.5; \
	elif [ -x /usr/share/man ]; then \
		$(INSTALL) -m 644 vsftpd.8 /usr/share/man/man8/vsftpd.8; \
		$(INSTALL) -m 644 vsftpd.conf.5 /usr/share/man/man5/vsftpd.conf.5; \
	else \
		$(INSTALL) -m 644 vsftpd.8 /usr/man/man8/vsftpd.8; \
		$(INSTALL) -m 644 vsftpd.conf.5 /usr/man/man5/vsftpd.conf.5; fi
	if [ -x /etc/xinetd.d ]; then \
		$(INSTALL) -m 644 xinetd.d/vsftpd /etc/xinetd.d/vsftpd; fi

clean: generic_clean
	rm -f $(INSTALL_DIR)/bin/vsftpd

vsftp_install:
	install vsftpd.conf $(INSTALL_DIR)/etc
	install vsftpd $(INSTALL_DIR)/bin

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)
