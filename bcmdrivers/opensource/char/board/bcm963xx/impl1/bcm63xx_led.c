/*
    Copyright 2000-2011 Broadcom Corporation

    <:label-BRCM:2011:DUAL/GPL:standard
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License, version 2, as published by
    the Free Software Foundation (the "GPL").
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    
    A copy of the GPL is available at http://www.broadcom.com/licenses/GPLv2.php, or by
    writing to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
    
    :>
*/

/***************************************************************************
 * File Name  : bcm63xx_led.c
 *
 * Description: 
 *    This file contains bcm963xx board led control API functions. 
 *
 ***************************************************************************/
/* Includes. */
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/capability.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/bcm_assert_locks.h>
#include <asm/uaccess.h>

#include <bcm_map_part.h>
#include <board.h>
#include <boardparms.h>
#if 1  /* Jayden@MSTC for LED control*/
#include "bcm_intr.h"
uint8 LedBhOn[4]={0,1,0,0};
uint8 LedCtlOn[4]={33,3,0,0};
uint8 LedCtlOff[4]={33,0,0,0};
uint8 LanLedCtlFrq[4]={135,0,0,0};/*set to 0x87 lowest 3 bits=111= 6Hz*/
 UBOOL8 IsEthLEDOn=TRUE;
#endif
extern spinlock_t bcm_gpio_spinlock;
#if 1 // for Internet LED
extern int bcmxtmrt_dataforwarded(void);
UINT8 LedInternetOn = FALSE;
UINT8 LedInternetDataTimerOn = TRUE;/*Jayden added for disable Internet LED data transmission timer,MitraStar,20130910*/
#endif
#define k125ms              (HZ / 8)   // 125 ms
#if 1 /*For To2 LED behavior,MitraStar Jayden,20130117*/
/****************************************************************************
    Registers for pseudo PHY access
****************************************************************************/

#define PSEUDO_PHY_ADDR                                   0x1e

#define REG_PSEUDO_PHY_MII_REG16                          0x10

    #define REG_PPM_REG16_SWITCH_PAGE_NUMBER_SHIFT        8
    #define REG_PPM_REG16_MDIO_ENABLE                     0x01

#define REG_PSEUDO_PHY_MII_REG17                          0x11

    #define REG_PPM_REG17_REG_NUMBER_SHIFT                8
    #define REG_PPM_REG17_OP_DONE                         0x00
    #define REG_PPM_REG17_OP_WRITE                        0x01
    #define REG_PPM_REG17_OP_READ                         0x02

#define REG_PSEUDO_PHY_MII_REG24                          0x18
#define REG_PSEUDO_PHY_MII_REG25                          0x19
#define REG_PSEUDO_PHY_MII_REG26                          0x1a
#define REG_PSEUDO_PHY_MII_REG27                          0x1b

/****************************************************************************
    Switch SPI Interface
****************************************************************************/

#define BCM5325_SPI_CMD_LEN                 1
#define BCM5325_SPI_ADDR_LEN                1
#define BCM5325_SPI_PREPENDCNT              (BCM5325_SPI_CMD_LEN+BCM5325_SPI_ADDR_LEN)

/* 5325 SPI Status Register */
#define BCM5325_SPI_STS                     0xfe

/* 5325 SPI Status Register definition */
#define BCM5325_SPI_CMD_SPIF                0x80
#define BCM5325_SPI_CMD_RACK                0x20

/* 5325 Command Byte definition */
#define BCM5325_SPI_CMD_READ                0x00    /* bit 0 - Read/Write */
#define BCM5325_SPI_CMD_WRITE               0x01    /* bit 0 - Read/Write */
#define BCM5325_SPI_CHIPID_MASK             0x7     /* bit 3:1 - Chip ID */
#define BCM5325_SPI_CHIPID_SHIFT            1
#define BCM5325_SPI_CMD_NORMAL              0x60    /* bit 7:4 - Mode */
#define BCM5325_SPI_CMD_FAST                0x10    /* bit 4 - Mode */

#define kFastBlinkCount     0          /*250ms, blink frequency 2Hz*/
#define kBlinkCount 		  1         /*500ms,blink frequency 1Hz */
#define kSlowBlinkCount     3          /*1s,blink frequency 0.5Hz*/
#define kOneSecCount     3          /*1s,for Pwr led to count 1 sec*/
#define LedGreen 0
#define LedRed 1
#else
#define kFastBlinkCount     0          // 125ms
#define kSlowBlinkCount     1          // 250ms
#endif
#if 1 /*For To2 ETH LED blink,MitraStar Jayden,20130520*/
#define PHY_PLL_ENABLE_DELAY 1
#define PHY_PORT_ENABLE_DELAY 40
static uint32 ephy_forced_pwr_down_status = 0;
#if defined(CONFIG_BCM_ETH_PWRSAVE)
unsigned int  eth_auto_power_down_enabled = 1;
unsigned int  eth_eee_enabled = 1;
#else
unsigned int  eth_auto_power_down_enabled = 0;
unsigned int  eth_eee_enabled = 0;
#endif
#if (defined(CONFIG_BCM96816) || defined(CONFIG_BCM96818))
#define NUM_INT_EPHYS 0
#define NUM_INT_GPHYS 2
#define DEFAULT_BASE_PHYID 0
#elif defined(CONFIG_BCM963268)
#define NUM_INT_EPHYS 3
#define NUM_INT_GPHYS 1
#define DEFAULT_BASE_PHYID 1
#elif defined(CONFIG_BCM96828)
#define NUM_INT_EPHYS 2
#define NUM_INT_GPHYS 2
#define DEFAULT_BASE_PHYID 1
#else
#define NUM_INT_EPHYS 4
#define NUM_INT_GPHYS 0
#define DEFAULT_BASE_PHYID 1
#endif
#define NUM_INT_PHYS (NUM_INT_EPHYS + NUM_INT_GPHYS)
#if defined(CONFIG_BCM96368) || defined(CONFIG_BCM96362) || defined(CONFIG_BCM96328) || defined(CONFIG_BCM96318)

#if !defined(CONFIG_BCM_ETH_HWAPD_PWRSAVE)
static uint32 mdix_manual_swap = 0;
#endif

#elif (defined(CONFIG_BCM96816) || defined(CONFIG_BCM96818))

static uint32 gphy_pwr_dwn[NUM_INT_GPHYS] =   {GPHY_PWR_DOWN_0, GPHY_PWR_DOWN_1};
#define ROBOSWGPHYCTRL RoboswEphyCtrl
#undef PHY_PORT_ENABLE_DELAY
#define PHY_PORT_ENABLE_DELAY 300 // GPHY on 6816 need much more time to link when connecting one to the other

#elif defined(CONFIG_BCM963268)
static uint32 gphy_pwr_dwn[NUM_INT_GPHYS] =   {GPHY_LOW_PWR};
#define ROBOSWGPHYCTRL RoboswGphyCtrl

#elif defined(CONFIG_BCM96828)
static uint64 ephy_energy_det[NUM_INT_PHYS] = {1<<(INTERRUPT_ID_EPHY_ENERGY_0-INTERNAL_ISR_TABLE_OFFSET),
                                               1<<(INTERRUPT_ID_EPHY_ENERGY_1-INTERNAL_ISR_TABLE_OFFSET),
                                               1<<(INTERRUPT_ID_GPHY_ENERGY_0-INTERNAL_ISR_TABLE_OFFSET),
                                               1<<(INTERRUPT_ID_GPHY_ENERGY_1-INTERNAL_ISR_TABLE_OFFSET)};
static uint32 gphy_pwr_dwn[NUM_INT_GPHYS] =   {GPHY1_LOW_PWR, GPHY2_LOW_PWR};
#define ROBOSWGPHYCTRL RoboswGphyCtrl
#else
#error /* Add definitions for new chips */
#endif

#ifndef _BCMMII_H_
#define _BCMMII_H_



/****************************************************************************
    Internal PHY registers
****************************************************************************/

//#define BCM_PHY_ID_M                        0x1F
//#define IsExtPhyId(id)                      ((id & BCM_PHY_ID_M) >= 0x10)
#define BCM_EXT_6829                        0x80
#define IsExt6829(id)                       (((id) & BCM_EXT_6829) && ((id & 0xFF) != 0xFF))
//#define isPhyConnected(phy_id)              (phy_id != 0xFF)

#define MII_CONTROL                         0x00
    #define MII_CONTROL_POWER_DOWN          (1 << 11)
    #define MII_CONTROL_ISOLATE_MII         (1 << 10)
    #define MII_CONTROL_RESTART_AUTONEG     (1 << 9)

#define MII_ASR                             0x19
#define MII_INTERRUPT                       0x1a
#define MII_INTERRUPT_MASK                  0x1b
#define MII_AS2SR                           0x1b  // Aux Status 2 Shadow Register (53101)
    #define MII_AS2SR_APD_EN                (1 << 5)
#define MII_REGISTER_1C                     0x1c
    #define MII_1C_WRITE_ENABLE             (1 << 15)
    #define MII_1C_AUTO_POWER_DOWN_SV       (0x0a << 10)
        #define MII_1C_APD_COMPATIBILITY    (1 << 8)
        #define MII_1C_AUTO_POWER_DOWN      (1 << 5)
        #define MII_1C_SLEEP_TIMER_SEL      (1 << 4)
        #define MII_1C_WAKEUP_TIMER_SEL_84  (1 << 0)
#define MII_1C_SHADOW_REG_SEL_S         10
#define MII_1C_SHADOW_REG_SEL_M         0x1F
#define MII_1C_SHADOW_CLK_ALIGN_CTRL        0x3
    #define GTXCLK_DELAY_BYPASS_DISABLE     (1 << 9)
#define MII_1C_SHADOW_LED_CONTROL           0x9
    #define ACT_LINK_LED_ENABLE             (1 << 4)
#define MII_1C_EXTERNAL_CONTROL_1           0xB
    #define LOM_LED_MODE                    (1 << 2)
#define MII_BRCM_TEST                       0x1f
    #define MII_BRCM_TEST_SHADOW_ENABLE     0x0080
    #define MII_BRCM_TEST_SHADOW2_ENABLE    0x0004

/* Shadow register 0x18 access */
#define MII_REGISTER_18                     0x18
   #define MII_REG_18_SEL(_s)               (((_s) << 12) | 0x7)
   #define MII_REG_18_WR(_s,_v)             (((_s) == 7 ? 0x8000 : 0) | (_v) | (_s)) /* Shadow 111 bit[15] = 1 for write */      
      #define RGMII_RXD_TO_RXC_SKEW         (1 << 8) /* Based on 54616 but should be good for others */

#define RGMII_WIRESPEED_EN            (1 << 4) /* Based on 54612 but should be good for others */

#if (defined(CONFIG_BCM96816) || defined(CONFIG_BCM96818) || defined(CONFIG_BCM96828))
#define MII_DSP_COEFF_ADDR                  0x17
#define MII_DSP_COEFF_RW_PORT               0x15
#endif

/* MII Auxiliary Status Register */
/*
 * 0x8000 auto negotiation done
 * 0x0004 link up
 * 0x0700 1000 Mbps, FDX
 * 0x0600 1000 Mbps, HDX
 * 0x0500  100 Mbps, FDX
 * 0x0300  100 Mbps, HDX
 * 0x0200   10 Mpbs, FDX
 * 0x0100   10 Mbps, HDX
 */

#define MII_ASR_ANHCD_MASK 0x0700
#define MII_ASR_DONE(r) ((r & 0x8000) != 0)
#define MII_ASR_LINK(r) ((r & 0x0004) != 0)
#define MII_ASR_FDX(r)  (((r & 0x0700) == 0x0700) || ((r & 0x0700) == 0x0500) || ((r & 0x0700) == 0x0200))
#define MII_ASR_1000(r) (((r & 0x0700) == 0x0700) || ((r & 0x0700) == 0x0600))
#define MII_ASR_100(r)  (((r & 0x0700) == 0x0500) || ((r & 0x0700) == 0x0300))
#define MII_ASR_10(r)   (((r & 0x0700) == 0x0200) || ((r & 0x0700) == 0x0100))
#define MII_ASR_NOHCD(r) ((r & MII_ASR_ANHCD_MASK) == 0)
#if defined (CONFIG_BCM96368)
#define MII_AENGSR                          0x1c
#define MII_AENGSR_SPD                      0x2 
#define MII_AENGSR_DPLX                     0x1 
#endif

typedef struct {
  int lnk:1;
  int fdx:1;
  int spd1000:1;
  int spd100:1;
} PHY_STAT;

