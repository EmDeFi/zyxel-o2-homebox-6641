/*
<:copyright-BRCM:2002:GPL/GPL:standard

   Copyright (c) 2002 Broadcom Corporation
   All Rights Reserved

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
/***********************************************************************/
/*                                                                     */
/*   MODULE:  board.h                                                  */
/*   PURPOSE: Board specific information.  This module should include  */
/*            all base device addresses and board specific macros.     */
/*                                                                     */
/***********************************************************************/
#ifndef _BOARD_H
#define _BOARD_H

#include "bcm_hwdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* BOARD_H_API_VER increases when other modules (such as PHY) depend on */
/* a new function in the board driver or in boardparms.h                */

#define BOARD_H_API_VER 3

/*****************************************************************************/
/*          board ioctl calls for flash, led and some other utilities        */
/*****************************************************************************/
/* Defines. for board driver */
#define BOARD_IOCTL_MAGIC       'B'
#define BOARD_DRV_MAJOR          206

#define BOARD_IOCTL_FLASH_WRITE                 _IOWR(BOARD_IOCTL_MAGIC, 0, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_FLASH_READ                  _IOWR(BOARD_IOCTL_MAGIC, 1, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_DUMP_ADDR                   _IOWR(BOARD_IOCTL_MAGIC, 2, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_SET_MEMORY                  _IOWR(BOARD_IOCTL_MAGIC, 3, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_MIPS_SOFT_RESET             _IOWR(BOARD_IOCTL_MAGIC, 4, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_LED_CTRL                    _IOWR(BOARD_IOCTL_MAGIC, 5, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_PSI_SIZE                _IOWR(BOARD_IOCTL_MAGIC, 6, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_SDRAM_SIZE              _IOWR(BOARD_IOCTL_MAGIC, 7, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_ID                      _IOWR(BOARD_IOCTL_MAGIC, 8, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_CHIP_ID                 _IOWR(BOARD_IOCTL_MAGIC, 9, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_CHIP_REV                _IOWR(BOARD_IOCTL_MAGIC, 10, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_CFE_VER                 _IOWR(BOARD_IOCTL_MAGIC, 11, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_BASE_MAC_ADDRESS        _IOWR(BOARD_IOCTL_MAGIC, 12, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_MAC_ADDRESS             _IOWR(BOARD_IOCTL_MAGIC, 13, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_RELEASE_MAC_ADDRESS         _IOWR(BOARD_IOCTL_MAGIC, 14, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_NUM_ENET_MACS           _IOWR(BOARD_IOCTL_MAGIC, 15, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_NUM_ENET_PORTS          _IOWR(BOARD_IOCTL_MAGIC, 16, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_SET_MONITOR_FD              _IOWR(BOARD_IOCTL_MAGIC, 17, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_WAKEUP_MONITOR_TASK         _IOWR(BOARD_IOCTL_MAGIC, 18, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_SET_TRIGGER_EVENT           _IOWR(BOARD_IOCTL_MAGIC, 19, BOARD_IOCTL_PARMS)        
#define BOARD_IOCTL_GET_TRIGGER_EVENT           _IOWR(BOARD_IOCTL_MAGIC, 20, BOARD_IOCTL_PARMS)        
#define BOARD_IOCTL_UNSET_TRIGGER_EVENT         _IOWR(BOARD_IOCTL_MAGIC, 21, BOARD_IOCTL_PARMS) 
#define BOARD_IOCTL_GET_WLAN_ANT_INUSE          _IOWR(BOARD_IOCTL_MAGIC, 22, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_SET_SES_LED                 _IOWR(BOARD_IOCTL_MAGIC, 23, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_SET_CS_PAR                  _IOWR(BOARD_IOCTL_MAGIC, 25, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_SET_GPIO                    _IOWR(BOARD_IOCTL_MAGIC, 26, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_FLASH_LIST                  _IOWR(BOARD_IOCTL_MAGIC, 27, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_BACKUP_PSI_SIZE         _IOWR(BOARD_IOCTL_MAGIC, 28, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_SYSLOG_SIZE             _IOWR(BOARD_IOCTL_MAGIC, 29, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_SET_SHUTDOWN_MODE           _IOWR(BOARD_IOCTL_MAGIC, 30, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_SET_STANDBY_TIMER           _IOWR(BOARD_IOCTL_MAGIC, 31, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_BOOT_IMAGE_OPERATION        _IOWR(BOARD_IOCTL_MAGIC, 32, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_TIMEMS                  _IOWR(BOARD_IOCTL_MAGIC, 33, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_GPON_OPTICS_TYPE        _IOWR(BOARD_IOCTL_MAGIC, 34, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_DEFAULT_OPTICAL_PARAMS  _IOWR(BOARD_IOCTL_MAGIC, 35, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_ALLOC_MAC_ADDRESSES         _IOWR(BOARD_IOCTL_MAGIC, 36, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_RELEASE_MAC_ADDRESSES       _IOWR(BOARD_IOCTL_MAGIC, 37, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_SPI_SLAVE_INIT              _IOWR(BOARD_IOCTL_MAGIC, 38, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_SPI_SLAVE_READ              _IOWR(BOARD_IOCTL_MAGIC, 39, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_SPI_SLAVE_WRITE             _IOWR(BOARD_IOCTL_MAGIC, 40, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_NUM_FE_PORTS            _IOWR(BOARD_IOCTL_MAGIC, 41, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_NUM_GE_PORTS            _IOWR(BOARD_IOCTL_MAGIC, 42, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_NUM_VOIP_PORTS          _IOWR(BOARD_IOCTL_MAGIC, 43, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_SWITCH_PORT_MAP         _IOWR(BOARD_IOCTL_MAGIC, 44, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_PERS_NAME               _IOWR(BOARD_IOCTL_MAGIC, 45, BOARD_IOCTL_PARMS)
/* __MTS__, Paul Ho, __AT&T__, __P870HNP__ ,Autumn*/
#if defined (WIRELESS) && ( defined(CONFIG_BCM96368) || defined(CONFIG_BCM96328) || defined(CONFIG_BCM963268))
#define BOARD_IOCTL_GET_PROCCESS_PID    _IOWR(BOARD_IOCTL_MAGIC, 49, BOARD_IOCTL_PARMS) 
#endif

