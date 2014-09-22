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

#ifndef __CMS_EID_H__
#define __CMS_EID_H__

/*!\file cms_eid.h
 * \brief Header file containing entity id assignments in
 * the CPE Management System (CMS).
 */


/*!\enum CmsEntityId
 * \brief Entity Id's (eid) are like process id's (pid), except they identify an
 * application in general, not the specific instance of that application.
 *
 * For example, the ppp application has an entity id of 22.  That entity id
 * refers to the ppp application in general, not a specific instance of ppp,
 * which is uniquely identified by its process id (pid).
 * 
 * Entity Id's are used by the various CMS API's to identify its caller.
 * They are also used by the messaging sub-system to identify the source
 * and destination of messages.  In most cases, there is only one instance
 * of a particular application, so specifying the CmsEntityId is enough to
 * identify the application.  However, for applications that may have multiple
 * instances, additional steps may be required.  (For details on how
 * to identify an application for the purpose of sending or receiving messages,
 * see cms_msg.h).
 *
 * Entity Id's and TR69 access bits are bound together by the entityInfoArray
 * in eid.c.  Entities that have TR69 access bits must be listed first.
 *
 * Any changes in this array must also be reflected in entityInfoArray in eid.c
 */
typedef enum {
   EID_INVALID=0,
   EID_TR69C=1,
   EID_TR64C=2,
   EID_HTTPD=3,
   EID_SNMPD=4,
   EID_CONSOLED=5,
   EID_TELNETD=6,
   EID_SSHD=7,
   EID_UPNP=8,
   EID_AVAILABLE1=9,
   EID_VECTORINGD=10,
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
/* Yi-Jun@MSTC: Update the condition for ZyIMS, 20121214 */
#ifdef MSTC_VOIP_ZYIMS
   /* MSTC_VOICE */
   EID_ZYIMS=11,
#endif /* MSTC_VOIP_ZYIMS */
/* Yi-Jun@MSTC: Flag for vodsl application used, 20121219 */
#ifdef VODSL_APP_BUILD
   EID_VODSL=11,
#endif /*VODSL_APP_BUILD*/
#else /*MSTC_COMMON_PATCH*/
   EID_VODSL=11,
#endif /*MSTC_COMMON_PATCH*/
   EID_WLMNGR=12,
   EID_CVTPSI=13,
   EID_DECTDBGD=14,
   EID_EPON_APP=15,
#if 0 /*We don't need it if VODSL is disabled*/
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
   /* MSTC_VOICE */
   EID_VODSL=16,
#endif /*MSTC_COMMON_PATCH*/
#endif /*We don't need it if VODSL is disabled*/
   EID_WLNVRAM=17,
   EID_WLWAPID=18,
   EID_WLEVENT=19,
   EID_SMD=20,
   EID_SSK=21,
   EID_PPP=22,
   EID_DHCPC=23,
   EID_DHCPD=24,
   EID_FTPD=25,
   EID_TFTPD=26,
   EID_TFTP=27,
   EID_DNSPROBE=28,
   EID_SYSLOGD=30,
   EID_KLOGD=31,
   EID_DDNSD=33,
   EID_ZEBRA=34,
   EID_RIPD=35,
   EID_SNTP=36,
   EID_URLFILTERD=37,
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
   EID_IGMP=38,
#endif /*MSTC_COMMON_PATCH*/
   EID_TRACERT=39,
   EID_PING=40,
   EID_DHCP6C=41,
   EID_DHCP6S=42,
   EID_RADVD=43,
   EID_DNSPROXY=44,
   EID_IPPD=45,
   EID_FTP=46,
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
   EID_MLD=47,
#endif /*MSTC_COMMON_PATCH*/
   EID_DSLDIAGD=48,
   EID_SOAPSERVER=49,
   EID_PWRCTL=50,
   EID_HOTPLUG=51,
   EID_L2TPD=52,
   EID_SAMBA=53,
   EID_PPTPD=54,
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
   EID_LASERD=55,
#endif /*MSTC_COMMON_PATCH*/
   EID_DECT=56,
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
   EID_SAMBD=57,
   /*__MTS__, Kenny, DNSMASQ*/
   EID_DNSMASQ=58,
#endif /*MSTC_COMMON_PATCH*/
   EID_OMCID=60,
   EID_OMCIPMD=61,   
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
   EID_RASTATUS6=62,
   /*__MTS__, Richard Huang, for https*/
   EID_HTTPD_SSL=63,
#else /*MSTC_COMMON_PATCH*/
   EID_RASTATUS6=62,
#endif /*MSTC_COMMON_PATCH*/
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_TELEFONICA_CUSTOMIZATION
   /* Merge from 11g, __TELEFONICA__, Mitrastar Jennifer, 20111208*/
   EID_CPULOAD=64,
#endif /*MSTC_TELEFONICA_CUSTOMIZATION*/
   EID_UNITTEST=90,
   EID_MISC=91,
/* For UDP Echo Plus profile in TR143, __TELEFONICA__, ZyXEL Eva, 20091103.  */
#ifdef MTS_Telefonica_TR69	
   EID_UDPECHOD=92,
#endif
   EID_WLWPS=93,
   EID_CMFD=94,
   EID_MCPD=95,
   EID_MOCAD=96,
   EID_RNGD=97,
   EID_DMSD=98,
/* For upload diagnostic profile in TR143, __TELEFONICA__, ZyXEL Eva, 20091120.  */
#ifdef MTS_Telefonica_TR69	
   EID_UPLOADD=99,
#endif
   EID_SWMDK=100,
   EID_OLT_TEST=101,
   EID_OSGID=102,
   EID_BMUD=103,
   EID_BMUCTL=104,
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_TELEFONICA_CUSTOMIZATION
/* For Telefonica 3G WAN backup, __TELEFONICA__, MitraStar Chehuai, 20110617 */
   EID_LINKUPDOWN=105,
   EID_CELLD=106,
   EID_WGET=107,
   EID_MCPCTL=108,
#else /*MSTC_TELEFONICA_CUSTOMIZATION*/
   EID_MCPCTL=107,
#endif /*MSTC_TELEFONICA_CUSTOMIZATION*/
#ifdef MSTC_SUPPORT_RFC6092
  EID_SetIP6rule = 109,
#endif
#ifdef MSTC_TO2_GERMANY_CUSTOMIZATION 
	/*For autoscheduling, Mitrastar ChingLung, 20121220*/
   EID_SCHEDULE=110,
	/*For IP Filter, Mitrastar Jessica, 20130114*/
   EID_SET_IPTABLES=111,
	/*_MSTC_, chris 2012-11-23 for netdiscovery process*/
   EID_NETDISCOVERY = 112,
	/* ChenHe@MSTC for FTP open access, 20130301*/
   EID_VSFTPD = 113,
	/*For To2 Service Remote GUI, MSTC ChaoWen, 20130215*/
   EID_HTTPD_SSL_SERVICE = 114,
	/* ChenHe@MSTC for usb rescan process, 20130411*/
   EID_RESCANUSB = 117,
   	/* _MSTC_, chris 2013-05-01 for autodisconnect process*/
   EID_AUTODISCONNECT = 118,
	/*For TO2 Kernel MSG monitor, MSTC Memphis Chung, 20130521*/
   EID_KERNELMSGMONITOR = 119,
	/*For TO2 Kernel Voip monitor, MSTC WenJuin, 20131206*/
   EID_VOIPMONITOR = 120,   
#endif
   EID_CMSQUERY=115,
   EID_PHONEBOOKM=116,

/* Yi-Jun@MSTC: The Stollmann CallMngr+, 20121221 */
#ifdef STOLLMANN_CALLMNGR
   EID_CALLMNGR=200,
   EID_CALLMNGR_BSPAL=201,
/* WenJuin@MSTC: The Stollmann CallMngr+ CLI cmd, 20130202 */   
   EID_CALLMNGR_CLI=202,
#endif /*STOLLMANN_CALLMNGR*/
   EID_MODUPDTD=500,
   EID_MODUPDTV=501,
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
   EID_NMBD=502, 
#endif /*MSTC_COMMON_PATCH*/
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#if defined(BRCM_WLAN) && defined(MTS_Telefonica_CLI) && defined(RMT_QCSAPI_SUPPORT)
   EID_RMT_QCSAPI_MNGR=503,
#endif
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
   /* __MSTC__, Axel, for LPR/RAW Printing Server */
   EID_TCPSVDRAW=504,
   EID_TCPSVDLPR=505,
#endif /*MSTC_COMMON_PATCH*/
EID_LAST=65535
} CmsEntityId;