/* MII Interrupt register. */
#define MII_INTR_ENABLE                     0x4000
#define MII_INTR_FDX                        0x0008
#define MII_INTR_SPD                        0x0004
#define MII_INTR_LNK                        0x0002


/****************************************************************************
    External switch pseudo PHY: Page (0x00)
****************************************************************************/

#define PAGE_CONTROL                                      0x00

    #define REG_PORT_CTRL                                 0x00

        #define REG_PORT_NO_SPANNING_TREE                 0x00
        #define REG_PORT_STP_STATE_DISABLED               0x20
        #define REG_PORT_STP_STATE_BLOCKING               0x40
        #define REG_PORT_STP_STATE_LISTENING              0x60
        #define REG_PORT_STP_STATE_LEARNING               0x80
        #define REG_PORT_STP_STATE_FORWARDING             0xA0
        #define REG_PORT_TX_DISABLE                       0x02
        #define REG_PORT_RX_DISABLE                       0x01
        #define REG_PORT_CTRL_DISABLE                     0x03

        #define REG_PORT_STP_SHIFT                        5
        #define REG_PORT_STP_MASK                         (0x7 << REG_PORT_STP_SHIFT)
        
#if defined (CONFIG_BCM96328)
    #define EPHY_PORTS       5
    #define NUM_RGMII_PORTS  1
    #define RGMII_PORT_ID    4
#elif defined (CONFIG_BCM96362)
    #define EPHY_PORTS       6
    #define NUM_RGMII_PORTS  2
    #define RGMII_PORT_ID    4
#elif defined (CONFIG_BCM963268)
    #define EPHY_PORTS       8
    #define GPHY_PORT_ID     3
    #define GPHY_PORT_PHY_ID 4
	#define NUM_RGMII_PORTS  4
    #define RGMII_PORT_ID    4
/*__MTSC__, Delon Yu, Support HPNA*/
#ifdef BUILD_HPNA 
    #define HPNA_PORTS_ID   4
#endif
    /*KeyYang@MSTC add for Quantenna WiFi Module support, 20120412*/
    #define WIFI_RGMII_PORT  CONFIG_QUANTENNA_WLAN_MII_PORT
#elif defined (CONFIG_BCM96828)
    #define EPHY_PORTS       8
    #define NUM_RGMII_PORTS  4
    #define GPHY1_PORT_ID     2
    #define GPHY1_PORT_PHY_ID 3
    #define GPHY2_PORT_ID     3
    #define GPHY2_PORT_PHY_ID 4
    #define RGMII_PORT_ID    4
    #define EPON_PORT_ID     7
#elif defined (CONFIG_BCM96368)
    #define EPHY_PORTS       6
    #define NUM_RGMII_PORTS  2
    #define RGMII_PORT_ID    4
    #define SAR_PORT_ID      7
#elif defined (CONFIG_BCM96816)
    #define EPHY_PORTS       4
    #define NUM_RGMII_PORTS  2
    #define SERDES_PORT_ID   4
    #define MOCA_PORT_ID     5
    #define GPON_PORT_ID     7
    #define RGMII_PORT_ID    2
#elif defined (CONFIG_BCM96818) /* FIXME - AC */
    #define EPHY_PORTS       4
    #define NUM_RGMII_PORTS  4
    #define SERDES_PORT_ID   4
    #define GPON_PORT_ID     7
    #define RGMII_PORT_ID    2
    #define RGMII1_PORT_ID   2
    #define RGMII2_PORT_ID   3
    #define RGMII3_PORT_ID   5	
#elif defined (CONFIG_BCM96318)
    #define EPHY_PORTS       4
    #define NUM_RGMII_PORTS  1
    #define RGMII_PORT_ID    5
#endif

#define USB_PORT_ID     6
#define MIPS_PORT_ID    8

/*__MTSC__, Delon Yu, Support HPNA*/
#ifdef BUILD_HPNA 
#define CS3211_PHYID2 0xc5
#endif

#define BCM54610_PHYID2  0xBD63
#define BCM50612_PHYID2  0x5E60
#define BCMAC201_PHYID2  0xBC30
#define BCM54616_PHYID2  0x5D10
#define BCM_PHYID_M      0xFFF0

    #define REG_MII_PORT_CONTROL                          0x08

        #define REG_MII_PORT_CONTROL_RX_UCST_EN           0x10
        #define REG_MII_PORT_CONTROL_RX_MCST_EN           0x08
        #define REG_MII_PORT_CONTROL_RX_BCST_EN           0x04  

    #define REG_SWITCH_MODE                               0x0b

        #define REG_SWITCH_MODE_FRAME_MANAGE_MODE         0x01
        #define REG_SWITCH_MODE_SW_FWDG_EN                0x02
        #define REG_SWITCH_MODE_FLSH_GPON_EGRESS_Q        0x08
        #define BROADCAST_TO_ONLY_MIPS                    0x20

    #define REG_CONTROL_MII1_PORT_STATE_OVERRIDE          0x0e
        #define REG_CONTROL_MPSO_MII_SW_OVERRIDE          0x80
        #define REG_CONTROL_MPSO_REVERSE_MII              0x10
        #define REG_CONTROL_MPSO_LP_FLOW_CONTROL          0x08
        #define REG_CONTROL_MPSO_SPEED100                 0x04
        #define REG_CONTROL_MPSO_FDX                      0x02
        #define REG_CONTROL_MPSO_LINKPASS                 0x01

    #define REG_PORT_FORWARD                              0x21
        #define REG_PORT_FORWARD_MCST                     0x80
        #define REG_PORT_FORWARD_UCST                     0x40
        #define REG_PORT_FORWARD_IP_MCST                  0x01

    #define REG_PORT_ENABLE                               0x23

    #define REG_PROTECTED_PORT_MAP                        0x24 

    #define REG_WAN_PORT_MAP                              0x26
        #define EN_MAN_TO_WAN                             (1<<9)

    #define REG_PAUSE_CAPBILITY                           0x28
                
        #define REG_PAUSE_CAPBILITY_OVERRIDE              (1<<23)
        #define REG_PAUSE_CAPBILITY_MIPS_RX               (1<<17)
        #define REG_PAUSE_CAPBILITY_SAR_RX                (1<<16)
        #define REG_PAUSE_CAPBILITY_USB_RX                (1<<15)
        #define REG_PAUSE_CAPBILITY_GMII1_RX              (1<<14)
        #define REG_PAUSE_CAPBILITY_GMII0_RX              (1<<13)
        #define REG_PAUSE_CAPBILITY_EPHY3_RX              (1<<12)
        #define REG_PAUSE_CAPBILITY_EPHY2_RX              (1<<11)
        #define REG_PAUSE_CAPBILITY_EPHY1_RX              (1<<10)
        #define REG_PAUSE_CAPBILITY_EPHY0_RX              (1<<9)
        #define REG_PAUSE_CAPBILITY_MIPS_TX               (1<<8)
        #define REG_PAUSE_CAPBILITY_SAR_TX                (1<<7)
        #define REG_PAUSE_CAPBILITY_USB_TX                (1<<6)
        #define REG_PAUSE_CAPBILITY_GMII1_TX              (1<<5)
        #define REG_PAUSE_CAPBILITY_GMII0_TX              (1<<4)
        #define REG_PAUSE_CAPBILITY_EPHY3_TX              (1<<3)
        #define REG_PAUSE_CAPBILITY_EPHY2_TX              (1<<2)
        #define REG_PAUSE_CAPBILITY_EPHY1_TX              (1<<1)
        #define REG_PAUSE_CAPBILITY_EPHY0_TX              (1<<0)

    #define REG_UCST_LOOKUP_FAIL                          0x32 /* 5397 only */

    #define REG_MCST_LOOKUP_FAIL                          0x34 /* 5397 only */

    #define REG_IPMC_LOOKUP_FAIL                          0x36 /* 5397 only */

    #define REG_DISABLE_LEARNING                          0x3c

    #define REG_PORT_STATE                                0x58

        #define REG_PORT_STATE_OVERRIDE                   0x40
        #define REG_PORT_STATE_FLOWCTL                    0x30
        #define REG_PORT_STATE_100                        0x04
        #define REG_PORT_STATE_1000                       0x08
        #define REG_PORT_STATE_FDX                        0x02
        #define REG_PORT_STATE_LNK                        0x01
        #define LINK_OVERRIDE_1000FDX  (REG_PORT_STATE_OVERRIDE |  \
            REG_PORT_STATE_1000 | REG_PORT_STATE_FDX | REG_PORT_STATE_LNK)
    #define REG_RGMII_CTRL_P4                             0x64
    #define REG_RGMII_CTRL_P5                             0x65
    #define REG_RGMII_CTRL_P7                             0x67
        #define REG_RGMII_CTRL_ENABLE_GMII                0x80
        #define REG_RGMII_CTRL_DLL_IQQD                   0x04
        #define REG_RGMII_CTRL_DLL_RXC_BYPASS             0x02
        #define REG_RGMII_CTRL_TIMING_SEL                 0x01
        #define REG_RGMII_CTRL_ENABLE_RGMII_OVERRIDE      0x40 
        #define REG_RGMII_CTRL_MODE                       0x30
            #define REG_RGMII_CTRL_MODE_RGMII             0x00
            #define REG_RGMII_CTRL_MODE_RvMII             0x20
            #define REG_RGMII_CTRL_MODE_MII               0x10
            #define REG_RGMII_CTRL_MODE_GMII              0x30

    #define REG_RGMII_TIMING_P4                           0x6c
    #define REG_RGMII_TIMING_P5                           0x6d
        #define RGMII_EN_DELAY_WRITE                      0x80
        #define RGMII_DELAY_VAL_M                         0x7
        #define RGMII_DELAY_VAL_S                         4
            #define RGMII_DELAY_VAL2                      0x7
        #define RGMII_DELAY_SEL_M                         0xF
        #define RGMII_DELAY_SEL_S                         0
            #define RGMII_DELAY_CLK_GTXR                  0x9
            #define RGMII_DELAY_CLK_RXR                   0xb

    #define REG_SW_RESET                                  0x79
        #define REG_SW_RST                                0x80
        #define REG_EN_RESET_REG                          0x02
        #define REG_EN_RESET_SW_CORE                      0x01

    #define REG_FAST_AGING_CTRL                           0x88
        #define FAST_AGE_START_DONE                       0x80
        #define FAST_AGE_DYNAMIC                          0x02
        #define FAST_AGE_STATIC                           0x01
        #define FAST_AGE_PORT                             0x04
        #define FAST_AGE_VLAN                             0x08
        #define FAST_AGE_SPT                              0x10
    #define REG_FAST_AGING_PORT                           0x89

    #define REG_SWPKT_CTRL_USB                            0xa0
    #define REG_SWPKT_CTRL_GPON                           0xa4

    #define REG_IUDMA_CTRL                                0xa8

        #define REG_IUDMA_CTRL_USE_QUEUE_PRIO             0x0400
        #define REG_IUDMA_CTRL_USE_DESC_PRIO              0x0200
        #define REG_IUDMA_CTRL_TX_DMA_EN                  0x0100
        #define REG_IUDMA_CTRL_RX_DMA_EN                  0x0080
        #define REG_IUDMA_CTRL_PRIO_SEL                   0x0040
        #define REG_IUDMA_CTRL_TX_MII_TAG_DISABLE         0x1000		

    #define REG_IUDMA_QUEUE_CTRL                          0xac
        #define REG_IUDMA_Q_CTRL_RXQ_SEL_S                16
        #define REG_IUDMA_Q_CTRL_RXQ_SEL_M                0xFFFF
        #define REG_IUDMA_Q_CTRL_PRIO_TO_CH_M             0x3
        #define REG_IUDMA_Q_CTRL_TXQ_SEL_S                0
        #define REG_IUDMA_Q_CTRL_TXQ_SEL_M                0xFFF
        #define REG_IUDMA_Q_CTRL_CH_TO_PRIO_M             0x7

    #define REG_MDIO_CTRL_ADDR                            0xb0
        #define REG_MDIO_CTRL_WRITE                       (1 << 31)
        #define REG_MDIO_CTRL_READ                        (1 << 30)
        #define REG_MDIO_CTRL_EXT                         (1 << 16)
        #define REG_MDIO_CTRL_EXT_BIT(id)                 (IsExtPhyId(id)? REG_MDIO_CTRL_EXT: 0)
        #define REG_MDIO_CTRL_ID_SHIFT                    25
        #define REG_MDIO_CTRL_ID_MASK                     (0x1f << REG_MDIO_CTRL_ID_SHIFT)
        #define REG_MDIO_CTRL_ADDR_SHIFT                  20
        #define REG_MDIO_CTRL_ADDR_MASK                   (0x1f << REG_MDIO_CTRL_ADDR_SHIFT)

    #define REG_MDIO_DATA_ADDR                            0xb4

    #define REG_EEE_CTRL                                  0xc0
        #define REG_EEE_CTRL_ENABLE                       0x01

    #define REG_EEE_TW_SYS_TX_100                         0xd0
    #define REG_EEE_TW_SYS_TX_1000                        0xd2