/*KeyYang@MSTC: Merge from Telefonica_Common, 20120711*/
#ifdef MTS_TEST_COMMAND_SUPPORT
/*__MTS__, Dennis*/
#define BOARD_IOCTL_SET_BUTTONTEST      _IOWR(BOARD_IOCTL_MAGIC, 50, BOARD_IOCTL_PARMS)

/*__MTS__, RaynorChung: Support USB test CMD for 963268 platform*/
#define BOARD_IOCTL_SET_USBTEST         _IOWR(BOARD_IOCTL_MAGIC, 51, BOARD_IOCTL_PARMS)
#endif /*MTS_TEST_COMMAND_SUPPORT*/

/*Mitrastar, Curtis, Push Button*/
#ifdef MTS_RESTORE_TO_DEFAULT 
#define BOARD_IOCTL_GET_SSK_PID      _IOWR(BOARD_IOCTL_MAGIC, 52, BOARD_IOCTL_PARMS)
#endif

/*KeyYang@MSTC Quqntenna RMT_QCSAPI Porting, 20120303*/
#ifdef QUANTENNA_WLAN_BTN_CONTROL
#define BOARD_IOCTL_SET_RPCD_PID    _IOWR(BOARD_IOCTL_MAGIC, 53, BOARD_IOCTL_PARMS)
#endif

#define BOARD_IOCTL_GET_DECT_PROCCESS_PID    _IOWR(BOARD_IOCTL_MAGIC, 54, BOARD_IOCTL_PARMS) 

/*Mini FTTH need to support WPS Error LED, KeyYang@MSTC 20130110*/
#ifdef MTS_WPS_ERR_LED_SUPPORTED
#define BOARD_IOCTL_SET_WPS_ERR_LED                 _IOWR(BOARD_IOCTL_MAGIC, 55, BOARD_IOCTL_PARMS)
#endif

