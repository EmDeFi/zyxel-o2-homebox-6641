# File: bcmdrivers/broadcom/net/enet/bcm963xx/Makefile
#
# Makefile for the BCM63xx Enet/switch driver
#

bcm_enet-objs += bcmenet.o
bcm_enet-objs += ethsw.o
bcm_enet-objs += bcmsw.o
bcm_enet-objs += ethsw_phy.o
bcm_enet-objs += ../shared/bcmproc.o
bcm_enet-objs += ../shared/bcmswaccess.o

ifeq ($(strip $(BRCM_CHIP)),6816)
bcm_enet-objs += eth6829.o
endif

ifneq ($(strip $(CONFIG_BCM_GMAC)),)
bcm_enet-objs += bcmgmac.o
endif

ifneq ($(strip $(CONFIG_BCM_ENET)),)
obj-$(CONFIG_BCM_ENET) = bcm_enet.o
endif

#-DDUMP_DATA  -- dump data
#-DUSE_BH     -- use bottom half
#-DUSE_PROC   -- display ethernet driver internal info
#-DUSE_EMAC   -- need MDC/MDIO functions
#-DBCM_ENET_RX_LOG   -- Enable rx debug log
#-DBCM_ENET_LOG      -- Enable debug log
#-DBCM_ENET_TX_LOG   -- Enable tx debug log
#-DBCM_ENET_LINK_LOG   -- Enable link debug log
#-DBCM_ENET_DEBUG_BUILD   -- Enable Debug Build
#-DRXCHANNEL_BYTE_RATE_LIMIT -- Enable rx byte rate limiting of a channel
#-DRXCHANNEL_PKT_RATE_LIMIT -- Enable rx pkt rate limiting of a channel
#-DVLAN_TAG_FFF_STRIP -- Enable Stripping of 0xFFF tagged packets
#-DUSE_DEFAULT_EGRESS_QUEUE -- Enable use of default egress queue
#-DSEPARATE_MAC_FOR_WAN_INTERFACES -- Enable separate MAC addresses for WAN ifs

ifneq ($(strip $(BUILD_HELLO)),)
# Enable support for HELLO Packet CMF
EXTRA_CFLAGS += -DSUPPORT_HELLO
endif

ifneq ($(strip $(BUILD_SWMDK)),)
EXTRA_CFLAGS += -DSUPPORT_SWMDK
endif

ifneq ($(strip $(BUILD_GPHY)),)
EXTRA_CFLAGS += -DMSTC_ENABLE_GPHY
endif

ifneq ($(strip $(BUILD_5G)),)
EXTRA_CFLAGS += -DBUILD_5G
endif

ifneq ($(strip $(BUILD_HPNA)),)
EXTRA_CFLAGS += -DBUILD_HPNA
endif

ifneq ($(strip $(CONFIG_BUILD_MSTC_EXTERNAL_HPNA)),)
EXTRA_CFLAGS += -DMSTC_EXTERNAL_HPNA
endif

ifneq ($(strip $(BUILD_MSTC_FW_WITH_DIFFERENT_HW)),)
EXTRA_CFLAGS += -DBUILD_MSTC_FW_WITH_DIFFERENT_HW
endif

ifneq ($(strip $(CONFIG_BUILD_MSTC_COMMON_PATCH)),)
EXTRA_CFLAGS += -DMSTC_COMMON_PATCH
endif

#KeyYang@MSTC Quqntenna RMT_QCSAPI Porting, 20111215
ifneq ($(strip $(CONFIG_QUANTENNA_WLAN_MII_PORT)),)
EXTRA_CFLAGS += -DQUANTENNA_WLAN_MII_PORT
endif
#End of KeyYang's Comment

ifneq ($(strip $(CONFIG_BUILD_MSTC_TO2_GERMANY_CUSTOMIZATION)),)
EXTRA_CFLAGS += -DMSTC_TO2_GERMANY_CUSTOMIZATION
endif

EXTRA_CFLAGS += -DUSE_BH
EXTRA_CFLAGS += -DUSE_VPORTS
EXTRA_CFLAGS += -DEXPORT_SYMTAB
EXTRA_CFLAGS += -Werror -Wfatal-errors
EXTRA_CFLAGS += -g
#EXTRA_CFLAGS +=  -DBCM_ENET_DEBUG_BUILD
#EXTRA_CFLAGS +=  -DBCM_ENET_RX_LOG
#EXTRA_CFLAGS +=  -DBCM_ENET_TX_LOG
EXTRA_CFLAGS +=  -DBCM_ENET_LOG
#EXTRA_CFLAGS +=  -DRXCHANNEL_BYTE_RATE_LIMIT
EXTRA_CFLAGS +=  -DRXCHANNEL_PKT_RATE_LIMIT
EXTRA_CFLAGS +=  -DVLAN_TAG_FFF_STRIP
EXTRA_CFLAGS +=  -DUSE_DEFAULT_EGRESS_QUEUE 
EXTRA_CFLAGS += -DSEPARATE_MAC_FOR_WAN_INTERFACES
EXTRA_CFLAGS += -DREPORT_HARDWARE_STATS
#EXTRA_CFLAGS += -DDUMP_DATA
#EXTRA_CFLAGS += -DNON_LINUX_BUILD

ifneq ($(strip $(BRCM_CONFIG_HIGH_RES_TIMERS)),)
EXTRA_CFLAGS += -DMOCA_HIGH_RES_TX
endif

ifneq ($(strip $(BRCM_SWITCH_SCHED_SP)),)
EXTRA_CFLAGS += -DBCM_SWITCH_SCHED_SP
endif
ifneq ($(strip $(BRCM_SWITCH_SCHED_WRR)),)
EXTRA_CFLAGS += -DBCM_SWITCH_SCHED_WRR
endif

EXTRA_CFLAGS += -I$(obj) -I$(obj)/../shared
EXTRA_CFLAGS += -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD)
EXTRA_CFLAGS += -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD)
EXTRA_CFLAGS += -I$(INC_BRCMBOARDPARMS_PATH)/$(BRCM_BOARD)
EXTRA_CFLAGS += -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)
EXTRA_CFLAGS += -I$(BRCMDRIVERS_DIR)/opensource/include/$(BRCM_BOARD)
ifeq ($(CONFIG_BCM_XTMRT_IMPL),1)
EXTRA_CFLAGS += -I$(BRCMDRIVERS_DIR)/opensource/net/xtmrt/impl3
else
EXTRA_CFLAGS += -I$(BRCMDRIVERS_DIR)/opensource/net/xtmrt/impl$(CONFIG_BCM_XTMRT_IMPL)
endif
EXTRA_CFLAGS += -I$(BRCMDRIVERS_DIR)/broadcom/char/fap/impl$(CONFIG_BCM_FAP_IMPL)
EXTRA_CFLAGS += -I$(BRCMDRIVERS_DIR)/broadcom/char/fap/impl$(CONFIG_BCM_FAP_IMPL)/4ke
EXTRA_CFLAGS += -I$(KERNEL_DIR)/net/core

-include $(TOPDIR)/Rules.make

clean:
	rm -f core *.o *.a *.s
