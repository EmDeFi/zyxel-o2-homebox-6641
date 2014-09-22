/***********************************************************************
 *
 *  Copyright (c) 2006-2007  Broadcom Corporation
 *  All Rights Reserved
 *
 * <:label-BRCM:2011:DUAL/GPL:standard
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as published by
 * the Free Software Foundation (the "GPL").
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * 
 * A copy of the GPL is available at http://www.broadcom.com/licenses/GPLv2.php, or by
 * writing to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 * 
:>
 *
 ************************************************************************/

#include "cms.h"
#include "cms_util.h"
#include "oal.h"


static CmsEntityInfo entityInfoArray[] = {

   {EID_TR69C,        /* eid */
    NDA_ACCESS_TR69C, /* TR69 attribute access bit */
    "tr69c",          /* name */
    "/bin/tr69c",     /* path */
    "",               /* run args */
#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
#if 1	/*[IreneWang@MSTC:TO2]$ TR69 should start after NTP send message, 20121228*/
    EIF_MDM|EIF_SERVER|EIF_SERVER_TCP|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE|EIF_IPV6,
#else
    EIF_MDM|EIF_SERVER|EIF_SERVER_TCP|EIF_LAUNCH_ON_BOOT|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE|EIF_IPV6,
#endif  /*1*/    
#else
#if 1	/*[IreneWang@MSTC:TO2]$ TR69 should start after NTP send message, 20121228*/
    EIF_MDM|EIF_SERVER|EIF_SERVER_TCP|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE,
#else
    EIF_MDM|EIF_SERVER|EIF_SERVER_TCP|EIF_LAUNCH_ON_BOOT|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE,
#endif  /*1*/    
#endif
    1,                /* backlog for TCP server port */
    TR69C_CONN_REQ_PORT,       /* TCP server port */
#ifdef SUPPORT_TR69C
#if 1	/*[IreneWang@MSTC:TO2]$ Init TR69 socket only after NTP synchronize finish, 20140317*/
    FALSE,            /* isFeatureCompiledIn */
#else
    TRUE,             /* isFeatureCompiledIn */
#endif	/* 1 */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},         /* misc mem usage parameters */

   {EID_TR64C,
    NDA_ACCESS_TR64C,
    "tr64c",
    "/bin/tr64c",
    "",
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifndef MSTC_COMMON_PATCH
    EIF_MDM|EIF_LAUNCH_ON_BOOT,
#else /*MSTC_COMMON_PATCH*/
    /*__MTS__, Justin, Support Enable and Disable by smd*/
    EIF_MDM|EIF_MESSAGING_CAPABLE,
#endif /*MSTC_COMMON_PATCH*/
    0,
    TR64C_HTTP_CONN_PORT,
#ifdef SUPPORT_TR64C
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_HTTPD,
    NDA_ACCESS_HTTPD,
    "httpd",
/*__MTS__, Nick Tseng, GUI Debug Mode with FTP*/
#ifdef SUPPORT_GUI_DEBUG_MODE
    "/var/httpd",
#else
    "/bin/httpd",
#endif /*SUPPORT_GUI_DEBUG_MODE*/
    "",
#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
    EIF_MDM|EIF_SERVER|EIF_SERVER_TCP|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE|EIF_IPV6,
#else
    EIF_MDM|EIF_SERVER|EIF_SERVER_TCP|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE,
#endif
    5,                /* backlog for TCP server port */ /*MSTC extend socket queue from 3 to 5*/
    HTTPD_PORT,       /* TCP server port */
#ifdef SUPPORT_HTTPD
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif

    0, 0, 0},

/*WenJuin@MSTC:EID_HTTPD_SSL_SERVICE init most before EID_HTTPD_SSL init,20130604 */
#if 1 /*For To2 Service Remote GUI, MSTC ChaoWen, 20130215*/
   {EID_HTTPD_SSL_SERVICE,
    NDA_ACCESS_HTTPD,
    "httpd_service",
    "/bin/httpd",
    " -s -S",
#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
    EIF_MDM|EIF_SERVER|EIF_SERVER_TCP|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE|EIF_IPV6,
#else
    EIF_MDM|EIF_SERVER|EIF_SERVER_TCP|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE,
#endif
    3,                /* backlog for TCP server port */
    HTTPD_PORT_SSL_SERVICE,   /* TCP server port */
    TRUE,             /* isFeatureCompiledIn */

    0, 0, 0},
#endif


/*__MTS__, Richard Huang*/
#ifdef SUPPORT_HTTPD_SSL 
   {EID_HTTPD_SSL,
    NDA_ACCESS_HTTPD,
    "httpd_user",
    "/bin/httpd",
    " -s -U",
#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
    EIF_MDM|EIF_SERVER|EIF_SERVER_TCP|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE|EIF_IPV6,
#else
    EIF_MDM|EIF_SERVER|EIF_SERVER_TCP|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE,
#endif
    3,                /* backlog for TCP server port */
    HTTPD_PORT_SSL,   /* TCP server port */
#if 1 /*Jessica@MSTC: only open the port for user remote GUI after changing port. 20140122*/
    FALSE,             /* isFeatureCompiledIn */
#else
    TRUE,
#endif

    0, 0, 0},
#endif /*SUPPORT_HTTPD_SSL*/

/* Merge from 11g, __TELEFONICA__, Mitrastar Jennifer, 20111208*/
#ifdef MSTC_TELEFONICA_CUSTOMIZATION 
		 {EID_CPULOAD,
		  0,
		  "",
		  "/bin/cpuload",
		  "",				  /* run args */
		  EIF_MESSAGING_CAPABLE|EIF_LAUNCH_ON_BOOT, /* EIF_ flags */
		  0,				/* backlog for TCP server port */
		  0,				/* TCP/UDP server port */
		  TRUE, 			/* isFeatureCompiledIn */
		  0, 0, 0}, 		/* misc memory parameters */
#endif  /*MSTC_TELEFONICA_CUSTOMIZATION*/


   {EID_SNMPD,
    NDA_ACCESS_SNMPD,
    "snmpd",
    "/bin/snmpd",
    "",
    EIF_MDM|EIF_MESSAGING_CAPABLE,
    /*    EIF_MDM, */
    0,
    SNMPD_PORT,
#ifdef SUPPORT_SNMP
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_CONSOLED,
    NDA_ACCESS_CONSOLED,
    "consoled",
    "/bin/consoled",
    "",
    EIF_MDM|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE,
    0,
    0,
#ifdef SUPPORT_CONSOLED
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_TELNETD,
    NDA_ACCESS_TELNETD,
    "telnetd",
    "/bin/telnetd",
    "",
#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
    EIF_MDM|EIF_SERVER|EIF_SERVER_TCP|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE|EIF_IPV6,
#else
    EIF_MDM|EIF_SERVER|EIF_SERVER_TCP|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE,
#endif
    3,             /* backlog for TCP server port */
    TELNETD_PORT,
#ifdef SUPPORT_TELNETD
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_SSHD,
    NDA_ACCESS_SSHD,
    "sshd",
    "/bin/sshd",
    "",
#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
    EIF_MDM|EIF_SERVER|EIF_SERVER_TCP|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE|EIF_IPV6,
#else
    EIF_MDM|EIF_SERVER|EIF_SERVER_TCP|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE,
#endif
    3,             /* backlog for TCP server port */
    SSHD_PORT,
#ifdef SUPPORT_SSHD
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_UPNP,
    NDA_ACCESS_UPNP,
    "upnp",
    "/bin/upnp",
    "",
    EIF_MDM|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE,
    0,
    0,
#ifdef SUPPORT_UPNP
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},


   {EID_OMCID,
    0,
    "omcid",
    "/bin/omcid",
    "-v 0 start",
    EIF_LAUNCH_ON_BOOT|EIF_MDM|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE,
    0,
    0,
#ifdef DMP_X_ITU_ORG_GPON_1
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},


   /*
    */
   {EID_OMCIPMD,
    0,
    "omcipmd",
    "/bin/omcipmd",
    "",
    EIF_LAUNCH_ON_BOOT|EIF_MDM|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE,
    0,
    0,
#ifdef DMP_X_ITU_ORG_GPON_1
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

#ifndef DMP_X_ITU_ORG_GPON_1
    {EID_CVTPSI,
     0,
     "cvtpsi",
     "/bin/cvtpsi",
     NULL,
     0,
     0,
     0,
     FALSE,            /* isFeatureCompiledIn */
     0, 0, 0},
#endif

   {EID_VECTORINGD,
    NDA_ACCESS_VECTORINGD,
    "vectoringd",
    "/bin/vectoringd",
    NULL,
    EIF_MDM|EIF_MESSAGING_CAPABLE|EIF_LAUNCH_ON_BOOT,
    0,
    0,
#ifdef SUPPORT_VECTORINGD
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_SMD,
    0,
    "smd",
    "/bin/smd",
    "",
    EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE,   /* flags */
    0,
    0,
    TRUE,               /* isFeatureCompiledIn */
    0, 0, 0},

   {EID_SSK,
    0,
    "ssk",
    "/bin/ssk",
    "",
    EIF_LAUNCH_IN_STAGE_1|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE,
    0,
    0,
    TRUE,             /* isFeatureCompiledIn */
    0, 0, 0},

   {EID_PPP,
    0,
    "pppd",
    "/bin/pppd",
    "-c",
    EIF_MESSAGING_CAPABLE|EIF_MULTIPLE_INSTANCES,
    0,
    0,
    TRUE,             /* isFeatureCompiledIn */
    0, 0, 0},

   {EID_DHCPC,
    0,
    "dhcpc",
    "/bin/dhcpc",
    "-f",  /* stay in foreground, we don't want it to fork or daemonize */
    EIF_MESSAGING_CAPABLE|EIF_MULTIPLE_INSTANCES,
    0,
    0,
    TRUE,             /* isFeatureCompiledIn */
    0, 0, 0},

   {EID_DHCPD,
    0,
    "dhcpd",
    "/bin/dhcpd",
    "",
    EIF_MESSAGING_CAPABLE,
    0,
    0,
#ifdef SUPPORT_UDHCP
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},
#if 1 /* ChenHe@MSTC for FTP open access, 20130301*/
{EID_VSFTPD,
        0,
        "vsftpd",
        "/bin/vsftpd",
        "",
        EIF_SERVER|EIF_SERVER_TCP|EIF_MESSAGING_CAPABLE,
        3, /* backlog for TCP server port */
        FTPD_PORT,
        TRUE,
        0, 0, 0},
#else
   {EID_FTPD,
    0,
    "bftpd",
    "/bin/bftpd",
    "",
#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
    EIF_SERVER|EIF_SERVER_TCP|EIF_MESSAGING_CAPABLE|EIF_IPV6,
#else
    EIF_SERVER|EIF_SERVER_TCP|EIF_MESSAGING_CAPABLE,
#endif
    1, /* backlog for TCP server port */
    FTPD_PORT,
#ifdef SUPPORT_FTPD
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},
#endif

#if 0 /*For To2: disable tftp, MSTC ChaoWen, 20130506*/
   {EID_TFTPD,
    0,
    "tftpd",
    "/usr/bin/tftpd",
    "",
#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
    EIF_SERVER|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE|EIF_IPV6,
#else
    EIF_SERVER|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE,
#endif
    0,
    TFTPD_PORT,
    TRUE,             /* isFeatureCompiledIn */
    0, 0, 0},

    {EID_TFTP,  /* this is the client */
    0,
    "tftp",
    "/usr/bin/tftp",
    "",
    EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE,
    0,
    0,
    TRUE,             /* isFeatureCompiledIn */
    0, 0, 0},
#endif

   {EID_DNSPROBE,
    0,                  /* TR69 attribute access bit */
    "dnsprobe",
    "/bin/dnsprobe",
    "",                 /* run args */
    0,                  /* EIF_ flags */
    0,                  /* backlog for TCP server port */
    0,                  /* TCP/UDP server port */
    TRUE,               /* isFeatureCompiledIn */
    0, 0, 0},           /* misc memory parameters */

   {EID_DNSPROXY,
    NDA_ACCESS_TO2,
    "dnsproxy",
    "/bin/dnsproxy",
    "",                 /* run args */
    EIF_MESSAGING_CAPABLE|EIF_AUTO_RELAUNCH, 
    0,
    0,
#ifdef DMP_X_BROADCOM_COM_DNSPROXY_1 /* aka SUPPORT_DNSPROXY */
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_SYSLOGD,
    0,                  /* TR69 attribute access bit */
    "syslogd",
    "/sbin/syslogd",
    "-n",               /* run args (don't daemonize) */
    0,                  /* flags */
    0,                  /* backlog for TCP server port */
    0,                  /* TCP/UDP server port */
    TRUE,               /* isFeatureCompiledIn */
    0, 0, 0},           /* misc memory parameters */

   {EID_KLOGD,
    0,                  /* TR69 attribute access bit */
    "klogd",
    "/sbin/klogd",
    "-n",               /* run args (don't daemonize) */
    0,                  /* flags */
    0,                  /* backlog for TCP server port */
    0,                  /* TCP/UDP server port */
    TRUE,               /* isFeatureCompiledIn */
    0, 0, 0},           /* misc memory parameters */

/* Yi-Jun@MSTC: Flag for vodsl application used, 20121219 */
#ifdef VODSL_APP_BUILD
   {EID_VODSL,
    NDA_ACCESS_VODSL,   /* tr69 attribute access bit, not applicable to vodsl */
    "vodsl",
    "/bin/vodsl",
    "",           /* run args */
    EIF_MDM|EIF_MESSAGING_CAPABLE|EIF_AUTO_RELAUNCH,
    0,            /* backlog for TCP server port, if this is a tcp server */
    0,            /* TCP/UDP server port */
#ifdef SUPPORT_VOXXXLOAD
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},     /* misc memory parameters */
#endif /*VODSL_APP_BUILD*/

/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
/* Yi-Jun@MSTC: Update the condition for ZyIMS, 20121214 */
#ifdef MSTC_VOIP_ZYIMS
   {EID_ZYIMS,
    NDA_ACCESS_ZYIMS,   /* tr69 attribute access bit, not applicable to zyims */
    "zyims",
    "/bin/voiceApp",
    "",           /* run args */
    EIF_LAUNCH_ON_BOOT|EIF_MDM|EIF_MESSAGING_CAPABLE,
    0,            /* backlog for TCP server port, if this is a tcp server */
    0,            /* TCP/UDP server port */
#ifdef VOXXXLOAD
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},     /* misc memory parameters */
#endif /*MSTC_VOIP_ZYIMS*/
#endif /*MSTC_COMMON_PATCH*/

   {EID_CMSQUERY,
    0,
    "cMsquery",
    "/bin/cMsquery",
    "",           /* run args */
    EIF_MESSAGING_CAPABLE,
    0,            /* backlog for TCP server port, if this is a tcp server */
    0,            /* TCP/UDP server port */
    FALSE,            /* isFeatureCompiledIn */
    0, 0, 0},     /* misc memory parameters */

/* Yi-Jun@MSTC: The Stollmann CallMngr+, 20121221 */
#ifdef STOLLMANN_CALLMNGR
   {EID_CALLMNGR,
    0,
    "callmngr",
    "/bin/callmngr",
    "",           /* run args */
    EIF_LAUNCH_ON_BOOT, 
    0,            /* backlog for TCP server port, if this is a tcp server */
    0,            /* TCP/UDP server port */
#ifdef VOXXXLOAD
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},     /* misc memory parameters */
   {EID_CALLMNGR_BSPAL,
    NDA_ACCESS_IMS,
    "callmngr_bspal",
    "/bin/callmngr_bspal",
    "",           /* run args */
    EIF_LAUNCH_ON_BOOT | EIF_MESSAGING_CAPABLE | EIF_MDM,
    0,            /* backlog for TCP server port, if this is a tcp server */
    0,            /* TCP/UDP server port */
#ifdef VOXXXLOAD
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},     /* misc memory parameters */

/* WenJuin@MSTC: The Stollmann CallMngr+ CLI cmd, 20130202 */       
   {EID_CALLMNGR_CLI,
    NDA_ACCESS_IMS,
    "mdm_cli",
    "/bin/mdm_cli",
    "",           /* run args */
    EIF_MESSAGING_CAPABLE | EIF_MDM,
    0,            /* backlog for TCP server port, if this is a tcp server */
    0,            /* TCP/UDP server port */
#ifdef VOXXXLOAD
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},     /* misc memory parameters */
#endif /*STOLLMANN_CALLMNGR*/

   {EID_DECTDBGD,
    NDA_ACCESS_DECTDBGD,   /* tr69 attribute access bit, not applicable to vodsl */
    "dectdbgd_NEW",
    "/etc/dectdbgd",
    "",           /* run args */
    EIF_MDM|EIF_MESSAGING_CAPABLE,
    0,            /* backlog for TCP server port, if this is a tcp server */
    0,            /* TCP/UDP server port */
    TRUE,             /* isFeatureCompiledIn */
    0, 0, 0},     /* misc memory parameters */

/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
/*__MTS__, Kenny, SUPPORT_DNSMASQ*/
	  {EID_DNSMASQ,
	   0, 
	   "dnsmasq",
	   "/bin/dnsmasq",
	   "",				   /* run args */
	   EIF_MESSAGING_CAPABLE,				   /* EIF_ flags */
	   0, 
	   0, 
/* SUPPORT_DNSMASQ */
#ifdef SUPPORT_DNSMASQ 
	   TRUE,			 /* isFeatureCompiledIn */
#else
	   FALSE,			 /* isFeatureCompiledIn */
#endif
	   0, 0, 0}, 
#endif /*MSTC_COMMON_PATCH*/

   {EID_DDNSD,
    0,                  /* TR69 attribute access bit */
/*[IreneWang@MSTC:TO2]$ Support DDNS, 20130227*/
#if 1
    "inadyn",
    "/bin/inadyn",
#else
    "ddnsd",
    "/bin/ddnsd",
#endif  /*1*/
    "",                 /* run args */
    EIF_MESSAGING_CAPABLE,  /* flags (later make it desktop capable) */
    0,                  /* backlog for TCP server port */
    0,                  /* TCP/UDP server port */
    TRUE,               /* isFeatureCompiledIn */
    0, 0, 0},           /* misc memory parameters */

   {EID_ZEBRA,
    0,                  /* TR69 attribute access bit */
    "zebra",
    "/bin/zebra",
    "",                 /* run args */
    EIF_MESSAGING_CAPABLE,  /* flags (later make it desktop capable) */
    0,                  /* backlog for TCP server port */
    0,                  /* TCP/UDP server port */
    TRUE,               /* isFeatureCompiledIn */
    0, 0, 0},           /* misc memory parameters */

   {EID_RIPD,
    0,                  /* TR69 attribute access bit */
    "ripd",
    "/bin/ripd",
    "",                 /* run args */
    EIF_MESSAGING_CAPABLE,  /* flags (later make it desktop capable) */
    0,                  /* backlog for TCP server port */
    0,                  /* TCP/UDP server port */
    TRUE,               /* isFeatureCompiledIn */
    0, 0, 0},           /* misc memory parameters */
    
   {EID_SNTP,
    0,                  /* TR69 attribute access bit */
    "sntp",
    "/bin/sntp",
    "",                 /* run args */
    EIF_MESSAGING_CAPABLE,  /* flags (later make it desktop capable) */
    0,                  /* backlog for TCP server port */
    0,                  /* TCP/UDP server port */
    TRUE,               /* isFeatureCompiledIn */
    0, 0, 0},           /* misc memory parameters */
    
   {EID_URLFILTERD,
    0,
    "urlfilterd",
    "/bin/urlfilterd",
    "",                 /* run args */
    EIF_MESSAGING_CAPABLE,  /* flags (later make it desktop capable) */
    0,
    0,
    TRUE,               /* isFeatureCompiledIn */
    0, 0, 0},

   {EID_DHCP6C,
    0,
    "dhcp6c",
    "/bin/dhcp6c",
    "",                 /* run args */
    EIF_MESSAGING_CAPABLE|EIF_MULTIPLE_INSTANCES,
    0,
    0,
#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_DHCP6S,
    0,
    "dhcp6s",
    "/bin/dhcp6s",
    "",                 /* run args */
    EIF_MESSAGING_CAPABLE,
    0,
    0,
#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_RADVD,
    0,
    "radvd",
    "/bin/radvd",
    "",                 /* run args */
    EIF_MESSAGING_CAPABLE,
    0,
    0,
#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_IPPD,
    0,
    "ippd",
    "/bin/ippd",
    "",                 /* run args */
    EIF_MESSAGING_CAPABLE,
    0,
    0,
#ifdef SUPPORT_IPP
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},


   {EID_RASTATUS6,
    0,
    "rastatus6",
    "/bin/rastatus6",
    "",                 /* run args */
    EIF_MESSAGING_CAPABLE,
    0,
    0,
#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
   /* __MSTC__, Axel, for LPR/RAW Printing Server */
   {EID_TCPSVDRAW,
    0,
    "tcpsvd",
    "/usr/bin/tcpsvd",
    "-E 0 9100 /usr/bin/lpr -P printer0",                 /* run args */
    EIF_MESSAGING_CAPABLE,
    0,
    0,
#ifdef SUPPORT_IPP
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_TCPSVDLPR,
    0,
    "tcpsvd",
    "/usr/bin/tcpsvd",
    "-E 0 515 /usr/sbin/lpd /dev",                 /* run args */
    EIF_MESSAGING_CAPABLE,
    0,
    0,
#ifdef SUPPORT_IPP
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},
#endif /*MSTC_COMMON_PATCH*/

   {EID_DSLDIAGD,
    0,
    "dsldiagd",
    "/bin/dsldiagd",
    "",                 /* run args */
    EIF_LAUNCH_ON_BOOT,
    0,
    0,
#ifdef SUPPORT_DSLDIAGD
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_SOAPSERVER,
    0,
    "soapserver",
    "/bin/soapserver",
    "-f",                /* run args */
    EIF_LAUNCH_ON_BOOT,
    0,
    0,
#ifdef SUPPORT_SOAP
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

    {EID_FTP,   /* this is FTP the client */
    0,
    "ftp",
    "/usr/bin/ftp",
    "",
    EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE,
    0,
    0,
    TRUE,             /* isFeatureCompiledIn */
    0, 0, 0},

   {EID_UNITTEST,
    0,
    "ut",
    "/",
    "",
    0,
    0,
    0,
    TRUE,             /* isFeatureCompiledIn */
    0, 0, 0},

   {EID_PING,
    0,
    "ping",
    "/bin/ping",
    "",                 /* run args */
#ifdef DMP_X_ITU_ORG_GPON_1
    EIF_MESSAGING_CAPABLE | EIF_MULTIPLE_INSTANCES,  /* EIF_ flags */
#else
    EIF_MESSAGING_CAPABLE ,  /* EIF_ flags */
#endif
    0,                  /* backlog for TCP server port */
    0,                  /* TCP/UDP server port */
    TRUE,               /* isFeatureCompiledIn */
    0, 0, 0},           /* misc memory parameters */

   {EID_TRACERT,
    0,
    "traceroute",
    "/usr/bin/traceroute",
    "",                 /* run args */
#ifdef DMP_X_ITU_ORG_GPON_1
    EIF_MESSAGING_CAPABLE | EIF_MULTIPLE_INSTANCES,  /* EIF_ flags */
#else
    EIF_MESSAGING_CAPABLE ,  /* EIF_ flags */
#endif
    0,                  /* backlog for TCP server port */
    0,                  /* TCP/UDP server port */
#ifdef DMP_X_ITU_ORG_GPON_1
    TRUE,               /* isFeatureCompiledIn */
#else
    FALSE,
#endif
    0, 0, 0},           /* misc memory parameters */

   {EID_PWRCTL,
    0,
    "pwrctl",
    "/bin/pwrctl",
    "",                 /* run args */
    EIF_MESSAGING_CAPABLE,  /* EIF_ flags */
    0,                  /* backlog for TCP server port */
    0,                  /* TCP/UDP server port */
    TRUE,               /* isFeatureCompiledIn */
    0, 0, 0},           /* misc memory parameters */

   {EID_HOTPLUG,
    0,
    "hotplug",
    "/bin/hotplug",
    "",
    EIF_MESSAGING_CAPABLE | EIF_MULTIPLE_INSTANCES,
    0,
    0,
#ifdef DMP_STORAGESERVICE_1
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_SAMBA,
    0,
    "smbd",
    "/bin/smbd",
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
    "-D -s /etc/samba/smb.conf -l=/var/tmp/smbvar --lockdir=/var/tmp/smbvar --piddir=/var/tmp/smbvar --private-dir=/var/tmp/smbvar",                 /* run args */
#else /*MSTC_COMMON_PATCH*/
    "-D",
#endif /*MSTC_COMMON_PATCH*/
    0,
    0,
    0,
#ifdef SUPPORT_SAMBA
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
   {EID_NMBD,
    0,
    "NMBD",
    "/bin/nmbd",
    "-D -s /etc/samba/smb.conf -l=/var/tmp/smbvar --lockdir=/var/tmp/smbvar --piddir=/var/tmp/smbvar --private-dir=/var/tmp/smbvar",                 /* run args */
    EIF_MESSAGING_CAPABLE,         /* EIF_ flags */
    0,                  /* backlog for TCP server port */
    0,                  /* TCP/UDP server port */
#ifdef SUPPORT_SAMBA    
    TRUE,               /* isFeatureCompiledIn */
#else
    FALSE,              /* isFeatureCompiledIn */
#endif
    0, 0, 0},           /* misc memory parameters */
#endif /*MSTC_COMMON_PATCH*/

   {EID_MISC,
    0,
    "misc",
    "/",
    "",
    0,
    0,
    0,
    TRUE,             /* isFeatureCompiledIn */
    0, 0, 0},

   {EID_WLMNGR,
    NDA_ACCESS_WLAN,
    "WLMNGR-DAEMON",
    "/bin/wlmngr",
    "",
    EIF_MDM|EIF_MESSAGING_CAPABLE, //|EIF_DESKTOP_LINUX_CAPABLE |EIF_LAUNCH_ON_BOOT,
    0,
    0,
#ifdef BRCM_WLAN
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,             /* isFeatureCompiledIn */
#endif
    0, 0, 0},

 {EID_WLNVRAM,
    NDA_ACCESS_WLAN,
    "WLNVRAM",
    "/bin/nvram",
    "",
    EIF_MDM|EIF_MESSAGING_CAPABLE,
    0,
    0,
#ifdef BRCM_WLAN
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,             /* isFeatureCompiledIn */
#endif
    0, 0, 0},
    
   {EID_WLEVENT,
    NDA_ACCESS_WLAN,
    "WLEVENT",
    "/bin/wlevt",
    "",
    EIF_MESSAGING_CAPABLE,
    0,
    0,
#ifdef BRCM_WLAN
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,             /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_WLWPS,
    NDA_ACCESS_WLAN,
    "WLWPS",
    "/bin/wps_monitor",
    "",
    EIF_MESSAGING_CAPABLE,
    0,
    0,
#ifdef BRCM_WLAN
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,             /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_WLWAPID,
    NDA_ACCESS_WLAN,
    "WLWAPID",
    "/bin/wapid",
    "",
    EIF_MESSAGING_CAPABLE, //|EIF_DESKTOP_LINUX_CAPABLE |EIF_LAUNCH_ON_BOOT,
    0,
    0,
#ifdef BCMWAPI_WAI
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,             /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_CMFD,
    0,
    "cmfd",
    "/bin/cmfd",
    "",
    EIF_LAUNCH_ON_BOOT|EIF_MESSAGING_CAPABLE,
    0,
    0,
#ifdef SUPPORT_CMFD
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_MCPD,
    0,
    "mcpd",
    "/bin/mcpd",
    "",                 /* run args */
    EIF_MESSAGING_CAPABLE,  /* flags (later make it desktop capable) */
    0,
    0,
#if defined(SUPPORT_IGMP) || defined (SUPPORT_MLD)
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_MCPCTL,
    0,
    "mcpctl",
    "/bin/mcpctl",
    "",                 /* run args */
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
    /* Merge common bug fix from DSL-491HNU-B1B_V2_TTNET revision: 3355 to solve
    that "STBs can't play IPTV program after user unplug-plug xDSL line"*/
    EIF_MESSAGING_CAPABLE|EIF_MDM,  /* flags (later make it desktop capable) */
#else /*MSTC_COMMON_PATCH*/
    EIF_MESSAGING_CAPABLE,  /* flags (later make it desktop capable) */
#endif /*MSTC_COMMON_PATCH*/
    0,
    0,
#if defined(SUPPORT_IGMP) || defined (SUPPORT_MLD)
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_L2TPD,
    0,
    "l2tpd",
    "/bin/openl2tpd",
    "",                 /* run args */
    0,
    0,
    0,
#ifdef DMP_X_BROADCOM_COM_L2TPAC_1
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_PPTPD,
    0,
    "pptp",
    "/bin/pptp",
    "",                 /* run args */
    0,
    0,
    0,
#ifdef DMP_X_BROADCOM_COM_PPTPAC_1
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_MOCAD,
    0,
    "mocad",
    "/bin/mocad",
    "",
    EIF_MESSAGING_CAPABLE|EIF_MULTIPLE_INSTANCES,
    0,
    0,
#ifdef BRCM_MOCA_DAEMON
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_RNGD,
    0,
    "rngd",
    "/bin/rngd",
    "",                 /* run args */
    EIF_LAUNCH_ON_BOOT,
    0,
    0,
#ifdef SUPPORT_HWRANDOM
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_DMSD,
    0,
    "bcmmserver",
    "/bin/bcmmserver",
    "",                 /* run args */
    EIF_MESSAGING_CAPABLE,
    0,
    0,
#ifdef DMP_X_BROADCOM_COM_DLNA_1
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_SWMDK,
    //NDA_ACCESS_SWMDK,
    0,
    "swmdk",
    "/bin/swmdk",
    "",                 /* run args */
    EIF_LAUNCH_ON_BOOT,
    0,
    0,
    TRUE,             /* isFeatureCompiledIn */
    0, 0, 0},

   {EID_EPON_APP,
    NDA_ACCESS_EPONAPP,
    "eponapp",
    "/bin/eponapp",
    "",                 /* run args */
    EIF_MDM | EIF_LAUNCH_ON_BOOT | EIF_MESSAGING_CAPABLE,
    0,
    0,
#ifdef DMP_X_BROADCOM_COM_EPON_1
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,             /* isFeatureCompiledIn */
#endif
    0, 0, 0},

{EID_OSGID,        /* eid */
    NDA_ACCESS_TR69C, /* attribute access bit */
    "osgid",          /* name */
    "/bin/osgid",     /* path */
    "",               /* run args */
    EIF_MDM|EIF_SERVER|EIF_SERVER_TCP|EIF_LAUNCH_ON_BOOT|EIF_MESSAGING_CAPABLE|EIF_DESKTOP_LINUX_CAPABLE,
    0,                /* backlog for TCP server port: TO_DO, should be NULL? */
    0,       /* TCP server port: TO_DO; should be NULL? unless felix is sending somethign up*/
#ifdef DMP_X_BROADCOM_COM_OSGI_JVM_1
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},         /* misc mem usage parameters */

/* Yi-Jun@MSTC: Flag for vodsl application used, 20121219 */
#ifdef VODSL_APP_BUILD
   {EID_DECT,
    NDA_ACCESS_VODSL,   /* tr69 attribute access bit, not applicable to vodsl */
    "dectd",
    "/bin/dectd",
    "",           /* run args */
    EIF_MDM|EIF_MESSAGING_CAPABLE,
    0,            /* backlog for TCP server port, if this is a tcp server */
    0,            /* TCP/UDP server port */
#ifdef SUPPORT_VOXXXLOAD
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},     /* misc memory parameters */
#endif /*VODSL_APP_BUILD*/

   {EID_BMUD,
    0,
    "bmud",
    "/bin/bmud",
    "",                 /* run args */
    EIF_LAUNCH_ON_BOOT|EIF_MESSAGING_CAPABLE,
    0,
    0,
#ifdef SUPPORT_BMU
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},

   {EID_BMUCTL,
    0,
    "bmuctl",
    "/bin/bmuctl",
    "",                 /* run args */
    EIF_MESSAGING_CAPABLE,  /* EIF_ flags */
    0,                  /* backlog for TCP server port */
    0,                  /* TCP/UDP server port */
#ifdef SUPPORT_BMU
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,            /* isFeatureCompiledIn */
#endif
    0, 0, 0},           /* misc memory parameters */

    {EID_MODUPDTD,
     0,
     "modupdtd",
     "/bin/modupdtd",
     "",                 /* run args */
     EIF_MESSAGING_CAPABLE,  /* EIF_ flags */
     0,                  /* backlog for TCP server port */
     0,                  /* TCP/UDP server port */
 #ifdef SUPPORT_MOD_SW_UPDATE
     TRUE,             /* isFeatureCompiledIn */
 #else
     FALSE,            /* isFeatureCompiledIn */
 #endif
     0, 0, 0},           /* misc memory parameters */

    {EID_MODUPDTV,
      0,
      "modupdtv",
      "",                 /* this app is never launched by smd */
      "",                 /* run args */
      EIF_MESSAGING_CAPABLE,  /* EIF_ flags */
      0,                  /* backlog for TCP server port */
      0,                  /* TCP/UDP server port */
  #ifdef SUPPORT_MOD_SW_UPDATE
      TRUE,             /* isFeatureCompiledIn */
  #else
      FALSE,            /* isFeatureCompiledIn */
  #endif
      0, 0, 0},           /* misc memory parameters */
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_TELEFONICA_CUSTOMIZATION
   /* For 3G WAN, __TELEFONICA__, MitraStar, Chehuai */      
   {EID_LINKUPDOWN,
    0,
    "link_updown",
    "/bin/link_updown",
    "",		                  /* run args */
    EIF_LAUNCH_ON_BOOT,
    /* EIF_ flags */
    0,				  /* backlog for TCP server port */
    0,				  /* TCP/UDP server port */
#ifdef DMP_X_ZYXEL_COM_WWAN_1
    TRUE, 			  /* isFeatureCompiledIn */
#else
    FALSE,			  /* isFeatureCompiledIn */
#endif
    0, 0, 0},  

   {EID_CELLD,
    NDA_ACCESS_CELLD, 
    "celld",
    "/bin/celld",
    "",				  /* run args */
    EIF_MESSAGING_CAPABLE|EIF_MDM |EIF_LAUNCH_ON_BOOT,//	  0,
    /* EIF_ flags */
    0,				  /* backlog for TCP server port */
    0,				  /* TCP/UDP server port */
#ifdef DMP_X_ZYXEL_COM_WWAN_1
    TRUE, 			  /* isFeatureCompiledIn */
#else
    FALSE,			  /* isFeatureCompiledIn */
#endif
    0, 0, 0},

/* For UDP Echo Plus profile in TR143, __TELEFONICA__, ZyXEL Eva, 20091103.  */
#ifdef MTS_Telefonica_TR69	
   {EID_UDPECHOD,       /*eid*/
    NDA_ACCESS_UDPECHOD,   /* TR69 attribute access bit */
    "udpechod",         /*name*/
    "/bin/udpechod",    /*path*/
    "",                 /* run args (don't daemonize) */
    EIF_MDM|EIF_SERVER|EIF_MESSAGING_CAPABLE, /* flags */
    0,                  /* backlog for TCP server port */
    0,                  /* TCP/UDP server port */
    TRUE,               /* isFeatureCompiledIn */
    0, 0, 0},           /* misc memory parameters */

   {EID_UPLOADD,       /*eid*/
    NDA_ACCESS_UPLOADD,   /* TR69 attribute access bit */
    "uploadd",         /*name*/
    "/bin/uploadd",    /*path*/
    "",                 /* run args (don't daemonize) */
    EIF_MDM|EIF_MESSAGING_CAPABLE, /* flags */
    0,                  /* backlog for TCP server port */
    0,                  /* TCP/UDP server port */
    TRUE,               /* isFeatureCompiledIn */
    0, 0, 0},            /* misc memory parameters */

#endif /*MTS_Telefonica_TR69*/

/*KeyYang@MSTC Add it for Quqntenna RMT_QCSAPI Porting 20111215*/
#if defined(BRCM_WLAN) && defined(MTS_Telefonica_CLI) && defined(RMT_QCSAPI_SUPPORT)
    {EID_RMT_QCSAPI_MNGR,
    NDA_ACCESS_WLAN,
    "WL_RMT_QCSAPIMNGR_DAEMON",
    "/usr/bin/rmt_qcsmngr",
    "",
    EIF_MDM|EIF_MESSAGING_CAPABLE, //|EIF_DESKTOP_LINUX_CAPABLE |EIF_LAUNCH_ON_BOOT,
    0,
    0,
#ifdef BRCM_WLAN
    TRUE,             /* isFeatureCompiledIn */
#else
    FALSE,             /* isFeatureCompiledIn */
#endif
    0, 0, 0},
#endif /*defined(BRCM_WLAN) && defined(MTS_Telefonica_CLI) && defined(RMT_QCSAPI_SUPPORT)*/
#endif /*MSTC_TELEFONICA_CUSTOMIZATION*/
    {EID_SCHEDULE,
    NDA_ACCESS_CVTPSI,
    "schedule",
    "/bin/autoscheduling",
    "",				   /* run args */
    EIF_MESSAGING_CAPABLE| EIF_MDM, /* EIF_ flags */
    0,				 /* backlog for TCP server port */
    0,				 /* TCP/UDP server port */
    TRUE,			 /* isFeatureCompiledIn */
    0, 0, 0},		 /* misc memory parameters */
#if 1    /*MSTC, chris for autodisconnect 20130501*/
    {EID_AUTODISCONNECT,
    NDA_ACCESS_CVTPSI,
    "autodisconnect",
    "/bin/autodisconnect",
    "",				   /* run args */
    EIF_MESSAGING_CAPABLE| EIF_MDM, /* EIF_ flags */
    0,				 /* backlog for TCP server port */
    0,				 /* TCP/UDP server port */
    TRUE,			 /* isFeatureCompiledIn */
    0, 0, 0},		 /* misc memory parameters */    
#endif    
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
   {EID_WGET,
    0,
    "wget",
    "/usr/bin/wget",
    "",                 /* run args */
    EIF_MESSAGING_CAPABLE,  /* EIF_ flags */
    0,                  /* backlog for TCP server port */
    0,                  /* TCP/UDP server port */
    TRUE,               /* isFeatureCompiledIn */
#ifdef MSTC_SUPPORT_RFC6092
    0, 0, 0},         /* misc memory parameters */
#else
    0, 0, 0}         /* misc memory parameters */
#endif /*MSTC_SUPPORT_RFC6092*/
#endif /*MSTC_COMMON_PATCH*/

#if 1
   ,{EID_NETDISCOVERY,
    NDA_ACCESS_NETDISCOVERY,
    "NETWORK_DISCOVERY",
    "/bin/netdiscovery",
    "",
    EIF_MDM|EIF_MESSAGING_CAPABLE|EIF_LAUNCH_ON_BOOT,
    0,
    0,
    TRUE,             /* isFeatureCompiledIn */
    0, 0, 0},
#endif
#if 1 /*For TO2 Kernel Voip monitor, MSTC WenJuin, 20131206*/
   {EID_VOIPMONITOR,
    0,
    "Voip_KernelMonitor",
    "/bin/voipmonitor",
    "",
    EIF_MESSAGING_CAPABLE|EIF_LAUNCH_ON_BOOT|EIF_AUTO_RELAUNCH,
    0,
    0,
    TRUE,             /* isFeatureCompiledIn */
    0, 0, 0},
#endif
#if 1 /*For IP Filter, Mitrastar Jessica, 20130114*/
	{EID_SET_IPTABLES,
	NDA_ACCESS_CONSOLED,
	"sit",
	"/bin/sit",
	"", 				/* run args */
	EIF_MESSAGING_CAPABLE,	/* EIF_ flags */
	0,					/* backlog for TCP server port */
	0,					/* TCP/UDP server port */
	TRUE,			    /* isFeatureCompiledIn */
	0, 0, 0},			/* misc memory parameters */
#endif
#if 1 /* ChenHe@MSTC for usb rescan process, 20130411*/
	{EID_RESCANUSB,
	NDA_ACCESS_TO2,
	"sit",
	"/bin/rescanusb",
	"", 				/* run args */
	EIF_LAUNCH_ON_BOOT,	/* EIF_ flags */
	0,					/* backlog for TCP server port */
	0,					/* TCP/UDP server port */
	TRUE,			    /* isFeatureCompiledIn */
	0, 0, 0},			/* misc memory parameters */
#endif
#if 1 /* For TO2_GERMANY TE test , MSTC Memphis Chung, 20130521*/
	{EID_KERNELMSGMONITOR,
	NDA_ACCESS_TO2,
	"sit",
	"/bin/TEMsgMonitor",
	"", 				/* run args */
	EIF_MESSAGING_CAPABLE,	/* EIF_ flags */
	0,					/* backlog for TCP server port */
	0,					/* TCP/UDP server port */
	TRUE,			    /* isFeatureCompiledIn */
	0, 0, 0},			/* misc memory parameters */
#endif/*End TO2_GERMANY, Memphis*/
#ifdef MSTC_SUPPORT_RFC6092
      {EID_SetIP6rule,
      NDA_ACCESS_HTTPD,
	"setIP6rule",
	"bin/setIP6rule",
	"",
	EIF_MDM|EIF_MESSAGING_CAPABLE,
	0,
	0,
	TRUE,
	0,0,0}
#endif
   {EID_PHONEBOOKM,
    NDA_ACCESS_IMS,
    "pbd",
    "/bin/pbd",
    "",           /* run args */
    EIF_LAUNCH_ON_BOOT|EIF_MESSAGING_CAPABLE|EIF_MDM,
    0,            /* backlog for TCP server port, if this is a tcp server */
    0,            /* TCP/UDP server port */
    TRUE,            /* isFeatureCompiledIn */
    0, 0, 0},     /* misc memory parameters */
};


#define NUM_ENTITY_INFO_ENTRIES (sizeof(entityInfoArray)/sizeof(CmsEntityInfo))


UINT32 cmsEid_getNumberOfEntityInfoEntries(void)
{
   return NUM_ENTITY_INFO_ENTRIES;
}


const CmsEntityInfo *cmsEid_getFirstEntityInfo(void)
{
   return &(entityInfoArray[0]);
}

/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
/*__MTS__, Dennis*/
UBOOL8 cmsEid_setEntityCompiledIn(CmsEntityId eid, UBOOL8 flag)
{
   UINT32 i=0;

   while (i < NUM_ENTITY_INFO_ENTRIES)
   {
      if (entityInfoArray[i].eid == eid)
      {
         entityInfoArray[i].isFeatureCompiledIn = flag;
         return TRUE;
      }

      i++;
   }

   return FALSE;
}

CmsEntityId cmsEid_getEidByPort(int port, CmsEntityId exceptEid)
{
   UINT32 i=0;

   while (i < NUM_ENTITY_INFO_ENTRIES)
   {
      if (entityInfoArray[i].port == port 
	  	&& (exceptEid == EID_INVALID || entityInfoArray[i].eid != exceptEid) )
      {
         return (entityInfoArray[i].eid);
      }

      i++;
   }

   return EID_INVALID;
}
#endif /*MSTC_COMMON_PATCH*/

/* For UDP Echo Plus profile in TR143, __TELEFONICA__, ZyXEL Eva, 20091103.  */
#ifdef MTS_Telefonica_TR69	
UBOOL8 cmsEid_setEntityPort(CmsEntityId eid, int port)
{
   UINT32 i=0;

   while (i < NUM_ENTITY_INFO_ENTRIES)
   {
      if (entityInfoArray[i].eid == eid)
      {
         entityInfoArray[i].port = port;
         return TRUE;
      }
      i++;
   }
   return FALSE;
}
#endif /* MTS_Telefonica_TR69*/

const CmsEntityInfo *cmsEid_getEntityInfo(CmsEntityId eid)
{
   UINT32 i=0;

   while (i < NUM_ENTITY_INFO_ENTRIES)
   {
      if (entityInfoArray[i].eid == eid)
      {
         return (&entityInfoArray[i]);
      }

      i++;
   }

   return NULL;
}


const CmsEntityInfo *cmsEid_getEntityInfoByAccessBit(UINT16 bit)
{
   UINT32 i=0;

   if (bit == 0)
   {
      return NULL;
   }

   while (i < NUM_ENTITY_INFO_ENTRIES)
   {
      if (entityInfoArray[i].accessBit == bit)
      {
         return (&entityInfoArray[i]);
      }

      i++;
   }

   return NULL;
}


const CmsEntityInfo *cmsEid_getEntityInfoByStringName(const char *name)
{
   UINT32 i=0;

   if (name == NULL)
   {
      return NULL;
   }

   while (i < NUM_ENTITY_INFO_ENTRIES)
   {
      if (!strcmp(entityInfoArray[i].name, name))
      {
         return (&entityInfoArray[i]);
      }

      i++;
   }

   return NULL;
}


CmsRet cmsEid_getStringNamesFromBitMask(UINT16 bitMask, char **buf)
{
   UINT32 i, mask;
   UINT32 len=1; /* for empty mask, return a buffer with an empty string. */
   UINT32 idx=0;
   UINT32 numBits = sizeof(bitMask) * 8;
   const CmsEntityInfo *info;
   UBOOL8 firstName=TRUE;
   CmsRet ret = CMSRET_SUCCESS;

   /* first loop calculates how much space needed for string names */
   for (mask=1, i=0; i < numBits; i++)
   {
      info = cmsEid_getEntityInfoByAccessBit(bitMask & mask);
      if (info == NULL)
      {
         if (bitMask & mask)
         {
            cmsLog_error("unrecognized bitmask 0x%x", (bitMask & mask));
            ret = CMSRET_SUCCESS_UNRECOGNIZED_DATA_IGNORED;
         }
      }
      else
      {
         len += strlen(info->name) + 1;
      }

      mask = mask << 1;
   }

   if (((*buf) = cmsMem_alloc(len, ALLOC_ZEROIZE)) == NULL)
   {
      return CMSRET_RESOURCE_EXCEEDED;
   }

   /* this loop copies string names into buffer */
   for (mask=1, i=0; i < numBits; i++)
   {
      info = cmsEid_getEntityInfoByAccessBit(bitMask & mask);
      if (info != NULL)
      {
         if (firstName)
         {
            idx = sprintf((*buf), "%s", info->name);
            firstName = FALSE;
         }
         else
         {
            idx += sprintf(&((*buf)[idx]), ",%s", info->name);
         }
      }

      mask = mask << 1;
   }

   cmsAst_assert(idx < len);

   return ret;
}



CmsRet cmsEid_getBitMaskFromStringNames(const char *buf, UINT16 *bitMask)
{
   const char *start;
   const char *end;
   const CmsEntityInfo *info;
   CmsRet ret = CMSRET_SUCCESS;

   if (bitMask == NULL)
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   *bitMask = 0;

   if ((buf == NULL) || (strlen(buf) == 0))
   {
      /* null or empty string means no bits are set */
      return CMSRET_SUCCESS;
   }

   start = buf;
   end = strchr(start, ',');
   while (end != NULL)
   {
      char name[BUFLEN_256]; /* this should be long enough to hold string names */

      cmsAst_assert((end - start + 1) < (SINT32) sizeof(name));
      snprintf(name, end - start + 1, "%s", start);
      info = cmsEid_getEntityInfoByStringName(name);
      if (info == NULL)
      {
         cmsLog_debug("ignoring name %s", name);
         ret = CMSRET_SUCCESS_UNRECOGNIZED_DATA_IGNORED;
      }
      else
      {
         (*bitMask) |= info->accessBit;
      }

      start = end+1;
      while ((*start == ' ') && (*start != 0))
      {
         start++;
      }

      if (*start != 0)
      {
         end = strchr(start, ',');
      }
      else
      {
         end = NULL;
      }
   }

   /* there is one more name at the end (with no trailing comma) */
   info = cmsEid_getEntityInfoByStringName(start);
   if (info == NULL)
   {
      cmsLog_debug("ignoring name %s", start);
      ret = CMSRET_SUCCESS_UNRECOGNIZED_DATA_IGNORED;
   }
   else
   {
      (*bitMask) |= info->accessBit;
   }



   return ret;
}


SINT32 cmsEid_getPid(void)
{
   return (oal_getPid());
}