#ifdef MSTC_TO2_GERMANY_CUSTOMIZATION //chenhe for tr69 fault code
#define BOARD_IOCTL_FW_UPGRADE_STATUS                 _IOWR(BOARD_IOCTL_MAGIC, 56, BOARD_IOCTL_PARMS)
#endif
#ifdef MSTC_TO2_GERMANY_CUSTOMIZATION /*Jayden added for WLAN button,MitraStar,20130828*/
#define BOARD_IOCTL_WLAN_BUTTON_LOCK                 _IOWR(BOARD_IOCTL_MAGIC, 57, BOARD_IOCTL_PARMS)
#endif

#if 1 /* Support the CLI command to test incorrect bad block tables(BBT), __CHT__, MitraStar SeanLu, 20140328 */
/*ChenHe@MSTC merge from http://svn.mitrastar.com.tw/svn/CPE_TRUNK/BRCM_412/Products/DSL-401GNU-B1B_V2_CTL/132VFX0. r16481*/
#define BOARD_IOCTL_BRCMNAND_BBT_READ             _IOWR(BOARD_IOCTL_MAGIC, 58, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_BRCMNAND_BBT_WRITE            _IOWR(BOARD_IOCTL_MAGIC, 59, BOARD_IOCTL_PARMS)
#endif

// for the action in BOARD_IOCTL_PARMS for flash operation
typedef enum 
{
    PERSISTENT,
    NVRAM,
    BCM_IMAGE_CFE,
    BCM_IMAGE_FS,
    BCM_IMAGE_KERNEL,
    BCM_IMAGE_WHOLE,
    SCRATCH_PAD,
    FLASH_SIZE,
    SET_CS_PARAM,
    BACKUP_PSI,
    SYSLOG,
/*__MTS__, RaynorChung: Support 963268 nand flash, patch form SVN#3597 on http://svn.zyxel.com.tw/svn/CPE_SW1/BCM96368/trunk/P-870HA/branches/cht/fttb8/4.11*/
#ifdef MTS_NAND_FLASH_SUPPORT
    FILETAG,
#endif
    /*MitraStar, Elina*/
    UPDATE_CONFIG,
#ifdef ZYXEL_OBM_IMAGE_DEFAULT /* Image Default, __CHT__, MitraStar Curtis, 20111124. */
    IMAGETAG,
    IMGDEF_PSI,
#endif
#ifdef MSTC_TO2_GERMANY_CUSTOMIZATION //chenhe for tr69 fault code
    WRITE_PARTITION_STATUS,
    READ_PARTITION_STATUS,
#endif
#if (defined(MSTC_FWUP_FROM_FLASH) || defined(CONFIG_BUILD_MSTC_FWUP_FROM_FLASH))
    IMAGE_FS_FLASH
#endif
} BOARD_IOCTL_ACTION;
    
typedef struct boardIoctParms
{
    char *string;
    char *buf;
    int strLen;
    int offset;
    BOARD_IOCTL_ACTION  action;        /* flash read/write: nvram, persistent, bcm image */
    int result;
} BOARD_IOCTL_PARMS;