/** Param accessible by TR69C;  Used in MdmNodeAttributes.accessBitMask. */
#define NDA_ACCESS_TR69C              0x0001

/** Param accessible by TR64C;  Used in MdmNodeAttributes.accessBitMask. */
#define NDA_ACCESS_TR64C              0x0002

/** Param accessible by httpd;  Used in MdmNodeAttributes.accessBitMask. */
#define NDA_ACCESS_HTTPD              0x0004

/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_TELEFONICA_CUSTOMIZATION
/* For Telefonica 3G WAN backup, __TELEFONICA__, MitraStar Chehuai, 20110617 */
/** Param accessible by CELLD;  Used in MdmNodeAttributes.accessBitMask. */
#define NDA_ACCESS_CELLD              0x0004
#endif /*MSTC_TELEFONICA_CUSTOMIZATION*/

/** Param accessible by SNMP;  Used in MdmNodeAttributes.accessBitMask. */
#define NDA_ACCESS_SNMPD              0x0008

/** Param accessible by consoled/cli;  Used in MdmNodeAttributes.accessBitMask. */
#define NDA_ACCESS_CONSOLED           0x0010

/** Param accessible by telnetd/cli;  Used in MdmNodeAttributes.accessBitMask. */
#define NDA_ACCESS_TELNETD            0x0020