/****************************************************************************
    External switch pseudo PHY: Page (0x02)
****************************************************************************/

#define PAGE_MANAGEMENT                                   0x02

    #define REG_GLOBAL_CONFIG                             0x00

        #define ENABLE_MII_PORT                           0x80
        #define IGMP_REDIRECTION_EN                       0x01
        #define RECEIVE_IGMP                              0x08
        #define RECEIVE_BPDU                              0x02
        #define GLOBAL_CFG_RESET_MIB                      0x01

    #define REG_BRCM_HDR_CTRL                             0x03

        #define BRCM_HDR_EN_GMII_PORT_5                   0x02
        #define BRCM_HDR_EN_IMP_PORT                      0x01

    #define REG_AGING_TIME_CTRL                           0x08

        #define REG_AGE_CHANGE                            0x100000
        #define REG_AGE_TIME_MASK                         0x0FFFFF

    #define REG_MIRROR_CAPTURE_CTRL                       0x10

        #define REG_MIRROR_ENABLE                         0x8000
        #define REG_CAPTURE_PORT_M                        0xF

    #define REG_MIRROR_INGRESS_CTRL                       0x12

        #define REG_INGRESS_MIRROR_M                      0x1FF

    #define REG_DEV_ID                                    0x30

/****************************************************************************
    ARL CTRL Regsiters: Page (0x04)
****************************************************************************/

#define PAGE_ARLCTRL                                      0x04

    #define REG_ARLCFG                                    0x00
        #define MULTIPORT_ADDR_EN_M                       0x1
        #define MULTIPORT_ADDR_EN_S                       4
    #define REG_MULTIPORT_ADDR1_LO                        0x10
    #define REG_MULTIPORT_ADDR1_HI                        0x14
    #define REG_MULTIPORT_VECTOR1                         0x18
    #define REG_MULTIPORT_ADDR2_LO                        0x20
    #define REG_MULTIPORT_ADDR2_HI                        0x24
    #define REG_MULTIPORT_VECTOR2                         0x28

/****************************************************************************
    ARL/VLAN Table Access: Page (0x05)
****************************************************************************/

#define PAGE_AVTBL_ACCESS                                 0x05

    #define REG_ARL_TBL_CTRL                              0x00
        #define ARL_TBL_CTRL_START_DONE                   0x80 
        #define ARL_TBL_CTRL_READ                         0x01
    #define REG_ARL_MAC_INDX_LO                           0x02
    #define REG_ARL_MAC_INDX_HI                           0x04
    #define REG_ARL_VLAN_INDX                             0x08
    #define REG_ARL_MAC_LO_ENTRY                          0x10
    #define REG_ARL_VID_MAC_HI_ENTRY                      0x14
    #define REG_ARL_DATA_ENTRY                            0x18
        #define ARL_DATA_ENTRY_VALID                      0x8000
        #define ARL_DATA_ENTRY_STATIC                     0x4000
    #define REG_ARL_SRCH_CTRL                             0x30
        #define ARL_SRCH_CTRL_START_DONE                  0x80 
        #define ARL_SRCH_CTRL_SR_VALID                    0x01
    #define REG_ARL_SRCH_ADDR                             0x32
    #define REG_ARL_SRCH_MAC_LO_ENTRY                     0x34
    #define REG_ARL_SRCH_VID_MAC_HI_ENTRY                 0x38
    #define REG_ARL_SRCH_DATA_ENTRY                       0x3C
    #define REG_VLAN_TBL_CTRL                             0x60
    #define REG_VLAN_TBL_INDX                             0x62
    #define REG_VLAN_TBL_ENTRY                            0x64

/* External switch 53115 and 53125 ARL search registers */
    #define REG_ARL_SRCH_CTRL_531xx                       0x50
    #define REG_ARL_SRCH_MAC_LO_ENTRY_531xx               0x60
    #define REG_ARL_SRCH_VID_MAC_HI_ENTRY_531xx           0x64
    #define REG_ARL_SRCH_DATA_ENTRY_531xx                 0x68
        #define ARL_DATA_ENTRY_VALID_531xx                0x10000
        #define ARL_DATA_ENTRY_STATIC_531xx               0x8000
    #define REG_ARL_SRCH_DATA_RESULT_DONE_531xx           0x78
    
/****************************************************************************
   Flow Control: Page (0x0A)
****************************************************************************/

#define PAGE_FLOW_CTRL                                    0x0A

#if (defined(CONFIG_BCM96816) || defined(CONFIG_BCM96818))
    #define REG_FC_DIAG_PORT_SEL                          0x00
    #define REG_FC_CTRL                                   0x0E
    #define REG_FC_PRIQ_HYST                              0x10
    #define REG_FC_PRIQ_PAUSE                             0x20
    #define REG_FC_PRIQ_DROP                              0x30
    #define REG_FC_PRIQ_TOTAL_HYST                        0x40
    #define REG_FC_PRIQ_TOTAL_PAUSE                       0x50
    #define REG_FC_PRIQ_TOTAL_DROP                        0x60
    #define REG_FC_PRIQ_LO_DROP                           0x70
    #define REG_FC_Q_MON_CNT                              0x90
    #define REG_FC_PEAK_Q_MON_CNT                         0xA0
    #define REG_PEAK_RXBUF_COUNT                          0xB0
#else
    #define REG_FC_DIAG_PORT_SEL                          0x00
    #define REG_FC_CTRL                                   0x0E
    #define REG_FC_PRIQ_HYST                              0x10
    #define REG_FC_PRIQ_PAUSE                             0x18
    #define REG_FC_PRIQ_DROP                              0x20
    #define REG_FC_PRIQ_TOTAL_HYST                        0x28
    #define REG_FC_PRIQ_TOTAL_PAUSE                       0x30
    #define REG_FC_PRIQ_TOTAL_DROP                        0x38
    #define REG_FC_Q_MON_CNT                              0x60
    #define REG_PEAK_RXBUF_COUNT                          0x78
#endif


/****************************************************************************
   External Switch Internal Phy: Page (0x10-0x14)
****************************************************************************/

#define PAGE_INTERNAL_PHY_MII                             0x10

/****************************************************************************
   MIB Counters: Page (0x20 to 0x28)
****************************************************************************/

#define PAGE_MIB_P0                                       0x20

    #define REG_MIB_P0_TXOCTETS                           0x00
    #define REG_MIB_P0_TXDROPS                            0x08
    #define REG_MIB_P0_TXBPKTS                            0x10
    #define REG_MIB_P0_TXMPKTS                            0x14
    #define REG_MIB_P0_TXUPKTS                            0x18  
    #define REG_MIB_P0_TXFRAMEINDISC                      0x34
    #define REG_MIB_P0_RXOCTETS                           0x44
    #define REG_MIB_P0_RXUNDERSIZEPKTS                    0x4C
    #define REG_MIB_P0_RXJABBERS                          0x70
    #define REG_MIB_P0_RXALIGNERRORS                      0x74
    #define REG_MIB_P0_RXFCSERRORS                        0x78
    #define REG_MIB_P0_RXDROPS                            0x84
    #define REG_MIB_P0_RXUPKTS                            0x88
    #define REG_MIB_P0_RXMPKTS                            0x8c
    #define REG_MIB_P0_RXBPKTS                            0x90
    #define REG_MIB_P0_RXFRAGMENTS                        0x98
    #define REG_MIB_P0_RXSYMBOLERRORS                     0xA0
    #define REG_MIB_P0_RXEXCESSSIZEDISC                   0x9C

    /* Define an offset for use for register reads of external switches */
    /* NOTE: The 5398 is the same as the itnernal Roboswitch, but on
       legacy designs can actually be an external switch. */
    #if (CONFIG_BCM_EXT_SWITCH_TYPE == 5398)
        #define REG_MIB_P0_EXTSWITCH_OFFSET               0x00
    #else
        #define REG_MIB_P0_EXTSWITCH_OFFSET               0x0C
    #endif
        
    
/****************************************************************************
   QOS : Page (0x30)
****************************************************************************/

#define PAGE_QOS                                          0x30

    #define REG_QOS_GLOBAL_CTRL                           0x00
#if defined (CONFIG_BCM96816) || defined (CONFIG_BCM96818)
        #define PORT_QOS_EN_M                             0x1
        #define PORT_QOS_EN_S                             7
        #define QOS_LAYER_SEL_M                           0x3
        #define QOS_LAYER_SEL_S                           3
        #define MIB_QOS_MONITOR_SET_M                     0x7
        #define MIB_QOS_MONITOR_SET_S                     0
#else
        #define PORT_QOS_EN_M                             0x1
        #define PORT_QOS_EN_S                             6
        #define QOS_LAYER_SEL_M                           0x3
        #define QOS_LAYER_SEL_S                           2
        #define MIB_QOS_MONITOR_SET_M                     0x3
        #define MIB_QOS_MONITOR_SET_S                     0
#endif
    #define REG_QOS_THRESHOLD_CTRL                        0x02
        #define AUTOSET_QOS_EN                            0x2000
    #define REG_QOS_8021P_EN                              0x04
    #define REG_QOS_8021P_PRIO_MAP                        0x10
    #define REG_QOS_PORT_PRIO_MAP_P0                      0x50
#if defined (CONFIG_BCM96816) || defined (CONFIG_BCM96818)
    #define REG_QOS_PRIO_TO_QID_SEL_BITS                  3
    #define REG_QOS_PRIO_TO_QID_SEL_M                     0x7
#else
    #define REG_QOS_PRIO_TO_QID_SEL_BITS                  2
    #define REG_QOS_PRIO_TO_QID_SEL_M                     0x3
#endif

    #define REG_QOS_DSCP_PRIO_MAP0LO                      0x30
    #define REG_QOS_DSCP_PRIO_MAP0HI                      0x34
    #define REG_QOS_DSCP_PRIO_MAP1LO                      0x36
    #define REG_QOS_DSCP_PRIO_MAP1HI                      0x38
    #define REG_QOS_DSCP_PRIO_MAP2LO                      0x3c
    #define REG_QOS_DSCP_PRIO_MAP2HI                      0x40
    #define REG_QOS_DSCP_PRIO_MAP3LO                      0x42
    #define REG_QOS_DSCP_PRIO_MAP3HI                      0x44

    #define REG_QOS_TXQ_CTRL                              0x80
#if defined (CONFIG_BCM96816) || defined (CONFIG_BCM96818)
        #define TXQ_CTRL_TXQ_MODE_M                       0x7 
        #define TXQ_CTRL_TXQ_MODE_S                       2 
        #define TXQ_CTRL_HQ_PREEMPT_M                     0x1
        #define TXQ_CTRL_HQ_PREEMPT_S                     5 
#else
        #define TXQ_CTRL_TXQ_MODE_M                       0x3 
        #define TXQ_CTRL_TXQ_MODE_S                       2 
        #define TXQ_CTRL_HQ_PREEMPT_M                     0x1
        #define TXQ_CTRL_HQ_PREEMPT_S                     4 
#endif
    #define REG_QOS_TXQ_WEIGHT_Q0                         0x81
    #define REG_QOS_VID_REMAP                             0xB0
    #define REG_QOS_VID_REMAP_CTRL                        0xD0

/****************************************************************************
    Port-Based VLAN Control: Page (0x31)
****************************************************************************/

#define PAGE_PORT_BASED_VLAN                              0x31

    #define REG_VLAN_CTRL_P0                              0x00
    #define REG_VLAN_CTRL_P1                              0x02
    #define REG_VLAN_CTRL_P2                              0x04
    #define REG_VLAN_CTRL_P3                              0x06
    #define REG_VLAN_CTRL_P4                              0x08
    #define REG_VLAN_CTRL_P5                              0x0a
    #define REG_VLAN_CTRL_P6                              0x0c
    #define REG_VLAN_CTRL_P7                              0x0e
    #define REG_VLAN_CTRL_P8                              0x10

/****************************************************************************
   802.1Q VLAN Control: Page (0x34)
****************************************************************************/