// LED defines 
typedef enum
{   
#ifdef MSTC_TO2_GERMANY_CUSTOMIZATION /*MSTC, chris*/
    kLedAdsl_TO2,
#endif
    kLedAdsl,
    kLedSecAdsl,
    kLedWanData,
    kLedWanError,
    kLedSes,
/*Mini FTTH need to support WPS Error LED, KeyYang@MSTC 20130110*/
#ifdef MTS_WPS_ERR_LED_SUPPORTED
	kLedWpsErr,
#endif    
    kLedVoip,
    kLedVoip1,
    kLedVoip2,
    kLedVoip1Error,
    kLedVoip2Error,
    kLedPots,
    kLedDect,
    kLedGpon,
    /*__MTS__, Dennis*/
    kLedMoCAL,
    kLedPowerG,
    kLedMoCAW,
    kLedEth0,
    kLedEth1,
    kLedEth2,
    kLedEth3,
    kLedEth0Duplex,
    kLedEth0Spd100,
    kLedEth0Spd1000,
    kLedEth1Duplex,
    kLedEth1Spd100,
    kLedEth1Spd1000,
    /*KeyYang@MSTC: Merge from Telefonica_Common, 20120702*/
    kLedUSB0,
    kLedUSB0Fail,
    kLedUSB1,
    kLedUSB1Fail,
    /*KeyYang@MSTC Quqntenna RMT_QCSAPI Porting, 20120528*/
    kLed5GWpsFail,
#ifdef MSTC_TO2_GERMANY_CUSTOMIZATION  /* Jayden@MSTC for LED control*/
  kLedWlan5G,
   kLedWlan,
    kLedSys,
    kLedInternet,
    kLedService,
    kLedWating,       
   kLedPhone,
   kLedInfo,
   kLedEth,
   kLedInternetDataTimer,/*Jayden added for disable Internet LED data transmission timer,MitraStar,20130910*/
   kLedEthFrqControl,/*Jayden added for ETH LED data transmission flash Frequence,MitraStar,20130911*/
#endif
    kLedEnd                             // NOTE: Insert the new led name before this one.
} BOARD_LED_NAME;

typedef enum
{
    kLedStateOff,                        /* turn led off */
    kLedStateOn,                         /* turn led on */
    /*KeyYang@MSTC: Merge from Telefonica_Common, 20120702*/
    ALLLedStateOn,
    ALLLedStateOff,
    kLedStateFail,                       /* turn led on red */
    /*KeyYang@MSTC: Merge from Telefonica_Common, 20120702*/
    kLedStateFailOff,                       /* turn led off red */
#ifdef MSTC_TO2_GERMANY_CUSTOMIZATION /*For To2 LED behavior,MitraStar Jayden,20130117*/
    kLedStateSlowBlinkContinues,         /* slow blink continues at 0.5HZ interval for green*/
    kLedStateBlinkContinues,		      /*blink continues at 1HZ interval for green*/
    kLedStateFastBlinkContinues,         /* fast blink continues at 2HZ interval for green*/	
    kLedFailStateSlowBlinkContinues,         /* slow blink continues at 0.5HZ interval for red*/
    kLedFailStateBlinkContinues,		      /*blink continues at 1HZ interval for red*/
    kLedFailStateFastBlinkContinues,         /* fast blink continues at 2HZ interval for red*/	
    kLedEthStateSlowBlinkContinues,/* slow blink continues at 0.5HZ interval for Eth led,20130521*/
    kLedPwrStateRedfor1sandGreen,/* for power led to red 1s and trun green,20130604*/
    kLedInternetDataTimerDisable,/*Jayden added for disable Internet LED data transmission timer,MitraStar,20130910*/
    kLedEthFrqSet,/*Jayden added for ETH LED data transmission flash Frequence,MitraStar,20130911*/
#else
    kLedStateSlowBlinkContinues,         /* slow blink continues at 2HZ interval */
    kLedStateFastBlinkContinues,         /* fast blink continues at 4HZ interval */
#endif    
    kLedStateUserWpsInProgress,          /* 200ms on, 100ms off */
    kLedStateUserWpsError,               /* 100ms on, 100ms off */
    kLedStateUserWpsSessionOverLap,       /* 100ms on, 100ms off, 5 times, off for 500ms */                     
    /*KeyYang@MSTC: Merge from Telefonica_Common, 20120702*/
    kLedStateBlinkOnce
} BOARD_LED_STATE;

typedef void (*HANDLE_LED_FUNC)(BOARD_LED_STATE ledState);

typedef enum
{
    kGpioInactive,
    kGpioActive
} GPIO_STATE_t;
#if 1 /* Support the CLI command to test incorrect bad block tables(BBT), __CHT__, MitraStar SeanLu, 20140328 */
/*ChenHe@MSTC merge from http://svn.mitrastar.com.tw/svn/CPE_TRUNK/BRCM_412/Products/DSL-401GNU-B1B_V2_CTL/132VFX0. r16481*/
typedef enum
{
    BrcmBBTMinSave,
    BrcmBBTMaxSave,
    BrcmBBTRun
} BRCMNAND_ACTION_t;
#endif