/** Param accessible by sshd/cli;  Used in MdmNodeAttributes.accessBitMask. */
#define NDA_ACCESS_SSHD               0x0040

/** Param accessible by UPnp;  Used in MdmNodeAttributes.accessBitMask. */
#define NDA_ACCESS_UPNP               0x0080

/** Param accessible by OMCID;  Used in MdmNodeAttributes.accessBitMask. */
#define NDA_ACCESS_AVAILABLE1         0x0100

/** Param accessible by vectoringd;  Used in MdmNodeAttributes.accessBitMask. */
#define NDA_ACCESS_VECTORINGD         0x0200

/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
#define NDA_ACCESS_IMS                   0x0400
/* Yi-Jun@MSTC: Update the condition for ZyIMS, 20121214 */
#ifdef MSTC_VOIP_ZYIMS
/* MSTC_VOICE */
/** Param accessible by ZYIMS;  Used in MdmNodeAttributes.accessBitMask. */
#define NDA_ACCESS_ZYIMS              0x0400
#undef NDA_ACCESS_IMS
#define NDA_ACCESS_IMS                   NDA_ACCESS_ZYIMS
#endif /*MSTC_VOIP_ZYIMS*/
#else /*MSTC_COMMON_PATCH*/
/** Param accessible by VODSL;  Used in MdmNodeAttributes.accessBitMask. */
#define NDA_ACCESS_VODSL              0x0400
#endif /*MSTC_COMMON_PATCH*/

/** Param accessible by WLANmgr;  Used in MdmNodeAttributes.accessBitMask. */
#define NDA_ACCESS_WLAN               0x0800

/** On legacy systems, param accessible by convert PSI to MDM;  Used in MdmNodeAttributes.accessBitMask.
 * On new 6816 systems, this bit is used by OMCIPMD.
 */
#define NDA_ACCESS_CVTPSI             0x1000