#define PAGE_8021Q_VLAN                                   0x34

    #define REG_VLAN_GLOBAL_8021Q                         0x00
        #define VLAN_EN_8021Q_M                           0x1
        #define VLAN_EN_8021Q_S                           7
        #define VLAN_IVL_SVL_M                            0x3
        #define VLAN_IVL_SVL_S                            5
        #define PRIO_TAG_FRAME_CTRL_M                     0x3
        #define PRIO_TAG_FRAME_CTRL_S                     0
    #define REG_VLAN_GLOBAL_CTRL1                         0x01
    #define REG_VLAN_GLOBAL_CTRL2                         0x02
    #define REG_VLAN_GLOBAL_CTRL3                         0x04
    #define REG_VLAN_GLOBAL_CTRL4                         0x06
    #define REG_VLAN_GLOBAL_CTRL5                         0x07
        #define TAG_STATUS_PRESERVE_M                     0x3
        #define TAG_STATUS_PRESERVE_S                     5
        #define TAG_STATUS_PRESERVE_01                    1
        #define TAG_STATUS_PRESERVE_10                    2
        #define VID_FFF_ENABLE_M                          0x1
        #define VID_FFF_ENABLE_S                          2
    #define REG_VLAN_8021P_REMAP                          0x0c
    #define REG_VLAN_DEFAULT_TAG                          0x10
        #define DEFAULT_TAG_PRIO_M                        0x7
        #define DEFAULT_TAG_PRIO_S                        13
        #define DEFAULT_TAG_CFI_M                         0x1
        #define DEFAULT_TAG_CFI_S                         12
        #define DEFAULT_TAG_VID_M                         0xFFF
        #define DEFAULT_TAG_VID_S                         0
    #define REG_VLAN_DOUBLE_TAG_TPID                      0x30
    #define REG_VLAN_ISP_PORT_SEL                         0x32

/****************************************************************************
   Jumbo Frame Control: Page (0x40)
****************************************************************************/

#define PAGE_JUMBO                                        0x40

    #define REG_JUMBO_PORT_MASK                           0x04
    #define REG_JUMBO_FRAME_SIZE                          0x08

    #define MAX_JUMBO_FRAME_SIZE                          2048

    // Defines for JUMBO register control
    #define ETHSWCTL_JUMBO_PORT_ALL                       9
    #define ETHSWCTL_JUMBO_PORT_MIPS                      8
    #define ETHSWCTL_JUMBO_PORT_GPON                      7
    #define ETHSWCTL_JUMBO_PORT_USB                       6
    #define ETHSWCTL_JUMBO_PORT_MOCA                      5
    #define ETHSWCTL_JUMBO_PORT_GPON_SERDES               4
    #define ETHSWCTL_JUMBO_PORT_GMII_2                    3
    #define ETHSWCTL_JUMBO_PORT_GMII_1                    2
    #define ETHSWCTL_JUMBO_PORT_GPHY_1                    1
    #define ETHSWCTL_JUMBO_PORT_GPHY_0                    0

    #define ETHSWCTL_JUMBO_PORT_MIPS_MASK                 0x0100
    #define ETHSWCTL_JUMBO_PORT_GPON_MASK                 0x0080
    #define ETHSWCTL_JUMBO_PORT_USB_MASK                  0x0040
    #define ETHSWCTL_JUMBO_PORT_MOCA_MASK                 0x0020
    #define ETHSWCTL_JUMBO_PORT_GPON_SERDES_MASK          0x0010
    #define ETHSWCTL_JUMBO_PORT_GMII_2_MASK               0x0008
    #define ETHSWCTL_JUMBO_PORT_GMII_1_MASK               0x0004
    #define ETHSWCTL_JUMBO_PORT_GPHY_1_MASK               0x0002
    #define ETHSWCTL_JUMBO_PORT_GPHY_0_MASK               0x0001
    #define ETHSWCTL_JUMBO_PORT_MASK_VAL                  0x01FF

/****************************************************************************
   Broadcast Storm Suprression Registers
****************************************************************************/

#define PAGE_BSS                                          0x41

    #define REG_BSS_IRC_CONFIG                            0x00
        #define IRC_CFG_XLENEN                            16
        #define IRC_CFG_PKT_MSK0_EXT_S                    17

    #define REG_BSS_RX_RATE_CTRL_P0                       0x10
        #define IRC_BKT0_RATE_CNT_M                       0xFF
        #define IRC_BKT0_RATE_CNT_S                       0
        #define IRC_BKT0_SIZE_M                           0x7
        #define IRC_BKT0_SIZE_S                           8
        #define IRC_BKT0_EN_M                             0x01
        #define IRC_BKT0_EN_S                             22

    #define REG_BSS_PKT_DROP_CNT_P0                       0x50

    #define REG_BSS_TX_RATE_CTRL_P0                       0x80
        #define ERC_RFSH_CNT_M                            0xFF
        #define ERC_RFSH_CNT_S                            0
        #define ERC_BKT_SIZE_M                            0x7
        #define ERC_BKT_SIZE_S                            8
        #define ERC_ERC_EN_M                              0x01
        #define ERC_ERC_EN_S                              11

/*Configure port state for HPAN module, KeyYang@MSTC 20120914*/
#if defined(BUILD_HPNA) && defined(MSTC_EXTERNAL_HPNA)
/****************************************************************************
   Control Reegister: Page (0x00)
****************************************************************************/

#define PAGE_CTRL										0x00
	/*	0x59: Port#1;	0x5A: Port#2;	0x5B: Port#3; 0x5C: Port#4; 0x5D: Port#5	*/
    #define REG_CTRL_OVERRIDE							0x59
#endif

/****************************************************************************
   EEE: Page (0x92)
****************************************************************************/

#define PAGE_EEE                                          0x92
    #define REG_EEE_EN_CTRL                               0x00
    #define REG_EEE_SLEEP_TIMER_G                         0x10
    #define REG_EEE_SLEEP_TIMER_FE                        0x34

/****************************************************************************
   FFE Registers
****************************************************************************/

#define PAGE_FFE                                          0xB4
    #define REG_FFE_RXPORT_CTRL                           0x1C
	    #define FFE_PAD_ENABLE_M                          0x1
	    #define FFE_PAD_ENABLE_S                          8
	    #define FFE_PAD_SIZE_M                            0xFF
	    #define FFE_PAD_SIZE_S                            0

/****************************************************************************
   Shared Defines for bcmsw.c and ethsw.c files
****************************************************************************/
#define TOTAL_SWITCH_PORTS   9
#define PBMAP_ALL            0x1FF
#define PBMAP_MIPS           0x100
#define PBMAP_MIPS_N_GPON    0x180
#define DEFAULT_PBVLAN_MAP   0x1FF
#define PBMAP_MIPS_N_EPON    0x180
#define PBMAP_UNIS           0x7F
#define PBMAP_EPON           0x80

#endif /* _BCMMII_H_ */

#if (defined(CONFIG_BCM96816) || defined(CONFIG_BCM96818))
#define NUM_INT_EPHYS 0
#define NUM_INT_GPHYS 2
#define DEFAULT_BASE_PHYID 0
#elif defined(CONFIG_BCM963268)
#define NUM_INT_EPHYS 3
#define NUM_INT_GPHYS 1
#define DEFAULT_BASE_PHYID 1
#elif defined(CONFIG_BCM96828)
#define NUM_INT_EPHYS 2
#define NUM_INT_GPHYS 2
#define DEFAULT_BASE_PHYID 1
#else
#define NUM_INT_EPHYS 4
#define NUM_INT_GPHYS 0
#define DEFAULT_BASE_PHYID 1
#endif

#endif
#define kLedOff             0
#define kLedOn              1

#define kLedGreen           0
#define kLedRed             1

// uncomment // for debug led
// #define DEBUG_LED

typedef int (*BP_LED_FUNC) (unsigned short *);

typedef struct {
    BOARD_LED_NAME ledName;
    BP_LED_FUNC bpFunc;
    BP_LED_FUNC bpFuncFail;
} BP_LED_INFO, *PBP_LED_INFO;

typedef struct {
    short ledGreenGpio;             // GPIO # for LED
    short ledRedGpio;               // GPIO # for Fail LED
    BOARD_LED_STATE ledState;       // current led state
    short blinkCountDown;           // Count for blink states
} LED_CTRL, *PLED_CTRL;

static BP_LED_INFO bpLedInfo[] =
{
#if 1 /*MSTC, chris*/
   {kLedAdsl_TO2, BpGetAdslLedGpio, BpGetAdslFailLedGpio},
   {kLedAdsl, NULL, NULL},
#else
    {kLedAdsl, BpGetAdslLedGpio, BpGetAdslFailLedGpio},
#endif    
    {kLedSecAdsl, BpGetSecAdslLedGpio, BpGetSecAdslFailLedGpio},
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120702*/
#ifdef MTS_LED_CONTROL
	{kLedEth0, BpGetEth0LedGpio, NULL},
	{kLedEth1, BpGetEth1LedGpio, NULL},
	{kLedEth2, BpGetEth2LedGpio, NULL},
	{kLedEth3, BpGetEth3LedGpio, NULL},
#endif /*MTS_LED_CONTROL*/
    {kLedWanData, BpGetWanDataLedGpio, BpGetWanErrorLedGpio},
    {kLedWanError, BpGetWanErrorLedGpio, NULL},
    {kLedSes, BpGetWirelessSesLedGpio, NULL},
/*Mini FTTH need to support WPS Error LED, KeyYang@MSTC 20130110*/
#ifdef MTS_WPS_ERR_LED_SUPPORTED
	{kLedWpsErr, BpGetWirelessWpsErrLedGpio, NULL},
#endif    
    {kLedVoip, BpGetVoipLedGpio, NULL},
    {kLedVoip1, BpGetVoip1LedGpio, BpGetVoip1FailLedGpio},
    {kLedVoip2, BpGetVoip2LedGpio, BpGetVoip2FailLedGpio},
    {kLedVoip1Error, BpGetVoip1FailLedGpio, NULL},
    {kLedVoip2Error, BpGetVoip2FailLedGpio, NULL},
    {kLedPots, BpGetPotsLedGpio, NULL},
    {kLedDect, BpGetDectLedGpio, NULL},
    {kLedGpon, BpGetGponLedGpio, BpGetGponFailLedGpio},
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120702*/
#ifdef MTS_LED_CONTROL
	{kLedMoCAL, NULL, NULL},
	{kLedPowerG, BpGetBootloaderPowerOnLedGpio, BpGetBootloaderStopLedGpio},
	{kLedMoCAW, NULL, NULL},
	{kLedUSB0, BpGetUSB0LedGpio, NULL},
	{kLedUSB0Fail, BpGetUSB0FailLedGpio, NULL}, /* Control USB fail as a independ LED, not control by BRCM fail LED function */
	{kLedUSB1, BpGetUSB1LedGpio, NULL},
	{kLedUSB1Fail, BpGetUSB1FailLedGpio, NULL}, /* Control USB fail as a independ LED, not control by BRCM fail LED function */
	{kLed5GWpsFail, BpGet5GWpsFailLedGpio, NULL}, /* KeyYang@MSTC Quqntenna RMT_QCSAPI Porting, 20120528*/
#endif /*MTS_LED_CONTROL*/
#if 1 /* Jayden@MSTC for LED control */
	{kLedSys, BpGetSysLedGpio, BpGetSysFailLedGpio},
	{kLedInternet, BpGetInternetLedGpio, BpGetInternetFailLedGpio},/*20121106 modify by Jayden for To2 LED control*/
	{kLedService, BpGetServiceLedGpio, NULL},
	{kLedWating, BpGetWaitingLedGpio, NULL},
#endif
#if 1 /*20121106 added by Jayden for  To2 LED test*/
	{kLedPhone, BpGetPhoneLedGpio, NULL},
	{kLedInfo, BpGetInfoLedGpio, BpGetInfoFailLedGpio},
	{kLedWlan, BpGetWlanLedGpio, NULL},
	{kLedWlan5G, BpGetWlanLedGpio5G, NULL},
	{kLedEth, NULL, NULL},
	{kLedInternetDataTimer, NULL, NULL},/*Jayden added for disable Internet LED data transmission timer,MitraStar,20130910*/
#endif
    {kLedEnd, NULL, NULL}
};

// global variables:
static struct timer_list gLedTimer;
static PLED_CTRL gLedCtrl = NULL;
static int gTimerOn = FALSE;
static int gTimerOnRequests = 0;
static unsigned int gLedRunningCounter = 0;  // only used by WLAN

void (*ethsw_led_control)(unsigned long ledMask, int value) = NULL;
EXPORT_SYMBOL(ethsw_led_control);