// FLASH_ADDR_INFO is now defined in flash_common.h
#include "flash_common.h"

typedef struct cs_config_pars_tag
{
  int   mode;
  int   base;
  int   size;
  int   wait_state;
  int   setup_time;
  int   hold_time;
} cs_config_pars_t;

#if (defined(MSTC_FWUP_FROM_FLASH) || defined(CONFIG_BUILD_MSTC_FWUP_FROM_FLASH))
typedef struct fwup_flash_parm_s {
	char filename[64];
	unsigned int size;
	unsigned int offset;
} fwup_flash_parm_t;
#endif
typedef enum
{
    MAC_ADDRESS_OP_GET=0,
    MAC_ADDRESS_OP_RELEASE,
    MAC_ADDRESS_OP_MAX
} MAC_ADDRESS_OPERATION;

typedef void (* kerSysMacAddressNotifyHook_t)(unsigned char *pucaMacAddr, MAC_ADDRESS_OPERATION op);

#define UBUS_BASE_FREQUENCY_IN_MHZ  160

#define IF_NAME_ETH    "eth"
#define IF_NAME_USB    "usb"
#define IF_NAME_WLAN   "wl"
#define IF_NAME_MOCA   "moca"
#define IF_NAME_ATM    "atm"
#define IF_NAME_PTM    "ptm"
#define IF_NAME_GPON   "gpon"
#define IF_NAME_EPON   "epon"
#define IF_NAME_VEIP    "veip"

#define MAC_ADDRESS_ANY         (unsigned long) -1

/* A unique mac id is required for a WAN interface to request for a MAC address.
 * The 32bit mac id is formated as follows:
 *     bit 28-31: MAC Address IF type (MAC_ADDRESS_*)
 *     bit 20-27: the unit number. 0 for atm0, 1 for atm1, ...
 *     bit 12-19: the connection id which starts from 1.
 *     bit 0-11:  not used. should be zero.
 */
#define MAC_ADDRESS_PLC         0x10000000
#define MAC_ADDRESS_EPONONU     0x20000000
#define MAC_ADDRESS_EPON        0x40000000
#define MAC_ADDRESS_GPON        0x80000000
#define MAC_ADDRESS_ETH         0xA0000000
#define MAC_ADDRESS_USB         0xB0000000
#define MAC_ADDRESS_WLAN        0xC0000000
#define MAC_ADDRESS_MOCA        0xD0000000
#define MAC_ADDRESS_ATM         0xE0000000
#define MAC_ADDRESS_PTM         0xF0000000

/*****************************************************************************/
/*          Function Prototypes                                              */
/*****************************************************************************/
#if !defined(__ASM_ASM_H)
void dumpaddr( unsigned char *pAddr, int nLen );
extern void kerSysEarlyFlashInit( void );
extern int kerSysGetChipId( void );
#if defined(CONFIG_11ac_throughput_patch_from_412L08)
char * kerSysGetChipName( char *buf, int len);
#endif
extern int kerSysGetDslPhyEnable( void );
extern void kerSysFlashInit( void );
extern void kerSysFlashAddrInfoGet(PFLASH_ADDR_INFO pflash_addr_info);
extern int kerSysCfeVersionGet(char *string, int stringLength);
/* Image Default, __CHT__, MitraStar Curtis, 20111124. */
#ifdef ZYXEL_OBM_IMAGE_DEFAULT 
void kerSysFlashBadBlockNumSet(int number);
#endif