/** Param accessible by DECTDBGD;  Used in MdmNodeAttributes.accessBitMask. */
#define NDA_ACCESS_DECTDBGD           0x2000

/* For UDP Echo Plus profile in TR143, __TELEFONICA__, ZyXEL Eva, 20091103.  */
#ifdef MTS_Telefonica_TR69	
#define NDA_ACCESS_UDPECHOD           0x2002 
#endif

/* For upload diagnostic profile in TR143, __TELEFONICA__, ZyXEL Eva, 20091120.  */
#ifdef MTS_Telefonica_TR69	
#define NDA_ACCESS_UPLOADD            0x2004 
#endif

/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
/* Yi-Jun@MSTC: Flag for vodsl application used, 20121219 */
#ifdef VODSL_APP_BUILD
/* MSTC_VOICE */
/** Param accessible by VODSL;  Used in MdmNodeAttributes.accessBitMask. */
#define NDA_ACCESS_VODSL              0x2008
#undef NDA_ACCESS_IMS
#define NDA_ACCESS_IMS                   NDA_ACCESS_VODSL
#endif /*VODSL_APP_BUILD*/
#endif /*MSTC_COMMON_PATCH*/

/** Param accessible by SWMDK;  Used in MdmNodeAttributes.accessBitMask. */
#define NDA_ACCESS_EPONAPP            0x4000

#ifdef MSTC_TO2_GERMANY_CUSTOMIZATION
#define NDA_ACCESS_TO2				  NDA_ACCESS_CVTPSI	/* set a value which had been used 0x1000*/
/** Param accessible by PPP;  Used in MdmNodeAttributes.accessBitMask. */
#define NDA_ACCESS_PPP            NDA_ACCESS_TO2

/** Param accessible by PPP;  Used in MdmNodeAttributes.accessBitMask. */
#define NDA_ACCESS_NETDISCOVERY            NDA_ACCESS_TO2
#endif
/* the access bits must not exceed 0x4000.  There are only 15 bits available! */



/** List of all management entities other than tr69c.
 *
 * tr69c considers these management entities "subscriber" management entities.
 * This is used by TR69C to grant access to all other management entities.
 * This bit mask is used in MdmNodeAttributes.accessBitMask.
 * See also DEFAULT_ACCESS_BIT_MASK in cms_mdm.h
 *
 */
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
/* MSTC_VOICE */
#define NDA_ACCESS_SUBSCRIBER  (NDA_ACCESS_TR64C        |                         \
                                NDA_ACCESS_HTTPD        | NDA_ACCESS_SNMPD      | \
                                NDA_ACCESS_CONSOLED     | NDA_ACCESS_TELNETD    | \
                                NDA_ACCESS_SSHD         | NDA_ACCESS_UPNP       | \
                                NDA_ACCESS_VECTORINGD   | NDA_ACCESS_CVTPSI     | \
                                NDA_ACCESS_IMS        | NDA_ACCESS_WLAN       | \
                                NDA_ACCESS_DECTDBGD     | NDA_ACCESS_EPONAPP)
#else /*MSTC_COMMON_PATCH*/
#define NDA_ACCESS_SUBSCRIBER  (NDA_ACCESS_TR64C        |                         \
                                NDA_ACCESS_HTTPD        | NDA_ACCESS_SNMPD      | \
                                NDA_ACCESS_CONSOLED     | NDA_ACCESS_TELNETD    | \
                                NDA_ACCESS_SSHD         | NDA_ACCESS_UPNP       | \
                                NDA_ACCESS_VECTORINGD   | NDA_ACCESS_CVTPSI     | \
                                NDA_ACCESS_VODSL        | NDA_ACCESS_WLAN       | \
                                NDA_ACCESS_DECTDBGD     | NDA_ACCESS_EPONAPP)
#endif /*MSTC_COMMON_PATCH*/


/** Entity is a server, which opens a socket and listens for requests.
 *
 * This flag is used in CmsEntityInfo.flags below.
 */
