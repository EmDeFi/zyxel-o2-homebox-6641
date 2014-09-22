/*
    Copyright 2000-2010 Broadcom Corporation

    Unless you and Broadcom execute a separate written software license
    agreement governing use of this software, this software is licensed
    to you under the terms of the GNU General Public License version 2
    (the “GPL?, available at http://www.broadcom.com/licenses/GPLv2.php,
    with the following added to such license:

        As a special exception, the copyright holders of this software give
        you permission to link this software with independent modules, and to
        copy and distribute the resulting executable under terms of your
        choice, provided that you also meet, for each linked independent
        module, the terms and conditions of the license of that module. 
        An independent module is a module which is not derived from this
        software.  The special exception does not apply to any modifications
        of the software.

    Notwithstanding the above, under no circumstances may you combine this
    software in any way with any other Broadcom software provided under a
    license other than the GPL, without Broadcom's express prior written
    consent.
*/                       

/**************************************************************************
 * File Name  : bcmnetlink.h
 *
 * Description: This file defines broadcom specific netlink message types
 ***************************************************************************/
#ifndef _BCMNETLINK_H
#define _BCMNETLINK_H

#include<linux/netlink.h>

#ifndef NETLINK_BRCM_MONITOR
#define NETLINK_BRCM_MONITOR 25
#endif

/* message types exchanged using NETLINK_BRCM_MONITOR */
#define MSG_NETLINK_BRCM_WAKEUP_MONITOR_TASK 0X1000

#define MSG_NETLINK_BRCM_LINK_STATUS_CHANGED 0X2000

#define MSG_NETLINK_BRCM_LINK_TRAFFIC_TYPE_MISMATCH 0X4000

/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_TELEFONICA_CUSTOMIZATION
/*David SUPPORT_TELEFONICA */
#define MSG_NETLINK_BRCM_IPV6_DEFAULT_ROUTER 0XB000

#define MSG_NETLINK_BRCM_IPV6_RA_PREFIX_ADDR 0X5000
#endif /*MSTC_TELEFONICA_CUSTOMIZATION*/

#define MSG_NETLINK_BRCM_SAVE_DSL_CFG		0X8000	/* Why the previous defined values are like bitmap???, just follow */

#define MSG_NETLINK_BRCM_CALLBACK_DSL_DRV	0X0100

#define MSG_NETLINK_BRCM_DECT_REGISTER	0X0200 /* DECT push button*/
#define MSG_NETLINK_BRCM_DECT_PAGING	0X0400 /* DECT push button*/

#ifdef MSTC_TO2_GERMANY_CUSTOMIZATION
/*[IreneWang@MSTC:TO2]$ Collect information for DeviceLog, 20130408*/
#define MSG_NETLINK_BRCM_SAVE_DSL_TRAINING_TIME 0X0800	/*Fellow spec to save DSL Training Time*/

/*[IreneWang@MSTC:TO2]$ Maintain the LPD printer queueName and deviceNode, 20130801*/
#define MSG_NETLINK_USB_PRINTER_INFO 0X0C00			/*For update the LPD printer information*/

/*MSTC chris for reset button, 20130429*/
#define MSG_NETLINK_BRCM_SYS_RESET 0X0020
/*MSTC chris for reset button, 20130429*/

#define MSG_NETLINK_BRCM_SYS_RESTOREDEFAULT 0X0040		/*ChenHe@MSTC for sending restoredefault event by netlink. 20130525*/

#define MSG_NETLINK_BRCM_IP_UPDATE 0X0080 //__TO2__,JhenYang, 2013-09-12

#define MSG_NETLINK_BRCM_TE_EVENT	0XC000	/*For TE testing, MSTC Memphis Chung, 20130521*/

#endif  /*MSTC_TO2_GERMANY_CUSTOMIZATION*/

extern void kerSysSendtoMonitorTask(int msgType, char *msgData, int msgDataLen);

#endif /*_BCMNETLINK_H */