/** This spinlock protects all access to gLedCtrl, gTimerOn
 *  gTimerOnRequests, gLedRunningCounter, and ledTimerStart function.
 *  Use spin_lock_irqsave to lock the spinlock because ledTimerStart
 *  may be called from interrupt handler (WLAN?)
 */
/* Yi-Jun@MSTC: Change 'brcm_ledlock' as global and export it in order to use the 'LED' register by others. */
#if 1
DEFINE_SPINLOCK(brcm_ledlock);
EXPORT_SYMBOL(brcm_ledlock);
#else /* 1 */
static spinlock_t brcm_ledlock;
#endif /* 1 */
static void ledTimerExpire(void);

#if 1 // for Internet LED
static struct timer_list gInternetLedTimer;
static void internetLedTimerExpire(void);
int (*isDataTrafficForwarded)(void);
EXPORT_SYMBOL(isDataTrafficForwarded);
#endif

#if 1 /* Jayden@MSTC for ETH LED control ,20130521*/
#define ROBOSWGPHYCTRL RoboswGphyCtrl
atomic_t phy_read_ref_cnt = ATOMIC_INIT(0);
atomic_t phy_write_ref_cnt = ATOMIC_INIT(0);

spinlock_t bcm_ethlock_phy_access;
int ethsw_phy_pll_up(int ephy_and_gphy)
{
    int ephy_status_changed = 0;

#if NUM_INT_GPHYS > 0
    if (ephy_and_gphy)
    {
        int i;
        uint32 roboswGphyCtrl = GPIO->ROBOSWGPHYCTRL;

        /* Bring up internal GPHY PLLs if they are down */
        for (i = 0; i < NUM_INT_GPHYS; i++)
        {
            if ((roboswGphyCtrl & gphy_pwr_dwn[i]) && !(ephy_forced_pwr_down_status & (1<<(i+NUM_INT_EPHYS))))
            {
                roboswGphyCtrl &= ~gphy_pwr_dwn[i];
                ephy_status_changed = 1;
            }
        }
        if (ephy_status_changed) {
            GPIO->ROBOSWGPHYCTRL = roboswGphyCtrl;
        }
    }
#endif

    /* This is a safety measure in case one tries to access the EPHY */
    /* while the PLL/RoboSw is powered down */
#if defined(CONFIG_BCM96368) || defined(CONFIG_BCM96362) || defined(CONFIG_BCM96328)
    if (GPIO->RoboswEphyCtrl & EPHY_PWR_DOWN_DLL)
    {
        /* Wait for PLL to stabilize before reading EPHY registers */
        GPIO->RoboswEphyCtrl &= ~EPHY_PWR_DOWN_DLL;
        ephy_status_changed = 1;
    }
#elif defined(ROBOSW250_CLK_EN)
    if (!(PERF->blkEnables & ROBOSW250_CLK_EN))
    {
        /* Enable robosw clock */
#if defined(ROBOSW025_CLK_EN)
        PERF->blkEnables |= ROBOSW250_CLK_EN | ROBOSW025_CLK_EN;
#else
        PERF->blkEnables |= ROBOSW250_CLK_EN;
#endif
        ephy_status_changed = 1;
    }
#endif

    if (ephy_status_changed) {
        if (irqs_disabled() || (preempt_count() != 0)) {
            mdelay(PHY_PLL_ENABLE_DELAY);
        } else {
            msleep(PHY_PLL_ENABLE_DELAY);
        }
        return (msecs_to_jiffies(PHY_PLL_ENABLE_DELAY));
    }
    return 0;
}
void ethsw_wreg_ext(int page, int reg, uint8 *data, int len, int is6829)
{
    unsigned long   addr = (SWITCH_BASE + (page << 8) + reg);
    volatile uint8 *base = (volatile uint8 *)addr;

    if (((len != 1) && (len % 2) != 0) || len > 8)
        panic("ethsw_wreg_ext: wrong length!\n");

    if ( 0 == is6829 )
    {
#if defined(CONFIG_BCM_ETH_PWRSAVE)
        ethsw_phy_pll_up(0);
#endif

        if (len == 1)
        {
            *base = *data;
        }
        else if (len == 2)
        {
            *(uint16 *)base = *(uint16 *)data;
        }
        else if (len == 4)
        {
            if ((reg % 4) == 0)
            {
                if ( (int)data & 3 )
                {
                    *(uint32 *)(base + 0) = ((*(uint16 *)(data + 0) << 16) | 
                                             (*(uint16 *)(data + 2) <<  0));
                }
                else
                {
                   *(uint32 *)(base + 0) = *(uint32 *)(data + 0);
                }
            }
            else
            {
                *(uint16 *)(base + 0) = *(uint16 *)(data + 2);
                *(uint16 *)(base + 2) = *(uint16 *)(data + 0);
            }
        }
        else if (len == 6)
        {
            if (reg % 4 == 0)
            {
                if ( (int)(data + 2) & 3 )
                {
                    *(uint32 *)(base + 0) = ((*(uint16 *)(data + 2) << 16) | 
                                             (*(uint16 *)(data + 4) <<  0));
                }
                else
                {
                   *(uint32 *)(base + 0) = *(uint32 *)(data + 2);
                }
                *(uint16 *)(base + 4) = *(uint16 *)(data + 0);
            }
            else
            {
                *(uint16 *)(base + 0) = *(uint16 *)(data + 4);
                if ( (int)(data + 0) & 3 )
                {
                    *(uint32 *)(base + 2) = ((*(uint16 *)(data + 0) << 16) | 
                                             (*(uint16 *)(data + 2) <<  0));
                }
                else
                {
                   *(uint32 *)(base + 2) = *(uint32 *)(data + 0);
                }
            }
        }
        else if (len == 8)
        {
            if ( (int)(data + 0) & 3 )
            {
                *(uint32 *)(base + 0) = ((*(uint16 *)(data + 4) << 16) | 
                                         (*(uint16 *)(data + 6) <<  0));
                *(uint32 *)(base + 4) = ((*(uint16 *)(data + 0) << 16) | 
                                         (*(uint16 *)(data + 2) <<  0));
            }
            else
            {
                *(uint32 *)(base + 0) = *(uint32 *)(data + 4);
                *(uint32 *)(base + 4) = *(uint32 *)(data + 0);
            }
        }
    }
#if (defined(CONFIG_BCM96816) || defined(CONFIG_BCM96818))
    else
    {
        unsigned long spiData;
        if (len == 1)
        {
            spiData = (unsigned long)*data;
            kerSysBcmSpiSlaveWrite(addr, spiData, 1);
        }
        else if (len == 2)
        {
            spiData = (unsigned long)*(uint16 *)data;
            kerSysBcmSpiSlaveWrite(addr, spiData, 2);
        }
        else if (len == 4)
        {
            if ((reg % 4) == 0)
            {
                spiData = (unsigned long)*(uint32 *)(data + 0);
                kerSysBcmSpiSlaveWrite(addr, spiData, 4);
            }
            else
            {
                spiData = (unsigned long)*(uint16 *)(data + 2);
                kerSysBcmSpiSlaveWrite(addr, spiData, 2);
                spiData = (unsigned long)*(uint16 *)(data + 0);
                kerSysBcmSpiSlaveWrite(addr+2, spiData, 2);
            }
        }
        else if (len == 6)
        {
            if (reg % 4 == 0)
            {
                spiData = (unsigned long)*(uint32 *)(data + 2);
                kerSysBcmSpiSlaveWrite(addr, spiData, 4);
                spiData = (unsigned long)*(uint16 *)(data + 0);
                kerSysBcmSpiSlaveWrite(addr+4, spiData, 2);
            }
            else
            {
                spiData = (unsigned long)*(uint16 *)(data + 4);
                kerSysBcmSpiSlaveWrite(addr, spiData, 2);
                spiData = (unsigned long)*(uint32 *)(data + 0);
                kerSysBcmSpiSlaveWrite(addr+2, spiData, 4);
            }
        }
        else if (len == 8)
        {
            spiData = (unsigned long)*(uint32 *)(data + 4);
            kerSysBcmSpiSlaveWrite(addr, spiData, 4);
            spiData = (unsigned long)*(uint32 *)(data + 0);
            kerSysBcmSpiSlaveWrite(addr+4, spiData, 4);
        }
    }
#else
    else
    {
        panic("ethsw_wreg_ext: 6829 not availble\n");
    }
#endif
}
void ethsw_rreg_ext(int page, int reg, uint8 *data, int len, int is6829)
{
    unsigned long   addr = (SWITCH_BASE + (page << 8) + reg);
    volatile uint8 *base = (volatile uint8 *)addr;

    if (((len != 1) && (len % 2) != 0) || len > 8)
        panic("ethsw_rreg_ext: wrong length!\n");

    if ( 0 == is6829 )
    {
#if defined(CONFIG_BCM_ETH_PWRSAVE)
        ethsw_phy_pll_up(0);
#endif

        if (len == 1)
        {
            *data = *base;
        }
        else if (len == 2)
        {
            *(uint16 *)(data + 0) = *(uint16 *)(base + 0);
        }
        else if (len == 4)
        {
            if ((reg % 4) == 0)
            {
                *(uint32 *)(data + 0) = *(uint32 *)(base + 0);
            }
            else
            {
                *(uint16 *)(data + 2) = *(uint16 *)(base + 0);
                *(uint16 *)(data + 0) = *(uint16 *)(base + 2);
            }
        }
        else if (len == 6)
        {
            if ((reg % 4) == 0)
            {
                *(uint32 *)(data + 2) = *(uint32 *)(base + 0);
                *(uint16 *)(data + 0) = *(uint16 *)(base + 4);
            }
            else
            {
                *(uint16 *)(data + 4) = *(uint16 *)(base + 0);
                *(uint32 *)(data + 0) = *(uint32 *)(base + 2);
            }
        }
        else if (len == 8)
        {
            *(uint32 *)(data + 4) = *(uint32 *)(base + 0);
            *(uint32 *)(data + 0) = *(uint32 *)(base + 4);
        }
    }
#if (defined(CONFIG_BCM96816) || defined(CONFIG_BCM96818))
    else
    {
        unsigned long spiData;
        if (1 == len)
        {
            kerSysBcmSpiSlaveRead(addr, &spiData, 1);
            *data = (char)spiData;
        }
        else if (len == 2)
        {
            kerSysBcmSpiSlaveRead(addr, &spiData, 2);
            *(uint16 *)(data + 0) = (uint16)spiData;
        }
        else if (len == 4)
        {
            if ((reg % 4) == 0)
            {
               kerSysBcmSpiSlaveRead(addr, &spiData, 4);
               *(uint32 *)(data + 0) = (uint32)spiData;
            }
            else
            {
                kerSysBcmSpiSlaveRead(addr, &spiData, 2);
                *(uint16 *)(data + 2) = (uint16)spiData;
                kerSysBcmSpiSlaveRead(addr+2, &spiData, 2);
                *(uint16 *)(data + 0) = (uint16)spiData;
            }
        }
        else if (len == 6)
        {
            if ((reg % 4) == 0)
            {
                kerSysBcmSpiSlaveRead(addr, &spiData, 4);
                *(uint32 *)(data + 2) = (uint32)spiData;
                kerSysBcmSpiSlaveRead(addr+4, &spiData, 2);
                *(uint16 *)(data + 0) = (uint16)spiData;
            }
            else
            {
                kerSysBcmSpiSlaveRead(addr, &spiData, 2);
                *(uint16 *)(data + 4) = (uint16)spiData;
                kerSysBcmSpiSlaveRead(addr+2, &spiData, 4);
                *(uint32 *)(data + 0) = (uint32)spiData;
            }
        }
        else if (len == 8)
        {
            kerSysBcmSpiSlaveRead(addr, &spiData, 4);
            *(uint32 *)(data + 4) = (uint32)spiData;
            kerSysBcmSpiSlaveRead(addr+4, &spiData, 4);
            *(uint32 *)(data + 0) = (uint32)spiData;
        }
    }
#else
    else
    {
        panic("ethsw_rreg_ext: 6829 not availble\n");
    }
#endif
}
void ethsw_phy_read_reg(int phy_id, int reg, uint16 *data, int ext_bit)
{
    uint32 reg_value;
    int    is6829 = 0, in_interrupt = 0;
	 spin_lock_init(&bcm_ethlock_phy_access);
#if defined(CONFIG_BCM96816)
    if ( IsExt6829(phy_id) )
    {
       is6829  = 1;
    }
#endif
    phy_id &= BCM_PHY_ID_M;

    reg_value = 0;

    if (in_interrupt()) {
        in_interrupt = 1;
    }

    /* Disable Phy interrupt so Phy ISR will not enter this function while a transaction is in progress. Will need similar protection if there is ever a need
           to access phy in another interrupt context (like the packet rx isr (napi)). We can use spin_lock_irqsave but the phy access takes >50uS and
           it disables all the interrupts on the local CPU(not just the Phy interrupt) */
    if (!in_interrupt) {
#if !defined(CONFIG_BCM96818)
        BcmHalInterruptDisable(INTERRUPT_ID_EPHY);
#endif
#if defined(CONFIG_BCM963268)
        BcmHalInterruptDisable(INTERRUPT_ID_GPHY);
#endif
#if defined(CONFIG_BCM96828)
        BcmHalInterruptDisable(INTERRUPT_ID_GPHY0);
        BcmHalInterruptDisable(INTERRUPT_ID_GPHY1);
#endif

    }
    atomic_inc(&phy_read_ref_cnt);
    spin_lock(&bcm_ethlock_phy_access);

    ethsw_wreg_ext(PAGE_CONTROL, REG_MDIO_CTRL_ADDR, (uint8 *)&reg_value, 4, is6829);
    reg_value = (ext_bit? REG_MDIO_CTRL_EXT: 0) | (REG_MDIO_CTRL_READ) |
        ((phy_id << REG_MDIO_CTRL_ID_SHIFT) & REG_MDIO_CTRL_ID_MASK) |
        (reg  << REG_MDIO_CTRL_ADDR_SHIFT);
    ethsw_wreg_ext(PAGE_CONTROL, REG_MDIO_CTRL_ADDR, (uint8 *)&reg_value, 4, is6829);

    if (irqs_disabled() || (preempt_count() != 0)) {
        udelay(60);
    } else {
        msleep(1);
    }
    ethsw_rreg_ext(PAGE_CONTROL, REG_MDIO_DATA_ADDR, (uint8 *)data, 2, is6829);

    spin_unlock(&bcm_ethlock_phy_access);
    atomic_dec(&phy_read_ref_cnt);
    if (!in_interrupt) {
        if (atomic_read(&phy_read_ref_cnt) == 0) {
#if !defined(CONFIG_BCM96818)
            BcmHalInterruptEnable(INTERRUPT_ID_EPHY);
#endif
#if defined(CONFIG_BCM963268) 
            BcmHalInterruptEnable(INTERRUPT_ID_GPHY);
#endif
#if defined(CONFIG_BCM96828) 
           BcmHalInterruptEnable(INTERRUPT_ID_GPHY0);
           BcmHalInterruptEnable(INTERRUPT_ID_GPHY1);
#endif
        }
    }
}
void ethsw_phy_write_reg(int phy_id, int reg, uint16 *data, int ext_bit)
{
    uint32 reg_value;
    int    is6829 = 0, in_interrupt = 0;
 spin_lock_init(&bcm_ethlock_phy_access);
#if defined(CONFIG_BCM96816)
    if ( IsExt6829(phy_id) )
    {
       is6829  = 1;
    }
#endif
    phy_id &= BCM_PHY_ID_M;

    reg_value = 0;

    if (in_interrupt()) {
        in_interrupt = 1;
    }

    /* Disable Phy interrupt so Phy ISR will not enter this function while a transaction is in progress. Will need similar protection if there is ever a need
           to access phy in another interrupt context (like the packet rx isr (napi)). We can use spin_lock_irqsave but the phy access takes >50uS and
           it disables all the interrupts on the local CPU(not just the Phy interrupt) */
    if (!in_interrupt) {
#if !defined(CONFIG_BCM96818)
        BcmHalInterruptDisable(INTERRUPT_ID_EPHY);
#endif
#if defined(CONFIG_BCM963268)
        BcmHalInterruptDisable(INTERRUPT_ID_GPHY);
#endif
#if  defined(CONFIG_BCM96828)
		BcmHalInterruptDisable(INTERRUPT_ID_GPHY0);
        BcmHalInterruptDisable(INTERRUPT_ID_GPHY1);
#endif

    }
    atomic_inc(&phy_write_ref_cnt);
    spin_lock(&bcm_ethlock_phy_access);

    ethsw_wreg_ext(PAGE_CONTROL, REG_MDIO_CTRL_ADDR, (uint8 *)&reg_value, 4, is6829);
    reg_value = (ext_bit? REG_MDIO_CTRL_EXT: 0) | (REG_MDIO_CTRL_WRITE) |
        ((phy_id << REG_MDIO_CTRL_ID_SHIFT) & REG_MDIO_CTRL_ID_MASK) |
        (reg  << REG_MDIO_CTRL_ADDR_SHIFT) | *data;
    ethsw_wreg_ext(PAGE_CONTROL, REG_MDIO_CTRL_ADDR, (uint8 *)&reg_value, 4, is6829);

    if (irqs_disabled() || (preempt_count() != 0)) {
        udelay(60);
    } else {
      msleep(1);
    }

    spin_unlock(&bcm_ethlock_phy_access);
    atomic_dec(&phy_write_ref_cnt);
    if (!in_interrupt) {
        if (atomic_read(&phy_write_ref_cnt) <= 0) {
#if !defined(CONFIG_BCM96818)
            BcmHalInterruptEnable(INTERRUPT_ID_EPHY);
#endif
#if defined(CONFIG_BCM963268) 
            BcmHalInterruptEnable(INTERRUPT_ID_GPHY);
#endif
#if defined(CONFIG_BCM96828)
			BcmHalInterruptEnable(INTERRUPT_ID_GPHY0);
            BcmHalInterruptEnable(INTERRUPT_ID_GPHY1);
#endif

        }
    }
}
void ethsw_phy_rreg(int phy_id, int reg, uint16 *data)
{
    int ext_bit = IsExtPhyId(phy_id);
    ethsw_phy_read_reg(phy_id, reg, data, ext_bit);
}
void ethsw_phy_wreg(int phy_id, int reg, uint16 *data)
{
    int ext_bit = IsExtPhyId(phy_id);
    ethsw_phy_write_reg(phy_id, reg, data, ext_bit);
}
void bcmsw_pmdio_wreg(int page, int reg, unsigned char *data, int len)
{
    uint16 v;
    int i;
	spinlock_t bcm_extsw_access;
	spin_lock_init(&bcm_extsw_access);
  /*  BCM_ENET_LINK_DEBUG("write op; page = %x; reg = %x; len = %d \n", 
        (unsigned int) page, (unsigned int) reg, len);
    BCM_ENET_LINK_DEBUG("given data = %02x %02x %02x %02x \n", 
        data[0], data[1], data[2], data[3]);*/
    spin_lock(&bcm_extsw_access);
    v = (page << REG_PPM_REG16_SWITCH_PAGE_NUMBER_SHIFT) | REG_PPM_REG16_MDIO_ENABLE;
    ethsw_phy_wreg(PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG16, &v);

    switch (len) {
        case 1:
            v = data[0];
            ethsw_phy_wreg(PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG24, &v);
            break;
        case 2:
            v = swab16(((uint16 *)data)[0]);
            ethsw_phy_wreg(PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG24, &v);
            break;
        case 4:
            ((uint32 *)data)[0] = swab32(((uint32 *)data)[0]);
            v = ((uint16 *)data)[0];
            ethsw_phy_wreg(PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG25, &v);
            v = ((uint16 *)data)[1];
            ethsw_phy_wreg(PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG24, &v);
            break;
        case 6:
            if ((reg %4) == 0) {
                *(uint32 *)data = swab32(*(uint32 *)data);
                *((uint16 *)(data + 4)) = swab16(*((uint16 *)(data + 4)));
            } else {
                *(uint16 *)data = swab32(*(uint16 *)data);
                *((uint32 *)(data + 2)) = swab16(*((uint32 *)(data + 2)));
            }
            v = ((uint16 *)data)[0];
            ethsw_phy_wreg(PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG26, &v);
            v = ((uint16 *)data)[1];
            ethsw_phy_wreg(PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG25, &v);
            v = ((uint16 *)data)[2];
            ethsw_phy_wreg(PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG24, &v);
            break;
        case 8:
            ((uint32 *)data)[0] = swab32(((uint32 *)data)[0]);
            v = ((uint16 *)data)[0];
            ethsw_phy_wreg(PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG27, &v);
            v = ((uint16 *)data)[1];
            ethsw_phy_wreg(PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG26, &v);
            ((uint32 *)data)[1] = swab32(((uint32 *)data)[1]);
            v = ((uint16 *)data)[2];
            ethsw_phy_wreg(PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG25, &v);
            v = ((uint16 *)data)[3];
            ethsw_phy_wreg(PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG24, &v);
            break;
    }

    v = (reg << REG_PPM_REG17_REG_NUMBER_SHIFT) | REG_PPM_REG17_OP_WRITE;
    ethsw_phy_wreg(PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG17, &v);

    for (i = 0; i < 20; i++) {
        ethsw_phy_rreg(PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG17, &v);
        if ((v & (REG_PPM_REG17_OP_WRITE | REG_PPM_REG17_OP_READ)) == REG_PPM_REG17_OP_DONE)
            break;
        udelay(10);
    }

    spin_unlock(&bcm_extsw_access);

    if (i >= 20)
        printk("ethsw_mdio_wreg: timeout!\n");
}
#endif
//**************************************************************************************
// LED operations
//**************************************************************************************