#define EIF_SERVER                    0x01


/** If entity is a server, server socket uses TCP (otherwise, uses UDP)
 *
 * This flag is used in CmsEntityInfo.flags below.
 */
#define EIF_SERVER_TCP                0x02


/** Entity should be launched during stage 1 of CMS startup..
 *
 * This flag is used in CmsEntityInfo.flags below.
 * So far, the only app we launch during stage 1 is ssk.
 */
#define EIF_LAUNCH_IN_STAGE_1         0x04


/** Entity should be launched on system boot (during stage 2).
 *
 * This flag is used in CmsEntityInfo.flags below.
 */
#define EIF_LAUNCH_ON_BOOT            0x08


/** Entity will need access to the MDM.
 *
 * This flag is used in CmsEntityInfo.flags below.
 * This flag will cause smd to pass the MDM shared memory id to the
 * app in the -m option.  
 * If the entity is going to access the MDM, it must have a NDA_ACCESS_xxx
 * bit defined for it.
 */
#define EIF_MDM                       0x10


/** Entity can run on the Desktop Linux.
 *
 * Some entities, such as httpd, can run on the Desktop Linux as
 * well as modems.  Others, such as dhcpd, can only run on the modem.
 */
#define EIF_DESKTOP_LINUX_CAPABLE     0x20


/** Entity will create a cms_msg communications link to smd. */
#define EIF_MESSAGING_CAPABLE         0x40


/** Entity can have multiple instances, currently only ppp and dhcpc */
#define EIF_MULTIPLE_INSTANCES        0x80


/** Entity supports IPv6. */
#define EIF_IPV6                      0x100

/** Tell smd to relaunch this App if it dies abnormally (sigsegv, etc) */
#define EIF_AUTO_RELAUNCH             0x200



/** A structure that contains info about an CmsEntityId.
 *
 * For each new CmsEntityId defined, add a corresponding entry to the
 * EntityInfoArray in cms_util/eid.c
 *
 */
typedef struct
{
   CmsEntityId eid;    /**< The entity id for this entry. */
   UINT16 accessBit;   /**< This entity's bit position in MdmNodeAttributes.accessBitMask (only 15 bits available) */
   char *name;         /**< Short name for this entity. */
   char *path;         /**< Full path to the executable. */
   char *runArgs;      /**< Args to pass to this executable on startup */
   UINT16 flags;       /**< Additional info about this entity, see EIF_xxx */
   UINT8  backLog;     /**< Backlog arg for the server socket */
   UINT16 port;        /**< If server, port number for the server socket. */
   UBOOL8 isFeatureCompiledIn; /**< Is this feature selected via make menuconfig */
   UINT32 maxMemUsage;    /**< Maximum memory usage, in KB, 0 if unknown. */
   UINT32 normalMemUsage; /**< Typical memory usage, in KB, 0 if unknown. */
   UINT32 minMemUsage;    /**< Minimum memory usage, in KB, 0 if unknown. */
} CmsEntityInfo;


/** Get the CmsEntityInfo for the specified CmsEntityId.
 *
 * @param eid (IN) CmsEntityId to get info on.
 * @return pointer to EntityInfo structure corresponding to the specified eid.
 *         Caller must not modify the returned CmsEntityInfo structure.
 *         Caller must not free the returned CmsEntityInfo structure.
 *         Returns NULL if CmsEntityId is invalid.
 */
const CmsEntityInfo *cmsEid_getEntityInfo(CmsEntityId eid);


/** Get the CmsEntityInfo for the specified bit in the bitmask.
 *
 * @param bit (IN) A bit in the MdmNodeAttributes.accessBitMask.  Only a 
 *                 single bit is allowed.
 * @return pointer to EntityInfo structure corresponding to the specified bit.
 *         Caller must not modify the returned CmsEntityInfo structure.
 *         Returns NULL if bit is not recognized.
 */
const CmsEntityInfo *cmsEid_getEntityInfoByAccessBit(UINT16 bit);


