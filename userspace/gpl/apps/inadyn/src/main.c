/*
Copyright (C) 2003-2004 Narcis Ilisei

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/* 
	Small cmd line program useful for maintaining an IP address in a Dynamic DNS system.
	
	Author: Narcis Ilisei
	Date: April 2003
	
	History:
		May 1 2003 - project begin.
			- intendend to be used only with dyndns.org service
		May 8 2003 - ver. 0.9
			- first version - hard coded parameters
		May 18 2003 : ver. 0.95
			- cmd line option parsing added.
        June 2003
            - pSOS support added.
            - no DNS lookup supported -> IPs of the server as parameters			
        November 2003 
            - makefile review
            - DBG print modified  
		History not updated anymore. See readme.  		
*/
#define MODULE_TAG "INADYN: "
#include "debug_if.h"
#include <stdlib.h>
#include "errorcode.h"
#include "dyndns.h"
#if 1
/*[IreneWang@MSTC:TO2]$ DDNS update mechanism from polling to event driven and update RG wan IP Address, 20130917*/
#include "cms_log.h"
#include "cms_util.h"
#include "cms.h"
#include "activated.h"
#endif

#if 1 /* Support DDNS with inadyn, __FTTB7-CHT__, MitraStar chiahsin, 20120703 */
#include "cms_msg.h"
#include <signal.h>
int time_to_quit = 0;
char fileName[128]={0};
void *msgHandle=NULL; /* handle to communications link to smd */
void quit_handler(int signal)
{
   time_to_quit = 1;   
}