// turn led on and set the ledState
static void setLed (PLED_CTRL pLed, unsigned short led_state, unsigned short led_type)
{
    short led_gpio;
    unsigned short gpio_state;
    unsigned long flags;

    if (led_type == kLedRed)
        led_gpio = pLed->ledRedGpio;
    else
        led_gpio = pLed->ledGreenGpio;

    if (led_gpio == -1)
        return;

    if (((led_gpio & BP_ACTIVE_LOW) && (led_state == kLedOn)) || 
        (!(led_gpio & BP_ACTIVE_LOW) && (led_state == kLedOff)))
        gpio_state = 0;
    else
        gpio_state = 1;

    /* spinlock to protect access to GPIODir, GPIOio */
    spin_lock_irqsave(&bcm_gpio_spinlock, flags);

#if defined(CONFIG_BCM96328) || defined(_BCM96318_)
    /* Enable LED controller to drive this GPIO */
    if (!(led_gpio & BP_GPIO_SERIAL))
        GPIO->GPIOMode |= GPIO_NUM_TO_MASK(led_gpio);
#endif

#if defined(CONFIG_BCM96362) || defined(CONFIG_BCM963268)
    /* Enable LED controller to drive this GPIO */
    if (!(led_gpio & BP_GPIO_SERIAL))
        GPIO->LEDCtrl |= GPIO_NUM_TO_MASK(led_gpio);
#endif

#if defined(CONFIG_BCM96328) || defined(CONFIG_BCM96362) || defined(CONFIG_BCM963268) || defined(CONFIG_BCM96318)
    LED->ledMode &= ~(LED_MODE_MASK << GPIO_NUM_TO_LED_MODE_SHIFT(led_gpio));
    if( gpio_state )
        LED->ledMode |= (LED_MODE_OFF << GPIO_NUM_TO_LED_MODE_SHIFT(led_gpio));
    else
        LED->ledMode |= (LED_MODE_ON << GPIO_NUM_TO_LED_MODE_SHIFT(led_gpio));

#else
    if (led_gpio & BP_GPIO_SERIAL) {
        while (GPIO->SerialLedCtrl & SER_LED_BUSY);
        if( gpio_state )
            GPIO->SerialLed |= GPIO_NUM_TO_MASK(led_gpio);
        else
            GPIO->SerialLed &= ~GPIO_NUM_TO_MASK(led_gpio);
    }
    else {
        GPIO->GPIODir |= GPIO_NUM_TO_MASK(led_gpio);
        if( gpio_state )
            GPIO->GPIOio |= GPIO_NUM_TO_MASK(led_gpio);
        else
            GPIO->GPIOio &= ~GPIO_NUM_TO_MASK(led_gpio);
    }
#endif

    spin_unlock_irqrestore(&bcm_gpio_spinlock, flags);
}