extern int kerSysNvRamSet(const char *string, int strLen, int offset);
/*__MTS__, RaynorChung: Support 963268 nand flash, patch form SVN#3597 on http://svn.zyxel.com.tw/svn/CPE_SW1/BCM96368/trunk/P-870HA/branches/cht/fttb8/4.11*/
#ifdef MTS_NAND_FLASH_SUPPORT
extern int kerSysNvRamGet(char *string, int strLen, int offset);
#else
extern void kerSysNvRamGet(char *string, int strLen, int offset);
#endif /*MTS_NAND_FLASH_SUPPORT*/
extern void kerSysNvRamGetBootline(char *bootline);
extern void kerSysNvRamGetBootlineLocked(char *bootline);
extern void kerSysNvRamGetBoardId(char *boardId);
extern void kerSysNvRamGetBoardIdLocked(char *boardId);
extern void kerSysNvRamGetBaseMacAddr(unsigned char *baseMacAddr);
extern unsigned long kerSysNvRamGetVersion(void);
/*MitraStar, Elina*/
extern int kerSysUpdateConfigGet(char *string, int strLen, int offset);
extern int kerSysFsFileSet(const char *filename, char *buf, int len);
extern int kerSysPersistentGet(char *string, int strLen, int offset);
extern int kerSysPersistentSet(char *string, int strLen, int offset);
extern int kerSysBackupPsiGet(char *string, int strLen, int offset);
extern int kerSysBackupPsiSet(char *string, int strLen, int offset);
extern int kerSysSyslogGet(char *string, int strLen, int offset);
extern int kerSysSyslogSet(char *string, int strLen, int offset);
extern int kerSysScratchPadList(char *tokBuf, int tokLen);
extern int kerSysScratchPadGet(char *tokName, char *tokBuf, int tokLen);
extern int kerSysScratchPadSet(char *tokName, char *tokBuf, int tokLen);
extern int kerSysScratchPadClearAll(void);
/*__MTS__, RaynorChung: Support 963268 nand flash, patch form SVN#3597 on http://svn.zyxel.com.tw/svn/CPE_SW1/BCM96368/trunk/P-870HA/branches/cht/fttb8/4.11*/
#ifdef MTS_NAND_FLASH_SUPPORT
extern int kerSysBcmImageSet( int flash_start_addr, char *string, int size, int* badblknum); //CLK
#else
extern int kerSysBcmImageSet( int flash_start_addr, char *string, int size,  int should_yield);
#endif /*MTS_NAND_FLASH_SUPPORT*/
extern int kerSysBcmNandImageSet( char *rootfs_part, char *string, int img_size, int should_yield );
extern int kerSysSetBootImageState( int state );
extern int kerSysGetBootImageState( void );
extern int kerSysSetOpticalPowerValues(unsigned short rxReading, unsigned short rxOffset, unsigned short txReading);
extern int kerSysGetOpticalPowerValues(unsigned short *prxReading, unsigned short *prxOffset, unsigned short *ptxReading);
extern int kerSysBlParmsGetInt( char *name, int *pvalue );
extern int kerSysBlParmsGetStr( char *name, char *pvalue, int size );
extern unsigned long kerSysGetMacAddressType( unsigned char *ifName );
extern int kerSysMacAddressNotifyBind(kerSysMacAddressNotifyHook_t hook);
extern int kerSysGetMacAddress( unsigned char *pucaAddr, unsigned long ulId );
extern int kerSysReleaseMacAddress( unsigned char *pucaAddr );
extern void kerSysGetGponSerialNumber( unsigned char *pGponSerialNumber);
extern void kerSysGetGponPassword( unsigned char *pGponPassword);
extern int kerSysGetSdramSize( void );
#if defined(CONFIG_BCM96368)
extern unsigned int kerSysGetSdramWidth( void );
#endif
extern void kerSysGetBootline(char *string, int strLen);
extern void kerSysSetBootline(char *string, int strLen);
extern void kerSysMipsSoftReset(void);
extern void kerSysLedCtrl(BOARD_LED_NAME, BOARD_LED_STATE);
/* 3G LED, __TELEFONICA__, MitraStar Chehuai, 20110708. */
#ifdef MTS_TELEFONICA_WWAN 
extern void kerSys3GLedCtrl(BOARD_LED_STATE);
#endif /*MTS_TELEFONICA_WWAN*/
extern int kerSysFlashSizeGet(void);
extern int kerSysMemoryMappedFlashSizeGet(void);
extern unsigned long kerSysReadFromFlash( void *toaddr, unsigned long fromaddr, unsigned long len );
extern void kerSysRegisterDyingGaspHandler(char *devname, void *cbfn, void *context);
extern void kerSysDeregisterDyingGaspHandler(char *devname);    
extern int kerConfigCs(BOARD_IOCTL_PARMS *parms);
/* private functions used within board driver only */
void stopOtherCpu(void);
void resetPwrmgmtDdrMips(void);