#if 1
/** Register or unregister our interest for some event events with smd. **/
void registerInterestInEvent(CmsMsgType msgType, UBOOL8 positive, void *msgData, UINT32 msgDataLen)
{
   CmsMsgHeader *msg = NULL;
   char *data = NULL;
   void *msgBuf = NULL;
   char *action = (positive) ? "REGISTER" : "UNREGISTER";
   CmsRet ret;

   if (msgData != NULL && msgDataLen != 0)
   {
      /* for msg with user data */
      msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader) + msgDataLen, ALLOC_ZEROIZE);
   }
   else
   {
      msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader), ALLOC_ZEROIZE);
   }

   msg = (CmsMsgHeader *)msgBuf;

   /* fill in the msg header */
   msg->type = (positive) ? CMS_MSG_REGISTER_EVENT_INTEREST : CMS_MSG_UNREGISTER_EVENT_INTEREST;
   msg->src = EID_DDNSD;
   msg->dst = EID_SMD;
   msg->flags_request = 1;
   msg->wordData = msgType;

   if (msgData != NULL && msgDataLen != 0)
   {
      data = (char *) (msg + 1);
      msg->dataLength = msgDataLen;
      memcpy(data, (char *)msgData, msgDataLen);
   }

   /* Only send the message. Do not wait for a reply in the context of the calling thread. 
   ** The reply will be received by the main message handling loop 
   */
   ret = cmsMsg_send(msgHandle, msg);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("%s_EVENT_INTEREST for 0x%x failed, ret=%d", action, msgType, ret);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);

   return;
}
UBOOL8 enable_debug(const char * filename)
{
	FILE * file = NULL;
	if (file = fopen(filename, "r"))
	{
		fclose(file);
		return TRUE;
	}
	return FALSE;
}
/* MAIN - Dyn DNS update entry point.*/
int inadyn_main(int argc, char* argv[])
{
	CmsMsgHeader *msg = NULL;
	CmsRet ret = CMSRET_SUCCESS;
	FILE *fPtr = NULL;
	int dual = 0;
	RegisterWANUpInterface correctInterface = WAN_UP_Not_Init;
	sprintf(fileName, "/var/ddnsd.cfg");
	
	FILE *fs = NULL;
	char tmpline[256]={0};
	int argc_1=0;
	char* val=NULL;
	char** argv_1[40]={0};

	RC_TYPE rc = -1;
	DYN_DNS_CLIENT *p_dyndns = NULL;

	cmsMsg_init(EID_DDNSD, &msgHandle);
	cmsLog_init(EID_DDNSD);
	if (enable_debug("/var/tmp/DDNS_WAN_UP_Interface")) {
		cmsLog_setLevel(LOG_LEVEL_DEBUG);
		cmsLog_debug("Enable Debug for Inadyn Process");
	}

	signal( SIGHUP, &quit_handler );
#if 0	/*[IreneWang@MSTC:TO2]$ Prevent Inadyn miss the SIGTERM, 20130304*/
	signal( SIGTERM, &quit_handler );
#endif  /*1*/
	signal( SIGQUIT, &quit_handler );
	
	while(!time_to_quit)			/* Leaving the loop when time_to_quit=1 */
	{
	   	ret = cmsMsg_receiveWithTimeout(msgHandle, &msg, 30000);		/* Receive message within 30s timeout*/
		if (ret == CMSRET_TIMED_OUT)
		{
			cmsLog_debug("Timeout after wait for 30s");
		      	if ((rc == RC_OK) || (rc == RC_OS_INVALID_IP_ADDRESS))
				time_to_quit = 1;				/*Only when update success and wait for wan up will leave the function*/
		}
		else if(ret != CMSRET_SUCCESS)
	      {
	      		cmsLog_error("cmsMsg_receive failed, ret=%d", ret);
	      }
	      else
		{
			if(msg->type)
				cmsLog_debug("receive message for msg 0x%x", msg->type);
			
			switch(msg->type)
			{
			case CMS_MSG_START_APP:
			case CMS_MSG_RESTART_APP:		/* Trigger the event from rutDDns_restartInadyn (config change) */
				fPtr = fopen("/var/ppp/isDual", "r");
				if (fPtr) {		/* Prevent dead when access nonexist file via library, so use fopen at begining time */
					fscanf(fPtr, "%d", &dual);
					if(correctInterface == WAN_UP_ALL)			/* Unregister wan up if there has been done before */
						registerInterestInEvent(CMS_MSG_WAN_CONNECTION_UP, FALSE, NULL, 0);
					if (dual) {
						if(correctInterface == WAN_UP_VOIP)		/* Prevent default False value in isDual file */
							registerInterestInEvent(CMS_MSG_WAN_CONNECTION_UP, FALSE, PPP1IFNAME, strlen(PPP1IFNAME));
						correctInterface = WAN_UP_BBI;
						registerInterestInEvent(CMS_MSG_WAN_CONNECTION_UP, TRUE, PPP2IFNAME, strlen(PPP2IFNAME));
						setActivatedStatus (DDNS_REGISTER_WAN_UP_INTERFACE, WAN_UP_BBI);
					}
					else {
						correctInterface = WAN_UP_VOIP;
						registerInterestInEvent(CMS_MSG_WAN_CONNECTION_UP, TRUE, PPP1IFNAME, strlen(PPP1IFNAME));
						setActivatedStatus (DDNS_REGISTER_WAN_UP_INTERFACE, WAN_UP_VOIP);
					}
					cmsLog_debug("Register interface %d wan up event", correctInterface);
					fclose(fPtr);
				}
				else {	/* File not ready(bootup time); listen all wan up and break here to wait wan up */
					correctInterface = WAN_UP_ALL;
					cmsLog_error("Fail to open file when register related wan interface, register for all wan up.");
					registerInterestInEvent(CMS_MSG_WAN_CONNECTION_UP, TRUE, NULL, 0);
					break;
				}
			case CMS_MSG_REGISTER_EVENT_INTEREST:
				if ((rc == RC_OK) && (msg->flags_response))
				{					/* If it has been update success at previous round, it not necessary to update when receive the responce */
					cmsLog_debug("Ignore register event response");
					break;			/*It meens last update success, so not update again.*/
				}
			case CMS_MSG_WAN_CONNECTION_UP:
				do
				{
					/* Initial variable before open config file */
					memset( tmpline, 0, sizeof( tmpline ));
					argc_1 = 0;
					val = NULL;
					memset( argv_1, 0, sizeof( argv_1 ));
					if ((fs = fopen(fileName, "r")) == NULL)
					{	/*Only remove the file when disable service or no enable rule, so unregister event and exit when file nonexist*/
						cmsLog_error("File open fail when reading DDNS config, unregister WAN UP event and exit.");
						if (correctInterface == WAN_UP_ALL) {
							registerInterestInEvent(CMS_MSG_WAN_CONNECTION_UP, FALSE, NULL, 0);
						}
						else if (correctInterface == WAN_UP_VOIP){
							registerInterestInEvent(CMS_MSG_WAN_CONNECTION_UP, FALSE, PPP1IFNAME, strlen(PPP1IFNAME));
						}
						else if (correctInterface == WAN_UP_BBI) {
							registerInterestInEvent(CMS_MSG_WAN_CONNECTION_UP, FALSE, PPP2IFNAME, strlen(PPP2IFNAME));
						}
						dyn_dns_destruct(p_dyndns);
						cmsMsg_cleanup(&msgHandle);
						cmsLog_cleanup();
						return 0;
					}

					/* Read config file into argv_1 array */
					while ( fgets(tmpline, 256, fs) != NULL )
					{
						val=strtok(tmpline, " ");
						argv_1[0]= malloc(sizeof(tmpline));
						memcpy(argv_1[0],val,sizeof(tmpline));

						while (val != NULL) 
						{
							argc_1++;		 
							val = strtok(NULL, " ");
							if(val!=NULL)
							{
								argv_1[argc_1] = malloc(sizeof( tmpline ));
								memset( argv_1[argc_1], 0, sizeof( tmpline ));    		  
								strncpy(argv_1[argc_1],val,sizeof(tmpline));
							}
						}   
						memset( tmpline, 0, sizeof( tmpline ));
					}
					fclose(fs);

					/* Create DYN_DNS_CLIENT object	*/
					// TODO: I have one concern about "p_dyndns" struct, when ddns construct successfully, it seams didn't free memory and alloc again in dyn_dns_construct() function. But process will exit if there are no event received, so it is not critcal.
				      rc = dyn_dns_construct(&p_dyndns);
					if (rc != RC_OK)
					{
						cmsLog_error("dyn construct fail!ret=%d.", rc);
					 	break;
					}

					rc = dyn_dns_main(p_dyndns, argc_1, argv_1);
					for(argc_1 = 0 ; argc_1 < sizeof(argv_1)/sizeof(argv_1[0]); argc_1++)		/* Loop to free argv_1 buffer [0~39] */
					{
						if(argv_1[argc_1])
							free(argv_1[argc_1]);
					}
					cmsLog_debug("The return value for dyn_dns_main is %d", rc);
				}while(!((rc == RC_OS_INVALID_IP_ADDRESS) || (rc == RC_ERROR) || (rc == RC_OK)));	/*Leave the loop when WAN not ready or receive msg or update success*/
				break;
			case CMS_MSG_STOP_APP:
				correctInterface = getActivatedStatus(DDNS_REGISTER_WAN_UP_INTERFACE);
				if (correctInterface == WAN_UP_ALL) {
					registerInterestInEvent(CMS_MSG_WAN_CONNECTION_UP, FALSE, NULL, 0);
				}
				else if (correctInterface == WAN_UP_VOIP){
					registerInterestInEvent(CMS_MSG_WAN_CONNECTION_UP, FALSE, PPP1IFNAME, strlen(PPP1IFNAME));
				}
				else if (correctInterface == WAN_UP_BBI) {
					registerInterestInEvent(CMS_MSG_WAN_CONNECTION_UP, FALSE, PPP2IFNAME, strlen(PPP2IFNAME));
				}
				rc = RC_OK;
				cmsLog_debug("Stop message, unregister %d wan up event", correctInterface);	/* It may stop others rule, then enable one rule, so it should wait.*/
				correctInterface = -1;
				break;
			default:
				cmsLog_debug("Not deal with this message : 0x%x", msg->type);
				break;
			}
		}
	}

	if ((correctInterface == WAN_UP_ALL) && (rc == RC_OK))
	{	/* Reregister wan if there has update success before isDual file ready */
		registerInterestInEvent(CMS_MSG_WAN_CONNECTION_UP, FALSE, NULL, 0);
		if (getActivatedStatus(ACTIVATION_ISDUAL)) {
			cmsLog_debug("Register ppp0.2 wan up success");
			registerInterestInEvent(CMS_MSG_WAN_CONNECTION_UP, TRUE, PPP2IFNAME, strlen(PPP2IFNAME));
			setActivatedStatus (DDNS_REGISTER_WAN_UP_INTERFACE, WAN_UP_BBI);
		}
		else {
			cmsLog_debug("Register ppp0.1 wan up success");
			registerInterestInEvent(CMS_MSG_WAN_CONNECTION_UP, TRUE, PPP1IFNAME, strlen(PPP1IFNAME));
			setActivatedStatus (DDNS_REGISTER_WAN_UP_INTERFACE, WAN_UP_VOIP);
		}
	}

	/* end of program */
	if ((rc != 0)&&(rc != RC_OS_INVALID_IP_ADDRESS))	/* Not printe error msg when leaving because wan not ready */
	{
		print_help_page();
		/* log error*/	
		DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "Main: Error '%s' (0x%x).\n", errorcode_get_name(rc), rc));
	}
	
	/* destroy DYN_DNS_CLIENT object*/
	rc = dyn_dns_destruct(p_dyndns);
	if (rc != RC_OK)
	{
		DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "Main: Error '%s' (0x%x) in dyn_dns_destruct().\n", errorcode_get_name(rc), rc));
	}
	 
	os_close_dbg_output();

	cmsLog_debug("End Inadyn Process");
	cmsMsg_cleanup(&msgHandle);
	cmsLog_cleanup();
	
	return (int) rc;
}
#else
int inadyn_main(int argc, char* argv[])
{
#if 1 /* Support DDNS with inadyn, __FTTB7-CHT__, MitraStar chiahsin, 20120703 */
   FILE *fs;
   char tmpline[256]={0};
   int argc_1=0;
   char *idx;
   char* val=NULL;
   char** argv_1[40]={0};

   strncpy(fileName,argv[1],sizeof(fileName));
   fs = fopen(fileName, "r");

   while ( fgets(tmpline, 256, fs) != NULL )
   {
      val=strtok(tmpline, " ");
      argv_1[0]= malloc(sizeof(tmpline));
      memcpy(argv_1[0],val,sizeof(tmpline));

      while (val != NULL)
      {
         argc_1++;
         val = strtok(NULL, " ");
         if(val!=NULL)
         {
            argv_1[argc_1]= malloc(sizeof( tmpline ));
            memset( argv_1[argc_1], 0, sizeof( tmpline ));
            strncpy(argv_1[argc_1],val,sizeof(tmpline));
         }
      }
      memset( tmpline, 0, sizeof( tmpline ));
   }

   fclose(fs);
#endif

   RC_TYPE rc = RC_OK;
   DYN_DNS_CLIENT *p_dyndns = NULL;

#if 1 /* Support DDNS with inadyn, __FTTB7-CHT__, MitraStar chiahsin, 20120703 */
   cmsMsg_init(EID_DDNSD, &msgHandle);
   signal( SIGHUP, &quit_handler );
/*[IreneWang@MSTC:TO2]$ Prevent Inadyn miss the SIGTERM, 20130304*/
#if 0
   signal( SIGTERM, &quit_handler );
#endif  /*1*/
   signal( SIGQUIT, &quit_handler );

   for(;!time_to_quit;)
#else
        do
#endif
   {
      /* create DYN_DNS_CLIENT object   */
      rc = dyn_dns_construct(&p_dyndns);
      if (rc != RC_OK)
      {
         break;
      }
#if 1 /* Support DDNS with inadyn, __FTTB7-CHT__, MitraStar chiahsin, 20120703 */
      rc = dyn_dns_main(p_dyndns, argc_1, argv_1);
#else
      rc = dyn_dns_main(p_dyndns, argc, argv);
#endif
   }
#if 0 /* Support DDNS with inadyn, __FTTB7-CHT__, MitraStar chiahsin, 20120703 */
        while(0);

#endif


        /* end of program */
        if (rc != 0)
        {
                print_help_page();
                /* log error*/
                DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "Main: Error '%s' (0x%x).\n", errorcode_get_name(rc), rc));
        }

        /* destroy DYN_DNS_CLIENT object*/
        rc = dyn_dns_destruct(p_dyndns);
        if (rc != RC_OK)
        {
                DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "Main: Error '%s' (0x%x) in dyn_dns_destruct().\n", errorcode_get_name(rc), rc));
        }


        os_close_dbg_output();
        return (int) rc;

}
#endif  /*1*/
#endif