// toggle the LED
#if 1 /*For To2 Eth LED behavior,MitraStar Jayden,20130521*/	
static void ledToggle(PLED_CTRL pLed)
{
    short led_gpio;	
	if(pLed->ledState==kLedEthStateSlowBlinkContinues){
		if(IsEthLEDOn){
			bcmsw_pmdio_wreg(0,18,LedCtlOff,2);/*ethswctl -c regaccess -n 1 -v 0x12 -l 2 -d 0x021*/
			IsEthLEDOn=FALSE;
		}
		else{
			 bcmsw_pmdio_wreg(0,18,LedCtlOn,2);/*ethswctl -c regaccess -n 1 -v 0x12 -l 2 -d 0x321*/
			IsEthLEDOn=TRUE;
		}
		
	}
	else{

		if((pLed->ledState==kLedFailStateSlowBlinkContinues)||(pLed->ledState==kLedFailStateBlinkContinues)||(pLed->ledState==kLedFailStateFastBlinkContinues))
			led_gpio = pLed->ledRedGpio;
		else
			led_gpio = pLed->ledGreenGpio;


	    if (led_gpio == -1)
	        return;

#if defined(CONFIG_BCM96328) || defined(CONFIG_BCM96362) || defined(CONFIG_BCM963268) || defined(CONFIG_BCM96318)
	    LED->ledMode ^= (LED_MODE_MASK << GPIO_NUM_TO_LED_MODE_SHIFT(led_gpio));
#else

	    if (led_gpio & BP_GPIO_SERIAL) {
	        while (GPIO->SerialLedCtrl & SER_LED_BUSY);
	        GPIO->SerialLed ^= GPIO_NUM_TO_MASK(led_gpio);
	    }
	    else {
	        unsigned long flags;

	        spin_lock_irqsave(&bcm_gpio_spinlock, flags);
	        GPIO->GPIODir |= GPIO_NUM_TO_MASK(led_gpio);
	        GPIO->GPIOio ^= GPIO_NUM_TO_MASK(led_gpio);
	        spin_unlock_irqrestore(&bcm_gpio_spinlock, flags);
	    }	
#endif
	}
}   

#else
static void ledToggle(PLED_CTRL pLed)
{
    short led_gpio;
    led_gpio = pLed->ledGreenGpio;
    if (led_gpio == -1)
        return;

#if defined(CONFIG_BCM96328) || defined(CONFIG_BCM96362) || defined(CONFIG_BCM963268) || defined(CONFIG_BCM96318)
    LED->ledMode ^= (LED_MODE_MASK << GPIO_NUM_TO_LED_MODE_SHIFT(led_gpio));
#else

    if (led_gpio & BP_GPIO_SERIAL) {
        while (GPIO->SerialLedCtrl & SER_LED_BUSY);
        GPIO->SerialLed ^= GPIO_NUM_TO_MASK(led_gpio);
    }
    else {
        unsigned long flags;

        spin_lock_irqsave(&bcm_gpio_spinlock, flags);
        GPIO->GPIODir |= GPIO_NUM_TO_MASK(led_gpio);
        GPIO->GPIOio ^= GPIO_NUM_TO_MASK(led_gpio);
        spin_unlock_irqrestore(&bcm_gpio_spinlock, flags);
    }
#endif
}   
#endif

#if 1 // for Internet LED
/** Start the Internet LED timer
 *
 * Must be called with brcm_ledlock held
 */
static void internetLedTimerStart(void)
{
#if defined(DEBUG_LED)
    printk("led: add_timer\n");
#endif

	BCM_ASSERT_HAS_SPINLOCK_C(&brcm_ledlock);

    init_timer(&gInternetLedTimer);
    gInternetLedTimer.function = (void*)internetLedTimerExpire;
    gInternetLedTimer.expires = jiffies + (HZ/4); /*flash frequency 2Hz*/ 	
    add_timer (&gInternetLedTimer);
} 

static void internetLedTimerExpire(void)
{
	unsigned long flags;
	int ret = 0;
	if(LedInternetDataTimerOn){/*Jayden added for disable Internet LED data transmission timer,MitraStar,20130910*/
		if ( LedInternetOn == FALSE)
		{
			internetLedTimerStart();
			return;
		}
		BCM_ASSERT_NOT_HAS_SPINLOCK_V(&brcm_ledlock);
		if (isDataTrafficForwarded != NULL)
			ret = isDataTrafficForwarded();
		
		spin_lock_irqsave(&brcm_ledlock, flags);
		if (ret != 0 )
		{
			ledToggle(&gLedCtrl[kLedInternet]);
		}
		else
		{
			setLed(&gLedCtrl[kLedInternet], kLedOn, kLedGreen);
		}
	    	internetLedTimerStart();
		spin_unlock_irqrestore(&brcm_ledlock, flags);
	}
}
#endif

/** Start the LED timer
 *
 * Must be called with brcm_ledlock held
 */
static void ledTimerStart(void)
{
#if defined(DEBUG_LED)
    printk("led: add_timer\n");
#endif

    BCM_ASSERT_HAS_SPINLOCK_C(&brcm_ledlock);

    init_timer(&gLedTimer);
    gLedTimer.function = (void*)ledTimerExpire;
#if 1 /*For To2 LED behavior,MitraStar Jayden,20130117*/
    gLedTimer.expires = jiffies + (HZ/4); /*flash frequency 2Hz*/ 	
#else
    gLedTimer.expires = jiffies + k125ms;        // timer expires in ~100ms
#endif   
    add_timer (&gLedTimer);
} 


// led timer expire kicks in about ~100ms and perform the led operation according to the ledState and
// restart the timer according to ledState
static void ledTimerExpire(void)
{
    int i;
    PLED_CTRL pCurLed;
    unsigned long flags;

    BCM_ASSERT_NOT_HAS_SPINLOCK_V(&brcm_ledlock);

    for (i = 0, pCurLed = gLedCtrl; i < kLedEnd; i++, pCurLed++)
    {
#if defined(DEBUG_LED)
        printk("led[%d]: Mask=0x%04x, State = %d, blcd=%d\n", i, pCurLed->ledGreenGpio, pCurLed->ledState, pCurLed->blinkCountDown);
#endif
        spin_lock_irqsave(&brcm_ledlock, flags);        // LEDs can be changed from ISR
        switch (pCurLed->ledState)
        {
 #if 1 
 	case kLedInternetDataTimerDisable:/*Jayden added for disable Internet LED data transmission timer,MitraStar,20130910*/
	case kLedEthFrqSet:	/*Jayden added for ETH LED data transmission flash Frequence,MitraStar,20130911*/
 #endif
        case kLedStateOn:
        case kLedStateOff:
        case kLedStateFail:
            pCurLed->blinkCountDown = 0;            // reset the blink count down
            spin_unlock_irqrestore(&brcm_ledlock, flags);
            break;
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120710*/
#ifdef MTS_LED_CONTROL
        /* LED , __TELEFONICA__, Mitrastar Curtis, 20120106 */
        case ALLLedStateOn:
        case ALLLedStateOff:
            spin_unlock_irqrestore(&brcm_ledlock, flags);
            break;
#endif /*MTS_LED_CONTROL*/
        case kLedStateSlowBlinkContinues:
            if (pCurLed->blinkCountDown-- == 0)
            {
                pCurLed->blinkCountDown = kSlowBlinkCount;
                ledToggle(pCurLed);
            }
            gTimerOnRequests++;
            spin_unlock_irqrestore(&brcm_ledlock, flags);
            break;

        case kLedStateFastBlinkContinues:
            if (pCurLed->blinkCountDown-- == 0)
            {
                pCurLed->blinkCountDown = kFastBlinkCount;
                ledToggle(pCurLed);
            }
            gTimerOnRequests++;
            spin_unlock_irqrestore(&brcm_ledlock, flags);
            break;

#if 1 /*For To2 LED behavior,MitraStar Jayden,20130117*/
	case kLedPwrStateRedfor1sandGreen:/*For To2 Pwr LED behavior,MitraStar Jayden,20130604*/
		 if (pCurLed->blinkCountDown-- == 0)
           	 {
              	  pCurLed->blinkCountDown = 0;
			 setLed (pCurLed, kLedOff, kLedRed);	  
			 setLed(pCurLed, kLedOn, kLedGreen);	  
                	 
            	}
		 gTimerOnRequests++;
            	spin_unlock_irqrestore(&brcm_ledlock, flags);
            	break;
	case kLedStateBlinkContinues:
            if (pCurLed->blinkCountDown-- == 0)
            {
                pCurLed->blinkCountDown = kBlinkCount;
                ledToggle(pCurLed);
            }
            gTimerOnRequests++;
            spin_unlock_irqrestore(&brcm_ledlock, flags);
            break;

	case kLedFailStateFastBlinkContinues:
            if (pCurLed->blinkCountDown-- == 0)
            {
                pCurLed->blinkCountDown = kFastBlinkCount;
                ledToggle(pCurLed);
            }
            gTimerOnRequests++;
            spin_unlock_irqrestore(&brcm_ledlock, flags);
            break;

	case kLedFailStateBlinkContinues:
            if (pCurLed->blinkCountDown-- == 0)
            {
                pCurLed->blinkCountDown = kBlinkCount;
                ledToggle(pCurLed);
            }
            gTimerOnRequests++;
            spin_unlock_irqrestore(&brcm_ledlock, flags);
            break;

	case kLedFailStateSlowBlinkContinues:
            if (pCurLed->blinkCountDown-- == 0)
            {
                pCurLed->blinkCountDown = kSlowBlinkCount;
                ledToggle(pCurLed);
            }
            gTimerOnRequests++;
            spin_unlock_irqrestore(&brcm_ledlock, flags);
            break;	
	case kLedEthStateSlowBlinkContinues:
            if (pCurLed->blinkCountDown-- == 0)
            {
                pCurLed->blinkCountDown = kSlowBlinkCount;
                ledToggle(pCurLed);
            }
            gTimerOnRequests++;
            spin_unlock_irqrestore(&brcm_ledlock, flags);
            break;		
#endif			

        case kLedStateUserWpsInProgress:          /* 200ms on, 100ms off */
            if (pCurLed->blinkCountDown-- == 0)
            {
                pCurLed->blinkCountDown = (((gLedRunningCounter++)&1)? kFastBlinkCount: kSlowBlinkCount);
                ledToggle(pCurLed);
            }
            gTimerOnRequests++;
            spin_unlock_irqrestore(&brcm_ledlock, flags);
            break;             

        case kLedStateUserWpsError:               /* 100ms on, 100ms off */
            if (pCurLed->blinkCountDown-- == 0)
            {
                pCurLed->blinkCountDown = kFastBlinkCount;
                ledToggle(pCurLed);
            }
            gTimerOnRequests++;
            spin_unlock_irqrestore(&brcm_ledlock, flags);
            break;        

        case kLedStateUserWpsSessionOverLap:      /* 100ms on, 100ms off, 5 times, off for 500ms */        
            if (pCurLed->blinkCountDown-- == 0)
            {
                if(((++gLedRunningCounter)%10) == 0) {
                    pCurLed->blinkCountDown = 4;
                    setLed(pCurLed, kLedOff, kLedGreen);
                    pCurLed->ledState = kLedStateUserWpsSessionOverLap;
                }
                else
                {
                    pCurLed->blinkCountDown = kFastBlinkCount;
                    ledToggle(pCurLed);
                }
            }
            gTimerOnRequests++;
            spin_unlock_irqrestore(&brcm_ledlock, flags);
            break;        

        default:
            spin_unlock_irqrestore(&brcm_ledlock, flags);
            printk("Invalid state = %d\n", pCurLed->ledState);
        }
    }

    // Restart the timer if any of our previous LED operations required
    // us to restart the timer, or if any other threads requested a timer
    // restart.  Note that throughout this function, gTimerOn == TRUE, so
    // any other thread which want to restart the timer would only
    // increment the gTimerOnRequests and not call ledTimerStart.
    spin_lock_irqsave(&brcm_ledlock, flags);
    if (gTimerOnRequests > 0)
    {
        ledTimerStart();
        gTimerOnRequests = 0;
    }
    else
    {
        gTimerOn = FALSE;
    }
    spin_unlock_irqrestore(&brcm_ledlock, flags);
}