/*
 * Access to shared GPIO registers should be done by calling these
 * functions below, which will grab a spinlock while accessing the
 * GPIO register.  However, if your code needs to access a shared
 * GPIO register but cannot call these functions, you should still
 * acquire the spinlock.
 * (Technically speaking, I should include spinlock.h and declare extern here
 * but that breaks the C++ compile of xtm.)
 */
//#include <linux/spinlock.h>
//extern spinlock_t bcm_gpio_spinlock;
extern void kerSysSetGpioState(unsigned short bpGpio, GPIO_STATE_t state);
extern void kerSysSetGpioStateLocked(unsigned short bpGpio, GPIO_STATE_t state);
extern void kerSysSetGpioDir(unsigned short bpGpio);
extern void kerSysSetGpioDirLocked(unsigned short bpGpio);
/*Mitrastar, Curtis, Push Button*/
#if defined(MTS_RESTORE_TO_DEFAULT) || defined(MTS_WPS_BTN_SCE) || defined(QUANTENNA_WLAN_BTN_CONTROL) || defined(MSTC_TO2_GERMANY_CUSTOMIZATION)
extern int kerSysGetGpioDir(unsigned short bpGpio);
extern int kerSysGetGpioDirLocked(unsigned short bpGpio);
#endif
extern int kerSysSetGpioDirInput(unsigned bpGpio);
extern int kerSysGetGpioValue(unsigned bpGpio);

// for the voice code, which has too many kernSysSetGpio to change
#define kerSysSetGpio kerSysSetGpioState


extern unsigned long kerSysGetUbusFreq(unsigned long miscStrapBus);
extern int kerSysGetAfeId( unsigned long *afeId );
#define __kerSysGetAfeId  kerSysGetAfeId

typedef void (*MocaHostIntrCallback)(void);
extern void kerSysRegisterMocaHostIntrCallback(MocaHostIntrCallback callback);


#if defined(CONFIG_BCM_CPLD1)
int BcmCpld1Initialize(void);
void BcmCpld1CheckShutdownMode(void);
void BcmCpld1SetShutdownMode(void);
void BcmCpld1SetStandbyTimer(unsigned int duration);
#endif

#if defined (CONFIG_BCM_AVS_PWRSAVE)
extern void kerSysBcmEnableAvs(int enable);
extern int kerSysBcmAvsEnabled(void);
#endif

#ifdef ZYXEL_OBM_IMAGE_DEFAULT	/* Image Default, __CHT__, MitraStar Curtis, 20111124. */
extern int kerSysImgDefGet(char *string, int strLen, int offset);
extern int kerSysImgDefTagGet(char *string);
#endif

#if defined(CONFIG_BCM_DDR_SELF_REFRESH_PWRSAVE)
#define CONFIG_BCM_PWRMNGT_DDR_SR_API
// The structure below is to be declared in ADSL PHY MIPS LMEM, if ADSL is compiled in
typedef struct _PWRMNGT_DDR_SR_CTRL_ {
  union {
   struct {
      unsigned char   phyBusy;
      unsigned char   tp0Busy;
      unsigned char   tp1Busy;
      unsigned char   reserved;
    };
    unsigned int      word;
  };
} PWRMNGT_DDR_SR_CTRL;

void BcmPwrMngtRegisterLmemAddr(PWRMNGT_DDR_SR_CTRL *pDdrSr);
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif /* _BOARD_H */