/** Get the CmsEntityInfo for the specified entity's short string name.
 *
 * @param stringName (IN) The short string name of the entity.
 * @return pointer to EntityInfo structure corresponding to the specified name.
 *         Caller must not modify the returned CmsEntityInfo structure.
 *         Caller must not free the returned CmsEntityInfo structure.
 *         Returns NULL if name is not recognized.
 */
const CmsEntityInfo *cmsEid_getEntityInfoByStringName(const char *stringName);


/** Convert the given bitmask into a buffer of comma separated string names.
 *
 * If the bitmask contains bits which are not recognized, this function
 * will ignore them and fill the buffer with the string names for bits that it
 * does recognize and return CMSRET_SUCCESS_UNRECOGNIZED_DATA_IGNORED.
 * If the bitmask is empty or does not contain any bits which this function
 * recognized, a buffer containing empty string will be returned.
 *
 * @param bitMask  (IN) bitmask.
 * @param buf     (OUT) A buffer containing the comma separated string names of the
 *                      entities in the bit mask.  The caller is responsible for
 *                      freeing the string buffer.
 * @return CmsRet enum.
 */
CmsRet cmsEid_getStringNamesFromBitMask(UINT16 bitMask, char **buf);


/** Convert the given comma separated strings names into a bit mask.
 *
 * If the buffer contains string names which are not recognized, this function
 * will ignore them and fill the bitMask with bits for the string names that it
 * does recognize and return CMSRET_SUCCESS_UNRECOGNIZED_DATA_IGNORED.
 *
 * @param buf      (IN) A buffer containing the comma separated string names of the
 *                      entities in the bit mask.  The caller is responsible for
 *                      freeing the string buffer.
 * @param bitMask (OUT) Resulting bitmask representation.
 *
 * @return CmsRet enum.
 */
CmsRet cmsEid_getBitMaskFromStringNames(const char *buf, UINT16 *bitMask);


/** Get the process/thread id of the caller.
 *
 * Unlike the entity id, a pid is unique for all processes.  So two
 * instances of the same entity id will have different pid's.  This is
 * useful when you need to distinguish among multiple instances of
 * the same eid.
 *
 * @return SINT32 representing the pid.
 */
SINT32 cmsEid_getPid(void);


/** Get the number of enties in the entityInfoArray.
 *
 * This function is for use by smd/oal_event.c.  Other callers should not use it.
 * 
 * @return number of entries in the entityInfoArray.
 */
UINT32 cmsEid_getNumberOfEntityInfoEntries(void);


/** Get the first entry in the entityInfoArray.
 *
 * This function is for use by smd/oal_event.c.  Other callers should not use it.
 * 
 * @return pointer to the first entry in the entityInfoArray.
 */
const CmsEntityInfo *cmsEid_getFirstEntityInfo(void);

/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
/*__MTS__, Dennis*/
UBOOL8 cmsEid_setEntityCompiledIn(CmsEntityId eid, UBOOL8 flag);
UBOOL8 cmsEid_setEntityPort(CmsEntityId eid, int port);
CmsEntityId cmsEid_getEidByPort(int port, CmsEntityId exceptEid);
#endif /*MSTC_COMMON_PATCH*/

/** Mask to get the generic eid out of a UINT32 eid field. */
#define EID_EID_MASK  0x0000ffff

/** Mask to get the pid out of a UINT32 eid field */
#define EID_PID_MASK  0xffff0000

/** Get the generic eid out of a UINT32 eid field */
#define GENERIC_EID(e) (((UINT32) e) & EID_EID_MASK)

/** Get the pid out of a UINT32 eid field */
#define PID_IN_EID(e) ((((UINT32) e) & EID_PID_MASK) >> 16)

/** Make a specific UINT32 eid field out of a pid and a generic eid */
#define MAKE_SPECIFIC_EID(p, e) ((p << 16) | (((UINT32) e) & EID_EID_MASK))

#endif  /* __EID_H__ */