void __init boardLedInit(void)
{
    PBP_LED_INFO pInfo;
    unsigned short i;
    short gpio;
#if defined(CONFIG_BCM96816)
    ETHERNET_MAC_INFO EnetInfo;
#endif

    spin_lock_init(&brcm_ledlock);

#if !(defined(CONFIG_BCM96328) || defined(CONFIG_BCM96362) || defined(CONFIG_BCM963268) || defined(CONFIG_BCM96318))
    /* Set blink rate for hardware LEDs. */
    GPIO->LEDCtrl &= ~LED_INTERVAL_SET_MASK;
    GPIO->LEDCtrl |= LED_INTERVAL_SET_80MS;
#endif

    gLedCtrl = (PLED_CTRL) kmalloc((kLedEnd * sizeof(LED_CTRL)), GFP_KERNEL);

    if( gLedCtrl == NULL ) {
        printk( "LED memory allocation error.\n" );
        return;
    }

    /* Initialize LED control */
    for (i = 0; i < kLedEnd; i++) {
        gLedCtrl[i].ledGreenGpio = -1;
        gLedCtrl[i].ledRedGpio = -1;
        gLedCtrl[i].ledState = kLedStateOff;
        gLedCtrl[i].blinkCountDown = 0;            // reset the blink count down
    }

    for( pInfo = bpLedInfo; pInfo->ledName != kLedEnd; pInfo++ ) {
        if( pInfo->bpFunc && (*pInfo->bpFunc) (&gpio) == BP_SUCCESS )
        {
            gLedCtrl[pInfo->ledName].ledGreenGpio = gpio;
        }
        if( pInfo->bpFuncFail && (*pInfo->bpFuncFail)(&gpio)==BP_SUCCESS )
        {
            gLedCtrl[pInfo->ledName].ledRedGpio = gpio;
        }
        setLed(&gLedCtrl[pInfo->ledName], kLedOff, kLedGreen);
        setLed(&gLedCtrl[pInfo->ledName], kLedOff, kLedRed);
    }

#if defined(CONFIG_BCM96816)
    if( BpGetEthernetMacInfo( &EnetInfo, 1 ) == BP_SUCCESS )
    {
        if ( EnetInfo.sw.ledInfo[0].duplexLed != BP_NOT_DEFINED )
        {
            gLedCtrl[kLedEth0Duplex].ledGreenGpio = EnetInfo.sw.ledInfo[0].duplexLed;
            setLed(&gLedCtrl[kLedEth0Duplex], kLedOff, kLedGreen);
        }
        if ( EnetInfo.sw.ledInfo[0].speedLed100 != BP_NOT_DEFINED )
        {
            gLedCtrl[kLedEth0Spd100].ledGreenGpio = EnetInfo.sw.ledInfo[0].speedLed100;
            setLed(&gLedCtrl[kLedEth0Spd100], kLedOff, kLedGreen);
        }
        if ( EnetInfo.sw.ledInfo[0].speedLed1000 != BP_NOT_DEFINED )
        {
            gLedCtrl[kLedEth0Spd1000].ledGreenGpio = EnetInfo.sw.ledInfo[0].speedLed1000;
            setLed(&gLedCtrl[kLedEth0Spd1000], kLedOff, kLedGreen);
        }
     
        if ( EnetInfo.sw.ledInfo[1].duplexLed != BP_NOT_DEFINED )
        {
            gLedCtrl[kLedEth1Duplex].ledGreenGpio = EnetInfo.sw.ledInfo[1].duplexLed;
            setLed(&gLedCtrl[kLedEth1Duplex], kLedOff, kLedGreen);
        }
        if ( EnetInfo.sw.ledInfo[1].speedLed100 != BP_NOT_DEFINED )
        {
            gLedCtrl[kLedEth1Spd100].ledGreenGpio = EnetInfo.sw.ledInfo[1].speedLed100;
            setLed(&gLedCtrl[kLedEth1Spd100], kLedOff, kLedGreen);
        }
        if ( EnetInfo.sw.ledInfo[1].speedLed1000 != BP_NOT_DEFINED )
        {
            gLedCtrl[kLedEth1Spd1000].ledGreenGpio = EnetInfo.sw.ledInfo[1].speedLed1000;
            setLed(&gLedCtrl[kLedEth1Spd1000], kLedOff, kLedGreen);
        }
    }
#endif

#if 1 // for Internet LED
	isDataTrafficForwarded = NULL;
	internetLedTimerStart();
#endif

#if defined(DEBUG_LED)
    for (i=0; i < kLedEnd; i++)
        printk("initLed: led[%d]: Gpio=0x%04x, FailGpio=0x%04x\n", i, gLedCtrl[i].ledGreenGpio, gLedCtrl[i].ledRedGpio);
#endif

}

// led ctrl.  Maps the ledName to the corresponding ledInfoPtr and perform the led operation
void boardLedCtrl(BOARD_LED_NAME ledName, BOARD_LED_STATE ledState)
{
    unsigned long flags;
    PLED_CTRL pLed;

    BCM_ASSERT_NOT_HAS_SPINLOCK_V(&brcm_ledlock);

    spin_lock_irqsave(&brcm_ledlock, flags);     // LED can be changed from ISR

    if( (int) ledName < kLedEnd ) {

        pLed = &gLedCtrl[ledName];

        // If the state is kLedStateFail and there is not a failure LED defined
        // in the board parameters, change the state to kLedStateSlowBlinkContinues.
        if( ledState == kLedStateFail && pLed->ledRedGpio == -1 )
            ledState = kLedStateSlowBlinkContinues;

        // Save current LED state
        pLed->ledState = ledState;

        // Start from LED Off and turn it on later as needed
 #if 1 /* Jayden@MSTC for To2 ETH LED control,20130521 */  
	 if(ledName!=kLedEth){
	 	if(ledName!=kLedInternetDataTimer && ledName!= kLedEthFrqControl){/*Jayden added for disable Internet LED data transmission timer,MitraStar,20130910*//*Jayden added for ETH LED data transmission flash Frequence,MitraStar,20130911*/
			 setLed (pLed, kLedOff, kLedGreen);
		        setLed (pLed, kLedOff, kLedRed);
	 	}
	 }
	 else{
	 	bcmsw_pmdio_wreg(0,26,LedBhOn,2);/*ethswctl -c regaccess -n 1 -v 0x1a -l 2 -d 0x100*/
		bcmsw_pmdio_wreg(0,18,LedCtlOff,2);/*ethswctl -c regaccess -n 1 -v 0x12 -l 2 -d 0x021*/
		IsEthLEDOn=FALSE;
	 }
 #else
        setLed (pLed, kLedOff, kLedGreen);
        setLed (pLed, kLedOff, kLedRed);
#endif		

        // Disable HW control for WAN Data LED. 
        // It will be enabled only if LED state is On
#if defined(CONFIG_BCM96328) || defined(CONFIG_BCM96362) || defined(CONFIG_BCM963268) || defined(CONFIG_BCM96318)
        if (ledName == kLedWanData)
            LED->ledHWDis |= GPIO_NUM_TO_MASK(pLed->ledGreenGpio);
#elif defined(CONFIG_BCM96368)
        if (ledName == kLedWanData) {
            GPIO->AuxLedCtrl |= AUX_HW_DISAB_2;
            if (pLed->ledGreenGpio & BP_ACTIVE_LOW)
                GPIO->AuxLedCtrl |= (LED_STEADY_ON << AUX_MODE_SHFT_2);
            else
                GPIO->AuxLedCtrl &= ~(LED_STEADY_ON << AUX_MODE_SHFT_2);
        }
#endif

        switch (ledState) {
        case kLedStateOn:
            // Enable SAR to control INET LED
#if defined(CONFIG_BCM96328) || defined(CONFIG_BCM96362) || defined(CONFIG_BCM963268) || defined(CONFIG_BCM96318)
#if 0 // for Internet LED
            if (ledName == kLedWanData)
                LED->ledHWDis &= ~GPIO_NUM_TO_MASK(pLed->ledGreenGpio);
#endif
#elif defined(CONFIG_BCM96368)
            if (ledName == kLedWanData) {
                GPIO->AuxLedCtrl &= ~AUX_HW_DISAB_2;
                if (pLed->ledGreenGpio & BP_ACTIVE_LOW)
                    GPIO->AuxLedCtrl &= ~(LED_STEADY_ON << AUX_MODE_SHFT_2);
                else
                    GPIO->AuxLedCtrl |= (LED_STEADY_ON << AUX_MODE_SHFT_2);
            }
#endif
 #if 1 /*  Jayden@MSTC for To2 ETH LED control,20130521 */  
 		 if(ledName==kLedEth){
		 	bcmsw_pmdio_wreg(0,18,LedCtlOn,2);/*ethswctl -c regaccess -n 1 -v 0x12 -l 2 -d 0x321*/
			IsEthLEDOn=TRUE;
 		 }
		 else
 	 		setLed (pLed, kLedOn, kLedGreen);
#if 1 // for Internet LED
		 if (ledName == kLedInternet) {
		 	LedInternetOn = TRUE;
		 }
#endif
 #else
            setLed (pLed, kLedOn, kLedGreen);
 #endif
            break;

        case kLedStateOff:
		 if(ledName==kLedEth){
		 	bcmsw_pmdio_wreg(0,18,LedCtlOff,2);/*ethswctl -c regaccess -n 1 -v 0x12 -l 2 -d 0x021*/
			IsEthLEDOn=FALSE;
 		 }	
#if 1 // for Internet LED
		 if (ledName == kLedInternet) {
			LedInternetOn = FALSE;
		 }
#endif
            break;
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120710*/
#ifdef MTS_LED_CONTROL
        /* LED , __TELEFONICA__, Mitrastar Curtis, 20120106 */
        case ALLLedStateOn:                
            setLed(pLed, kLedOn, kLedGreen);
            setLed(pLed, kLedOn, kLedRed);
            break;
        case ALLLedStateOff:
            setLed(pLed, kLedOff, kLedGreen);
            setLed(pLed, kLedOff, kLedRed);
            break;
#endif /*MTS_LED_CONTROL*/
        case kLedStateFail:
            setLed (pLed, kLedOn, kLedRed);
            break;
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120710*/
#ifdef MTS_LED_CONTROL
        case kLedStateFailOff:
            setLed (pLed, kLedOff, kLedRed);
            break;
#endif /*MTS_LED_CONTROL*/
        case kLedStateSlowBlinkContinues:
            pLed->blinkCountDown = kSlowBlinkCount;
            gTimerOnRequests++;
            break;

        case kLedStateFastBlinkContinues:
            pLed->blinkCountDown = kFastBlinkCount;
            gTimerOnRequests++;
            break;
			
#if 1 /*For To2 LED behavior,MitraStar Jayden,20130117*/
	case  kLedEthFrqSet:/*Jayden added for ETH LED data transmission flash Frequence,MitraStar,20130911*/
		 if(ledName==kLedEthFrqControl){
		 	bcmsw_pmdio_wreg(0,15,LanLedCtlFrq,1);/*ethswctl -c regaccess -n 1 -v 0xf -l 1 -d 0x87*/
	     	}
		break;
	case  kLedInternetDataTimerDisable:/*Jayden added for disable Internet LED data transmission timer,MitraStar,20130910*/
	     if(ledName==kLedInternetDataTimer){
		 	LedInternetDataTimerOn=FALSE;
	     }
			break;
	case kLedPwrStateRedfor1sandGreen:
	     setLed (pLed, kLedOn, kLedRed);
	     pLed->blinkCountDown = kOneSecCount;/*for 1sec*/
            gTimerOnRequests++;
            break;
	case kLedEthStateSlowBlinkContinues:
            pLed->blinkCountDown = kSlowBlinkCount;
            gTimerOnRequests++;
            break;
	case kLedStateBlinkContinues:
            pLed->blinkCountDown = kBlinkCount;
            gTimerOnRequests++;
            break;

	case kLedFailStateSlowBlinkContinues:
            pLed->blinkCountDown = kSlowBlinkCount;
            gTimerOnRequests++;
            break;

        case kLedFailStateFastBlinkContinues:
            pLed->blinkCountDown = kFastBlinkCount;
            gTimerOnRequests++;
            break;	

	case kLedFailStateBlinkContinues:
            pLed->blinkCountDown = kBlinkCount;
            gTimerOnRequests++;
            break;		
#endif
        case kLedStateUserWpsInProgress:          /* 200ms on, 100ms off */
            pLed->blinkCountDown = kFastBlinkCount;
            gLedRunningCounter = 0;
            gTimerOnRequests++;
            break;             

        case kLedStateUserWpsError:               /* 100ms on, 100ms off */
            pLed->blinkCountDown = kFastBlinkCount;
            gLedRunningCounter = 0;
            gTimerOnRequests++;
            break;        

        case kLedStateUserWpsSessionOverLap:      /* 100ms on, 100ms off, 5 times, off for 500ms */
            pLed->blinkCountDown = kFastBlinkCount;
            gTimerOnRequests++;
            break;        

        default:
            printk("Invalid led state\n");
        }
    }

    // If gTimerOn is false, that means ledTimerExpire has already finished
    // running and has not restarted the timer.  Then we can start it here.
    // Otherwise, we only leave the gTimerOnRequests > 0 which will be
    // noticed at the end of the ledTimerExpire function.
    if (!gTimerOn && gTimerOnRequests > 0)
    {
        ledTimerStart();
        gTimerOn = TRUE;
        gTimerOnRequests = 0;
    }
    spin_unlock_irqrestore(&brcm_ledlock, flags);
}
