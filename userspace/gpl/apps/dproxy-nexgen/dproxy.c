/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdarg.h>
#include <signal.h>
#include <syslog.h>
#include <linux/if.h>
#include "dproxy.h"
#include "dns_decode.h"
#include "conf.h"
#include "dns_list.h"
#include "dns_construct.h"
#include "dns_io.h"
#include "dns_dyn_cache.h"
#include "dns_mapping.h"

/*****************************************************************************/
/*****************************************************************************/
int dns_main_quit;
int dns_sock[2]={-1, -1};
fd_set rfds;
//BRCM
int dns_querysock[2]={-1, -1};
#ifdef DMP_X_ITU_ORG_GPON_1
static unsigned int dns_query_error = 0;
#endif

/* CMS message handle */
void *msgHandle=NULL;
int msg_fd;

/* WenJuin@MSTC: DNS Route, 20130319 */ 
#if 1
#include "dns_probe.h"
int dns_wanup = 0;
extern DNSRoute_Entry_t DNSRoute[DNSRoute_MAX_ENTRY];
extern int num_entry;
extern int num_wan;

extern UBOOL8 isActived;
extern UBOOL8 isNeedBBI;
extern UBOOL8 isVoipUP;
extern UBOOL8 isVoipv6UP;
extern UBOOL8 isBBIUP;
extern UBOOL8 isBBIv6UP;

/* WenJuin@MSTC: DNS Order, 20130427 */
DnsOrder VoipDnsOrder = DNSv4_1;	//the ACS/Voice/(NTP) query Dns Order in single/dual mode
DnsOrder BBIDnsOrder = DNSv6_1;	//the BBI(NTP) qeury Dns Order in single/dual mode
int Voip_total_retry = 0;	//Voip total retry times
int BBI_total_retry = 0;	//BBI total retry times
#endif

#if 1 /* ChenHe@MSTC for captive portal */
#include "activated.h"
#include "cms.h"
#include "cms_log.h"
#include "cms_util.h"
#endif

/*****************************************************************************/
/* WenJuin@MSTC: DNS Route, 20130320 */ 
#if 1
void Reset_list_count(Reset_Type action)
{
	if(action == NOT_RESET)
		return;

	cmsLog_notice("***** Start reset type[%d] ******",action);

	if(action == RESET_VOIP)
	{
		Voip_total_retry = 0;
	}else if(action == RESET_BBI)
	{
		BBI_total_retry = 0;
	}else if(action == RESET_ALL)
	{
		Voip_total_retry = 0;	
		BBI_total_retry = 0;
	}
		
	dns_request_t *r;
	for(r = dns_request_list; r; r = r->next){
		if((action == RESET_VOIP) && r->isDomainRt == 1)
		{
		       dns_list_remove( r );
		}else if((action == RESET_BBI) && r->isDomainRt == 0)
		{
		       dns_list_remove( r );
		}else if(action == RESET_ALL)
		{
		       dns_list_remove( r );
		}
		
		cmsLog_debug("id[0x%04x], cname[%s], isDomainRt[%d], retx_count[%d]\n",r->message.header.id,r->cname ,r->isDomainRt,r->retx_count);				
	}

	cmsLog_notice("VOIP Total Retry[%d], BBI Total Retry[%d]",Voip_total_retry,BBI_total_retry);
	cmsLog_notice("***** finish reset type[%d] ******",action);	
}

DnsOrder isChangeVoipDnsOrder(dns_request_t *retryEntry)
{
/* the change order is follow this rule:
*   [1] retx time is greater than "VOIP_SINGLE_RETYR_MAX" for same dns query
*   [2] total retx time is greater then VOIP_RETRY_MAX
*
*   Hint: this order is used in next run
*/	

	/* count retx time for check the order is need to change or not */
	retryEntry->retx_count = retryEntry->retx_count + 1;
	Voip_total_retry++;

	if(retryEntry->retx_count >= VOIP_SINGLE_RETYR_MAX || Voip_total_retry >= VOIP_RETRY_MAX)
	{
		if(retryEntry->retx_count >= VOIP_SINGLE_RETYR_MAX)
		{		
			cmsLog_debug("retry time[%d] greater then %d", retryEntry->retx_count,VOIP_SINGLE_RETYR_MAX);
		}else
		{
			cmsLog_debug("total retry time[%d] greater then %d \n", Voip_total_retry,VOIP_RETRY_MAX);		
		}	
		/* move to next dns order */
		switch(VoipDnsOrder)
		{
			case DNSv4_1:
				VoipDnsOrder = DNSv4_2;
				break;
			case DNSv4_2:
				/* fallback to first dns order */
				VoipDnsOrder = DNSv4_1;
				break;				
			default:
				break;
		}

		cmsLog_notice("====> change voip order to [%d]",VoipDnsOrder);
		Reset_list_count(RESET_VOIP);		
		return VoipDnsOrder;
	}
	return VoipDnsOrder;
}

DnsOrder isChangeBBIDnsOrder(dns_request_t *retryEntry)
{
/* the change order is follow this rule:
*   [1] retx time is greater than "BBI_SINGLE_RETYR_MAX" for same dns query
*   [2] total retx time is greater then BBI_RETRY_MAX
*
*   Hint: this order is used in next run
*/	

	/* count retx time for check the order is need to change or not */	
	retryEntry->retx_count = retryEntry->retx_count + 1;
	BBI_total_retry++;

	if(retryEntry->retx_count >= BBI_SINGLE_RETYR_MAX || BBI_total_retry >= BBI_RETRY_MAX)
	{
		if(retryEntry->retx_count >= BBI_SINGLE_RETYR_MAX)
		{
			cmsLog_debug("retry time[%d] greater then %d", retryEntry->retx_count,BBI_SINGLE_RETYR_MAX);			
		}else
		{
			cmsLog_debug("total retry time[%d] greater then %d \n", Voip_total_retry,BBI_RETRY_MAX);					
		}	
		/* move to next dns order */
		switch(BBIDnsOrder)
		{
			case DNSv6_1:
				BBIDnsOrder = DNSv6_2;
				break;	
			case DNSv6_2:
				if(isBBIUP){
					BBIDnsOrder = DNSv4_1;
				}else{
					BBIDnsOrder = DNSv6_1;
				}
				break;					
			case DNSv4_1:
				BBIDnsOrder = DNSv4_2;
				break;
			case DNSv4_2:
				/* fallback to first dns order */
				if(isBBIv6UP){
					BBIDnsOrder = DNSv6_1;
				}else{
					BBIDnsOrder = DNSv4_1;
				}
				break;				
			default:
				break;
		}
		
		cmsLog_notice("====> change voip order to [%d]",BBIDnsOrder);
		Reset_list_count(RESET_BBI);
		return BBIDnsOrder;
	}
	return BBIDnsOrder;
}

UBOOL8 VoipStatusChange(UBOOL8 isDual ,UBOOL8 status, UBOOL8 v6Status)
{	

	if(isVoipUP != status || isVoipv6UP != v6Status)
	{
		isVoipUP = status;
		isVoipv6UP = v6Status;
		
		/* Reset Voip query Dns Order */
		VoipDnsOrder = DNSv4_1;
		Reset_list_count(RESET_VOIP);
		return TRUE;
	}
	else
		return FALSE;
}

UBOOL8 BBIStatusChange(UBOOL8 isDual ,UBOOL8 status, UBOOL8 v6Status)
{
	if(isDual)
	{
		if(isBBIUP != status || isBBIv6UP != v6Status)
		{
			isBBIUP = status;
			isBBIv6UP = v6Status;
			
			/* Reset BBI query Dns Order */
			if(isBBIv6UP == TRUE){
				BBIDnsOrder = DNSv6_1;	//BBI support IPv6
			}else{
				BBIDnsOrder = DNSv4_1;	//BBI not support IPv6
			}
			Reset_list_count(RESET_BBI);		
			return TRUE;
		}
		else
			return FALSE;
	}else
	{
		/* the wan status is same voip in single session */
		if(isBBIUP != isVoipUP || isBBIv6UP != isVoipv6UP)
		{
			isBBIUP = isVoipUP;
			isBBIv6UP = isVoipv6UP;
			
			/* Reset BBI query Dns Order */
			if(isBBIv6UP == TRUE){
				BBIDnsOrder = DNSv6_1;	//BBI support IPv6
			}else{
				BBIDnsOrder = DNSv4_1;	//BBI not support IPv6
			}
			Reset_list_count(RESET_BBI);		
			return TRUE;
		}
		else
			return FALSE;
	}
}

void Init_wanStatus(UINT32 status,int *InterfaceStatus)
{
	switch(status)
	{
		case NONACTIVED:
			/* non-actived case */
			isActived = FALSE;
			isNeedBBI = FALSE;			
			break;
		case ACTIVED_NEEDBBI:
			/* actived & need BBI case */	
			isActived = TRUE;
			isNeedBBI = TRUE;			
			break;
		case ACTIVED_NOT_NEEDBBI:
			/* actived &  not need BBI case */
			isActived = TRUE;
			isNeedBBI = FALSE;				
			break;
		default:
			break;
	}

	switch(*InterfaceStatus)
	{
		/* Single mode */			
		case S_V4_DOWN_V6_DOWN:
			VoipStatusChange(FALSE, FALSE, FALSE);
			BBIStatusChange(FALSE, FALSE, FALSE);				
			break;
		case S_V4_DOWN_V6_UP:
			VoipStatusChange(FALSE, FALSE, TRUE);
			BBIStatusChange(FALSE, FALSE, TRUE);				
			break;		
		case S_V4_UP_V6_DOWN:
			VoipStatusChange(FALSE, TRUE, FALSE);
			BBIStatusChange(FALSE, TRUE, FALSE);				
			break;	
		case S_V4_UP_V6_UP:
			VoipStatusChange(FALSE, TRUE, TRUE);
			BBIStatusChange(FALSE, TRUE, TRUE);				
			break;				

		/* Dual mode => VOIPv6 is always down in single mode */			
		case D_VOIP_DOWN_BBIv4_DOWN_BBIv6_DOWN:
			VoipStatusChange(TRUE, FALSE, FALSE);
			BBIStatusChange(TRUE, FALSE, FALSE);				
			break;	
		case D_VOIP_DOWN_BBIv4_DOWN_BBIv6_UP:
			VoipStatusChange(TRUE, FALSE, FALSE);
			BBIStatusChange(TRUE, FALSE, TRUE);				
			break;	
		case D_VOIP_DOWN_BBIv4_UP_BBIv6_DOWN:
			VoipStatusChange(TRUE, FALSE, FALSE);
			BBIStatusChange(TRUE, TRUE, FALSE);				
			break;	
		case D_VOIP_DOWN_BBIv4_UP_BBIv6_UP:
			VoipStatusChange(TRUE, FALSE, FALSE);
			BBIStatusChange(TRUE, TRUE, TRUE);				
			break;	
		case D_VOIP_UP_BBIv4_DOWN_BBIv6_DOWN:
			VoipStatusChange(TRUE, TRUE, FALSE);
			BBIStatusChange(TRUE, FALSE, FALSE);				
			break;	
		case D_VOIP_UP_BBIv4_DOWN_BBIv6_UP:
			VoipStatusChange(TRUE, TRUE, FALSE);
			BBIStatusChange(TRUE, FALSE, TRUE);				
			break;	
		case D_VOIP_UP_BBIv4_UP_BBIv6_DOWN:
			VoipStatusChange(TRUE, TRUE, FALSE);
			BBIStatusChange(TRUE, TRUE, FALSE);				
			break;	
		case D_VOIP_UP_BBIv4_UP_BBIv6_UP:
			VoipStatusChange(TRUE, TRUE, FALSE);
			BBIStatusChange(TRUE, TRUE, TRUE);				
			break;				
   
		default:
			break;
	}	

	cmsLog_notice("## [Revieve]isActived[%d], isNeedBBI[%d]",isActived,isNeedBBI);
	cmsLog_notice("## [Revieve]VOIPv4=%d, VOIPv6=%d, BBIv4=%d, BBIv6=%d ====> DnsInterfaceStatus=%d\n",isVoipUP,isVoipv6UP,isBBIUP,isBBIv6UP,*InterfaceStatus);		
}

int lookup_domain_name_table(dns_request_t *m)
{
	int i=0;
	int found=0;
	if(getActivatedStatus(ACTIVATION_ISDUAL) == TRUE_STR)
	{

		for(i=0;i<num_entry;i++)
		{			
			if(memcmp(m->cname, DNSRoute[i].domainName, strlen(DNSRoute[i].domainName)) == 0)
			{
				found = 1;
				break;
			}
		}

		memset(m->primary, 0, sizeof(m->primary));
		memset(m->secondary, 0, sizeof(m->secondary));
	}
	
	if(found==1)
	{
		memcpy(m->primary, DNSRoute[i].primary, 16);
		memcpy(m->secondary, DNSRoute[i].secondary, 16);
		cmsLog_notice("### match table entry [%s], server[%s][%s].\n", DNSRoute[i].domainName, m->primary, m->secondary);
		return 1;
	}else{
		cmsLog_notice("### Not match. cname:[%s]\n##", m->cname);
		return 0;
	}
}

UBOOL8 isSpecialDomain(char *domain, char *type)
{
	int i=0;

	/* only search in Single mode */	
	if(getActivatedStatus(ACTIVATION_ISDUAL) != TRUE_STR)
	{
		for(i=0;i<num_entry;i++)
		{			
			if((memcmp(domain, DNSRoute[i].domainName, strlen(DNSRoute[i].domainName)) == 0) && 
				memcmp(type, DNSRoute[i].type, strlen(DNSRoute[i].type)) == 0)
			{
				cmsLog_notice("%s [type:%s] is special case in single mode.",domain,type);			
				return TRUE;
			}
		}
	}

	return FALSE;	
}

int rut_translateSubnetMask(char *strSubnetMask)
{
	int iBit;
	int ret = 0;
	struct in_addr srcIpMask;
	
	inet_aton(strSubnetMask, &srcIpMask);

	for(iBit = 0; iBit < 32; iBit++){
		if((srcIpMask.s_addr >> iBit) & 1){
			ret++;
		}
	}
	return ret;
}

void addDnsRoute(const char* ip, const char* cname)
{
	int i=0;
	int found=0;
	char cmd[128] = {0};
	char *p1=NULL, *p2=NULL;
	int mask=0;
	struct in_addr srcIP;
	struct in_addr srcMask;

	inet_aton(ip, &srcIP);
	
	for(i=0;i<num_entry;i++)
	{
		
		if(strstr(DNSRoute[i].domainName, "*") != NULL)
		{
			p1 = strstr(DNSRoute[i].domainName,".");
			p2 = strstr(cname, ".");

			if(p1 && p2)
			{
				if(memcmp(p1, p2, strlen(p1)) == 0)
				{
					found = 1;
					break;
				}
			}
		}
		else if(memcmp(cname, DNSRoute[i].domainName, strlen(DNSRoute[i].domainName)) == 0)
		{
			found = 1;
			break;
		}
	}

	if(found==1)
	{
		inet_aton(DNSRoute[i].subnetMask, &srcMask);
		
		srcIP.s_addr = srcIP.s_addr & srcMask.s_addr;

		mask = rut_translateSubnetMask(DNSRoute[i].subnetMask);
		sprintf(cmd, "ip route replace %s/%d via %s dev %s", inet_ntoa(srcIP), mask, DNSRoute[i].gateway, DNSRoute[i].ifcName);
		cmsLog_notice("[add route rule]cmd:%s\n",cmd);		  	
		system(cmd);	
		
		sprintf(cmd, "ip route replace %s/%d via %s dev %s table 10", inet_ntoa(srcIP), mask, DNSRoute[i].gateway, DNSRoute[i].ifcName);
		cmsLog_notice("[add route rule]cmd:%s\n",cmd);		  	
		system(cmd);
	}
}
#endif /* 1 */

int dns_init()
{
  struct addrinfo hints, *res, *p;
  int errcode;
  int ret, i, on=1;

  memset(&hints, 0, sizeof (hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  /*
   * BRCM:
   * Open sockets to receive DNS queries from LAN.
   * dns_sock[0] is used for DNS queries over IPv4
   * dns_sock[1] is used for DNS queries over IPv6
   */
  errcode = getaddrinfo(NULL, PORT_STR, &hints, &res);
  if (errcode != 0)
  {
     debug("gai err %d %s", errcode, gai_strerror(errcode));
     exit(1) ;
  }

  p = res;
  while (p)
  {
     if ( p->ai_family == AF_INET )   i = 0;
     else if ( p->ai_family == AF_INET6 )   i = 1;
     else {
        debug_perror("Unknown protocol!!\n");
        goto next_lan;
     }

     dns_sock[i] = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

     if (dns_sock[i] < 0)
     {
        debug("Could not create dns_sock[%d]", i);
        goto next_lan;
     }

#ifdef IPV6_V6ONLY
     if ( (p->ai_family == AF_INET6) && 
          (setsockopt(dns_sock[i], IPPROTO_IPV6, IPV6_V6ONLY, 
                      &on, sizeof(on)) < 0) )
     {
        debug_perror("Could not set IPv6 only option for LAN");
        close(dns_sock[i]);
        goto next_lan;
     }
#endif

     /* bind() the socket to the interface */
     if (bind(dns_sock[i], p->ai_addr, p->ai_addrlen) < 0){
        debug("dns_init: bind: dns_sock[%d] can't bind to port", i);
        close(dns_sock[i]);
     }

next_lan:
     p = p->ai_next;
  }

  freeaddrinfo(res);

  if ( (dns_sock[0] < 0) && (dns_sock[1] < 0) )
  {
     debug_perror("Cannot create sockets for LAN");
     exit(1) ;
  }

  /*
   * BRCM:
   * use different sockets to send queries to WAN so we can use ephemeral port
   * dns_querysock[0] is used for DNS queries sent over IPv4
   * dns_querysock[1] is used for DNS queries sent over IPv6
   */
  errcode = getaddrinfo(NULL, "0", &hints, &res);
  if (errcode != 0)
  {
     debug("gai err %d %s", errcode, gai_strerror(errcode));
     exit(1) ;
  }

  p = res;
  while (p)
  {
     if ( p->ai_family == AF_INET )   i = 0;
     else if ( p->ai_family == AF_INET6 )   i = 1;
     else
     {
        debug_perror("Unknown protocol!!\n");
        goto next_wan;
     }

     dns_querysock[i] = socket(p->ai_family, p->ai_socktype, 
                               p->ai_protocol);

     if (dns_querysock[i] < 0)
     {
        debug("Could not create dns_querysock[%d]", i);
        goto next_wan;
     }

#ifdef IPV6_V6ONLY
     if ( (p->ai_family == AF_INET6) && 
          (setsockopt(dns_querysock[i], IPPROTO_IPV6, IPV6_V6ONLY, 
                     &on, sizeof(on)) < 0) )
     {
        debug_perror("Could not set IPv6 only option for WAN");
        close(dns_querysock[i]);
        goto next_wan;
     }
#endif

     /* bind() the socket to the interface */
     if (bind(dns_querysock[i], p->ai_addr, p->ai_addrlen) < 0){
        debug("dns_init: bind: dns_querysock[%d] can't bind to port", i);
        close(dns_querysock[i]);
     }

next_wan:
     p = p->ai_next;
  }

  freeaddrinfo(res);

  if ( (dns_querysock[0] < 0) && (dns_querysock[1] < 0) )
  {
     debug_perror("Cannot create sockets for WAN");
     exit(1) ;
  }

  // BRCM: Connect to ssk
  if ((ret = cmsMsg_init(EID_DNSPROXY, &msgHandle)) != CMSRET_SUCCESS) {
  	debug_perror("dns_init: cmsMsg_init() failed");
	exit(1);
  }
  cmsMsg_getEventHandle(msgHandle, &msg_fd);

  dns_main_quit = 0;

  FD_ZERO( &rfds );
  if (dns_sock[0] > 0)
     FD_SET( dns_sock[0], &rfds );
  if (dns_sock[1] > 0)  
     FD_SET( dns_sock[1], &rfds );
  if (dns_querysock[0] > 0)  
     FD_SET( dns_querysock[0], &rfds );
  if (dns_querysock[1] > 0)  
     FD_SET( dns_querysock[1], &rfds );

  FD_SET( msg_fd, &rfds );

#ifdef DNS_DYN_CACHE
  dns_dyn_hosts_add();
#endif

/* WenJuin@MSTC: DNS Route, 20130320 */ 
#if 1
	VoipDnsOrder = DNSv4_1;
	BBIDnsOrder = DNSv6_1;	//suppose IPv6 is up
	Voip_total_retry = 0;
	BBI_total_retry = 0;
	load_domainRoute_table();
#endif /* 1 */

  return 1;
}

/*****************************************************************************/
void dns_handle_new_query(dns_request_t *m)
{
   int retval = -1;
   dns_dyn_list_t *dns_entry;
   char srcIP[INET6_ADDRSTRLEN];
   int query_type = m->message.question[0].type;

   cmsLog_notice("****************** New Query  ****************************");			

   if( query_type == A )
   {
     debug(".... query_type ==A"); 
      retval = 0;         
#ifdef DNS_DYN_CACHE
	//__MSTC__, JhihPing, /* support DNS cache for IPv6 */
      if ((dns_entry = dns_dyn_find(m))) 
      {
         strcpy(m->ip, inet_ntoa(dns_entry->addr));
         m->ttl = abs(dns_entry->expiry - time(NULL));
         retval = 1;
         debug(".... %s, srcPort=%d, retval=%d\n", m->cname, ((struct sockaddr_in *)&m->src_info)->sin_port, retval); 
      }
#endif      
   }
   else if( query_type == AAA)
   { /* IPv6 todo */
   debug(".... query_type ==AAA"); 
     retval = 0;
//__MSTC__, JhihPing, /* support DNS cache for IPv6 */
#ifdef DNS_DYN_CACHE
      if ((dns_entry = dns_dyn_find(m))) 
      {
         inet_ntop(AF_INET6, &dns_entry->addr6, m->ip, INET6_ADDRSTRLEN);
         m->ttl = abs(dns_entry->expiry6 - time(NULL));
         retval = 1;
         debug("TYPE=AAAA.... %s, srcPort=%d, retval=%d\n", m->cname, ((struct sockaddr_in6 *)&m->src_info)->sin6_port, retval); 
      }
#endif      
   }
   else if( query_type == PTR ) 
   {
      retval = 0;   
      /* reverse lookup */
#ifdef DNS_DYN_CACHE
      if ((dns_entry = dns_dyn_find(m))) 
      {
         strcpy(m->cname, dns_entry->cname);
         m->ttl = abs(dns_entry->expiry - time(NULL));
         retval = 1;
         debug("TYPE==PTR.... %s, srcPort=%d, retval=%d\n", m->cname,  ((struct sockaddr_in *)&m->src_info)->sin_port, retval); 
         
      }
#endif
   }  
   else /* BRCM:  For all other query_type including TXT, SPF... */
   {
       retval = 0;
   }
   
   inet_ntop( m->src_info.ss_family, 
              get_in_addr(&m->src_info), 
              srcIP, INET6_ADDRSTRLEN );
   cmsLog_notice("dns_handle_new_query: %s %s, srcPort=%d, retval=%d\n", 
                  m->cname, srcIP, 
                  ((struct sockaddr_in *)&m->src_info)->sin_port, 
                  retval);

  switch( retval )
  {
     case 0:
     {	
	char dns1[INET6_ADDRSTRLEN];	 	
	int proto;

	struct sockaddr_storage sa;
	int sock=-1;

	/* check query match DNSRoute or not ===> only using in Dual mode */
	if((getActivatedStatus(ACTIVATION_ISDUAL) == TRUE_STR) && (isVoipUP == TRUE || isVoipv6UP == TRUE) &&  lookup_domain_name_table(m))
	{	
              cmsLog_notice("****************** VOIP  ****************************");
	
		/* DNS route packet */
		m->isDomainRt = 1;	//mark to dns route query

		switch(VoipDnsOrder)
		{
			case DNSv4_1:
				if(cmsUtl_strcmp(m->primary, "0.0.0.0") && cmsUtl_strcmp(m->primary, ""))
				{
					/* for single dns address */
					memcpy(dns1, m->primary, 16);
				}
				break;
			case DNSv4_2:
				 if(cmsUtl_strcmp(m->secondary, "0.0.0.0") && cmsUtl_strcmp(m->secondary, ""))
				{
					/* for single dns address */
					memcpy(dns1, m->secondary, 16);
				}
				break;
			default:
				break;
		}
		
		if (cmsUtl_isValidIpAddress(AF_INET,dns1))
		{
			proto = AF_INET;
			sock = dns_querysock[0];
			((struct sockaddr_in *)&sa)->sin_port = PORT;
			inet_pton(proto, dns1, &(((struct sockaddr_in *)&sa)->sin_addr));		
		}
	
		sa.ss_family = proto;
		
		dns_list_add(m);
		dns_write_packet( sock, &sa, m );

		cmsLog_notice("[DNS route], dns[%s] in retx_count[%d], cname[%s]", dns1,m->retx_count,m->cname);
	
	}else if((isBBIUP == TRUE || isBBIv6UP == TRUE) && dns_mapping_find_dns_ip(&(m->src_info), query_type, 
                                    dns1, &proto
#if 1 /* Jayden@MSTC for TO2_Germany DNS behavior, 20130307 */	/* first query, retx_count == 0, device should use primary dns server */
	, BBIDnsOrder
	,m->cname //FOR SPECIAL
#endif
	) == TRUE){	
	
		m->isDomainRt = 0; //mark to BBI query

		cmsLog_notice("Found dns server %s for subnet %s", dns1, srcIP);

		if (proto == AF_INET && cmsUtl_isValidIpAddress(AF_INET,dns1))
		{
			sock = dns_querysock[0];
			((struct sockaddr_in *)&sa)->sin_port = PORT;
			inet_pton(proto, dns1, &(((struct sockaddr_in *)&sa)->sin_addr));		
		}
		else if(proto == AF_INET6&& cmsUtl_isValidIpAddress(AF_INET6,dns1))
		{
			sock = dns_querysock[1];
			((struct sockaddr_in6 *)&sa)->sin6_port = PORT;
			inet_pton( proto, dns1, 
			&(((struct sockaddr_in6 *)&sa)->sin6_addr) );
		}
		
		sa.ss_family = proto;
		
		dns_list_add(m);
		dns_write_packet( sock, &sa, m );
		cmsLog_notice("[Normal route], dns[%s] in retx_count[%d], cname[%s]", dns1,m->retx_count,m->cname);
	}
	else
	{
		cmsLog_debug("No dns service.");
        }   
        break;
     }

     case 1: 		
        dns_construct_reply( m );
#if 1 //__MSTC__, JhihPing, /* support DNS cache for IPv6 */
        if (m->src_info.ss_family == AF_INET6)
           dns_write_packet(dns_sock[1], &m->src_info, m);
        else
#endif
        /* Only IPv4 can support caching now */
        dns_write_packet(dns_sock[0], &m->src_info, m);
        break;

     default:
        debug("Unknown query type: %d\n", query_type);
   }
}
/*****************************************************************************/
void dns_handle_request(dns_request_t *m)
{
  dns_request_t *ptr = NULL;
//  dns_dyn_list_t *dns_entry; 

  /* request may be a new query or a answer from the upstream server */
  ptr = dns_list_find_by_id(m);

  if( ptr != NULL ){
      debug("Found query in list; id 0x%04x flags 0x%04x\n ip %s - cname %s\n", 
             m->message.header.id, m->message.header.flags.flags, 
             m->ip, m->cname);
      cmsLog_debug("Found query in list; id 0x%04x flags 0x%04x\n ip %s - cname %s",m->message.header.id, m->message.header.flags.flags,m->ip, m->cname);
      cmsLog_notice("voip[order:%d][Total:%d] ------- BBI[order:%d][Total:%d] ==> sigle retx time[%d]",VoipDnsOrder,Voip_total_retry,BBIDnsOrder,BBI_total_retry,ptr->retx_count);

      /* message may be a response */
      if( m->message.header.flags.flags & 0x8000)
      {
          cmsLog_notice("****************** success get response  ****************************");			
      
          int sock;
          char srcIP[INET6_ADDRSTRLEN];

#if 1
          if( !(m->message.header.flags.flags & 0xF) ){
		   /* lookup success */
                 cmsLog_notice("****************** success get Answer  ****************************");			
		   /* WenJuin@MSTC: DNS Route for data packet route, 20130320 */ 
	          if((ptr->isDomainRt == 1) && (getActivatedStatus(ACTIVATION_ISDUAL) == TRUE_STR)){
	             cmsLog_debug("add Dns Route rule");						  	
	             addDnsRoute(m->ip, m->cname);
	          }
		
	          if(ptr->isDomainRt == 1){
			Voip_total_retry = (Voip_total_retry - ptr->retx_count);
			if(Voip_total_retry<0){
				Voip_total_retry = 0;
			}

			ptr->retx_count = 0;	//reset retx to 0
	          }
		   else
	          {
			BBI_total_retry = (BBI_total_retry - ptr->retx_count);
			if(BBI_total_retry<0){
				BBI_total_retry = 0;
			}	

			ptr->retx_count = 0;	//reset retx to 0
	          }	

          }      
#endif /* 1 */


          if (ptr->src_info.ss_family == AF_INET)
             sock = dns_sock[0];
          else
             sock = dns_sock[1];

          inet_ntop( m->src_info.ss_family, 
                     get_in_addr(&m->src_info), 
                     srcIP, INET6_ADDRSTRLEN );
   
          debug("Replying with answer from %s, id 0x%04x\n", 
                 srcIP, m->message.header.id);

          dns_write_packet( sock, &ptr->src_info, m );
          dns_list_unarm_requests_after_this( ptr );  // BRCM	   

#ifdef DMP_X_ITU_ORG_GPON_1
          if( m->message.header.flags.f.rcode != 0 ){ // lookup failed
              dns_query_error++;
              debug("dns_query_error = %u\n", dns_query_error);
          }
#endif

#ifdef DNS_DYN_CACHE
          //__MSTC__, JhihPing, /* support DNS cache for IPv6 */
          dns_dyn_cache_add(m);
#endif
#if 1
	   /* lookup success */
          if( !(m->message.header.flags.flags & 0xF) ){
	          cmsLog_notice("remove %p out List",ptr);		  
		   dns_list_remove( ptr ); 		//if success	 
          }
#endif
	}
	else
	{
              cmsLog_notice("****************** Retx  ****************************");
	
		debug("Duplicate query to %s (retx_count=%d)", m->cname, ptr->retx_count);
/* WenJuin@MSTC: DNS query behavior, 20130328 */ 
#if 1			
		int query_type = m->message.question[0].type;
		UBOOL8 found = FALSE;
		dns_dyn_list_t *dns_entry;
					
		if( query_type == PTR ) 
		{
			
			/* reverse lookup */
#ifdef DNS_DYN_CACHE
			if ((dns_entry = dns_dyn_find_host(m))) 
			{
				strcpy(m->cname, dns_entry->cname);
				m->ttl = abs(dns_entry->expiry - time(NULL));
				found = TRUE;
				debug("TYPE==PTR.... %s, srcPort=%d\n", m->cname,  ((struct sockaddr_in *)&m->src_info)->sin_port); 
			}

			if(found)
			{
				dns_construct_reply( m );

				if (m->src_info.ss_family == AF_INET6){
					dns_write_packet(dns_sock[1], &m->src_info, m);
				}else{
				/* Only IPv4 can support caching now */
					dns_write_packet(dns_sock[0], &m->src_info, m);
				}
			}
#endif
		   dns_list_remove( ptr ); 

		}  
#endif /* 1 */
		
		if (!found)
		{	
			char dns1[INET6_ADDRSTRLEN];	 	
			int proto;
			struct sockaddr_storage sa;
			int sock=-1;

			/* check query match DNSRoute or not */
			if((getActivatedStatus(ACTIVATION_ISDUAL) == TRUE_STR) && (isVoipUP == TRUE || isVoipv6UP == TRUE) &&  lookup_domain_name_table(m))
			{
		              cmsLog_notice("****************** VOIP  ****************************");

				/* re-mark dns query type to VOIP query */
				ptr->isDomainRt = 1;
					
				/* check is change order for this dns query */
				isChangeVoipDnsOrder(ptr);
				
				cmsLog_error("=>send again(Voip)\n");
				cmsLog_error("isDomainRt=%d ,retx_count=%d",ptr->isDomainRt, ptr->retx_count);		

				switch(VoipDnsOrder)
				{
					case DNSv4_1:
						if(cmsUtl_strcmp(m->primary, "0.0.0.0") && cmsUtl_strcmp(m->primary, ""))
						{
							/* for single dns address */
							memcpy(dns1, m->primary, 16);
							break;
						}
					case DNSv4_2:

						 if(cmsUtl_strcmp(m->secondary, "0.0.0.0") && cmsUtl_strcmp(m->secondary, ""))
						{
							/* for single dns address */
							memcpy(dns1, m->secondary, 16);
							break;
						}
					default:
						break;
				}				

				if (cmsUtl_isValidIpAddress(AF_INET,dns1))
				{
					proto = AF_INET;
					sock = dns_querysock[0];
					((struct sockaddr_in *)&sa)->sin_port = PORT;
					inet_pton(proto, dns1, &(((struct sockaddr_in *)&sa)->sin_addr));
				}

				sa.ss_family = proto;
				cmsLog_notice("[DNS route], dns[%s] in retx_count[%d], cname[%s]", dns1,ptr->retx_count,m->cname);
				dns_write_packet( sock, &sa, m );					

			}
			else if((isBBIUP == TRUE || isBBIv6UP == TRUE) && (dns_mapping_find_dns_ip(&(m->src_info),m->message.question[0].type, dns1, &proto, isChangeBBIDnsOrder(ptr), m->cname) == TRUE))
			{

				/* re-mark dns query type to BBI query for NTP fallback */
				ptr->isDomainRt = 0;

				cmsLog_debug("=>send again(BBI)\n");
				cmsLog_debug("isDomainRt=%d ,retx_count=%d",ptr->isDomainRt, ptr->retx_count);		

				if (proto == AF_INET && cmsUtl_isValidIpAddress(AF_INET,dns1))
				{
					sock = dns_querysock[0];
					((struct sockaddr_in *)&sa)->sin_port = PORT;
					inet_pton(proto, dns1, &(((struct sockaddr_in *)&sa)->sin_addr));
				}
				else if (proto == AF_INET6 && cmsUtl_isValidIpAddress(AF_INET6,dns1))
				{
					sock = dns_querysock[1];
					((struct sockaddr_in6 *)&sa)->sin6_port = PORT;
					inet_pton( proto, dns1, 
					&(((struct sockaddr_in6 *)&sa)->sin6_addr) );				
				}

				sa.ss_family = proto;
				cmsLog_notice("[Normal route], dns[%s] in retx_count[%d], cname[%s]", dns1,ptr->retx_count,m->cname);		
				dns_write_packet( sock, &sa, m );	
			
			}		
			else	
			{
		              cmsLog_notice("****************** Not found dns server  ****************************");				
			
				/* Not find Dns Server ,minus retx and BBI_total_retry */	
				ptr->retx_count = ptr->retx_count - 1;
				BBI_total_retry--;
				cmsLog_notice("minus count ---->BBI[single:%d][Total:%d]",ptr->retx_count,BBI_total_retry);
			}
		}
	}
  }
  else
  {
      dns_handle_new_query( m );
  }

  debug("dns_handle_request: done\n\n");
}

/*****************************************************************************/
static void processCmsMsg(void)
{
  CmsMsgHeader *msg;
  CmsRet ret;

  while( (ret = cmsMsg_receiveWithTimeout(msgHandle, &msg, 0)) == CMSRET_SUCCESS) {
    switch(msg->type) {
    case CMS_MSG_SET_LOG_LEVEL:
      cmsLog_debug("got set log level to %d", msg->wordData);
      cmsLog_setLevel(msg->wordData);
      if ((ret = cmsMsg_sendReply(msgHandle, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS) {
        cmsLog_error("send response for msg 0x%x failed, ret=%d", msg->type, ret);
      }
      break;
    case CMS_MSG_SET_LOG_DESTINATION:
      cmsLog_debug("got set log destination to %d", msg->wordData);
      cmsLog_setDestination(msg->wordData);
      if ((ret = cmsMsg_sendReply(msgHandle, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS) {
        cmsLog_error("send response for msg 0x%x failed, ret=%d", msg->type, ret);
      }
      break;

    case CMS_MSG_DNSPROXY_RELOAD:
      cmsLog_debug("received CMS_MSG_DNSPROXY_RELOAD\n");
      /* Reload config file */
#ifdef DNS_DYN_CACHE
      dns_dyn_hosts_add();
#endif

      /* load the /var/dnsinfo.conf into the linked list for determining
      * which dns ip to use for the dns query.
      */
      dns_mapping_conifg_init();

/* WenJuin@MSTC: DNS Route, 20130320 */ 
#if 1
	load_domainRoute_table();
	Init_wanStatus(msg->wordData,(int *)(msg+1));
#endif /* 1 */
	 
      /*
       * During CDRouter dhcp scaling tests, this message is sent a lot to dnsproxy.
       * To make things more efficient/light weight, the sender of the message does
       * not expect a reply.
       */
      break;

#ifdef SUPPORT_DEBUG_TOOLS

    case CMS_MSG_DNSPROXY_DUMP_STATUS:
       printf("\n============= Dump dnsproxy status=====\n");
       printf("config.name_server=%s config.domain=%s\n",
              config.name_server, config.domain_name);
       dns_list_print();
       dns_DomainRt_print();   
       dns_dyn_print();
       break;

    case CMS_MSG_DNSPROXY_DUMP_STATS:
       printf("stats dump not implemented yet\n");
       break;

#endif /* SUPPORT_DEBUG_TOOLS */

#ifdef DMP_X_ITU_ORG_GPON_1
    case CMS_MSG_DNSPROXY_GET_STATS:
    {
        char buf[sizeof(CmsMsgHeader) + sizeof(DnsGetStatsMsgBody)]={0};
        CmsMsgHeader *replyMsgPtr = (CmsMsgHeader *) buf;
        DnsGetStatsMsgBody *dnsStats = (DnsGetStatsMsgBody *) (replyMsgPtr+1);


        // Setup response message.
        replyMsgPtr->type = msg->type;
        replyMsgPtr->dst = msg->src;
        replyMsgPtr->src = EID_DNSPROXY;
        replyMsgPtr->flags.all = 0;
        replyMsgPtr->flags_response = 1;
        //set dns query error counter 
        replyMsgPtr->dataLength = sizeof(DnsGetStatsMsgBody);
        dnsStats->dnsErrors = dns_query_error;

        cmsLog_notice("dns query error is %d", dns_query_error);
        // Attempt to send CMS response message & test result.
        ret = cmsMsg_send(msgHandle, replyMsgPtr);
        if (ret != CMSRET_SUCCESS)
        {
           // Log error.
           cmsLog_error("Send message failure, cmsReturn: %d", ret);
        }
    }
    break;
#endif

    default:
      cmsLog_error("unrecognized msg 0x%x", msg->type);
      break;
    }
    CMSMEM_FREE_BUF_AND_NULL_PTR(msg);
  }
  
  if (ret == CMSRET_DISCONNECTED) {
    cmsLog_error("lost connection to smd, exiting now.");
    dns_main_quit = 1;
  }
}

/*****************************************************************************/
int dns_main_loop()
{
    struct timeval tv, *ptv;
    fd_set active_rfds;
    int retval;
    dns_request_t m;
    dns_request_t *ptr, *next;
    while( !dns_main_quit )
    {
 
      if (tv.tv_sec == 0) { /* To wait indefinitely */
         ptv = NULL;
         debug("\n\n=============select will wait indefinitely============");
      } else {
        tv.tv_usec = 0;
        ptv = &tv;
        debug("select timeout = %lu seconds", tv.tv_sec);
     }

      /* copy the main rfds in the active one as it gets modified by select*/
      active_rfds = rfds;
      retval = select( FD_SETSIZE, &active_rfds, NULL, NULL, ptv );

      if (retval){
         cmsLog_notice("\n\nreceived something");
         if (FD_ISSET(msg_fd, &active_rfds)) { /* message from ssk */
            cmsLog_notice("received cms message");
            processCmsMsg();
         } else if ((dns_sock[0] > 0) && FD_ISSET(dns_sock[0], &active_rfds)) {
            cmsLog_notice("received DNS message (LAN side IPv4)");
            /* data is now available */
            bzero(&m, sizeof(dns_request_t));
            //BRCM
#if 1/*DNS_DYN_CACHE only for IPv4 ,MitraSatr Jayden,20130227*/
    		m.addrtype=AF_INET;
#endif            
            if (dns_read_packet(dns_sock[0], &m) == 0) {
               dns_handle_request( &m );
            }
         } else if ((dns_sock[1] > 0) && FD_ISSET(dns_sock[1], &active_rfds)) {
            cmsLog_notice("received DNS message (LAN side IPv6)");
            /* data is now available */
            bzero(&m, sizeof(dns_request_t));
 #if 1/*DNS_DYN_CACHE only for IPv4 ,MitraSatr Jayden,20130227*/
 		m.addrtype=AF_INET6;
#endif			
            //BRCM
            if (dns_read_packet(dns_sock[1], &m) == 0) {
               dns_handle_request( &m );
            }
         } else if ((dns_querysock[0] > 0) && 
                    FD_ISSET(dns_querysock[0], &active_rfds)) {
            cmsLog_notice("received DNS response (WAN side IPv4)");
            bzero(&m, sizeof(dns_request_t));
 #if 1/*DNS_DYN_CACHE only for IPv4 ,MitraSatr Jayden,20130227*/
    		m.addrtype=AF_INET;
#endif   			
            if (dns_read_packet(dns_querysock[0], &m) == 0)
               dns_handle_request( &m );
         } else if ((dns_querysock[1] > 0) && 
                    FD_ISSET(dns_querysock[1], &active_rfds)) {
            cmsLog_notice("received DNS response (WAN side IPv6)");
            bzero(&m, sizeof(dns_request_t));
 #if 1/*DNS_DYN_CACHE only for IPv4 ,MitraSatr Jayden,20130227*/
    		m.addrtype=AF_INET6;
#endif   			
            if (dns_read_packet(dns_querysock[1], &m) == 0)
               dns_handle_request( &m );
         }
      } else { /* select time out */
         time_t now = time(NULL);
         ptr = dns_request_list;
         while (ptr) {
            next = ptr->next;

            if (ptr->expiry <= now) {
               char date_time_buf[DATE_TIME_BUFLEN];
               get_date_time_str(date_time_buf, sizeof(date_time_buf));

               debug("removing expired request %p\n", ptr);
               cmsLog_debug("%s dnsproxy: query for %s timed out after %d secs "
                      "(type=%d switch_on_timeout=%d retx_count=%d)",
                      date_time_buf, ptr->cname, DNS_TIMEOUT,
                      (unsigned int) ptr->message.question[0].type,
                      ptr->switch_on_timeout, ptr->retx_count);

               /* switch_on_timeout code removed here for not to support probing */
               dns_list_remove(ptr);
            }

            ptr = next;
         }

      } /* if (retval) */

    }  /* while (!dns_main_quit) */
   return 0;
}


/*
 * Return a buffer which contains the current date/time.
 */
void get_date_time_str(char *buf, unsigned int buflen)
{
	time_t t;
	struct tm *tmp;

	memset(buf, 0, buflen);

	t = time(NULL);
	tmp = localtime(&t);
	if (tmp == NULL) {
		debug_perror("could not get localtime");
		return;
	}

	strftime(buf, buflen, "[%F %k:%M:%S]", tmp);

}


/*****************************************************************************/
void debug_perror( char * msg ) {
	debug( "%s : %s\n" , msg , strerror(errno) );
}

#if 0 //BRCM: Mask the debug() function. It's redefined by cmsLog_debug()
#ifdef DPROXY_DEBUG
/*****************************************************************************/
void debug(char *fmt, ...)
{
#define MAX_MESG_LEN 1024
  
  va_list args;
  char text[ MAX_MESG_LEN ];
  
  sprintf( text, "[ %d ]: ", getpid());
  va_start (args, fmt);
  vsnprintf( &text[strlen(text)], MAX_MESG_LEN - strlen(text), fmt, args);	 
  va_end (args);
  
  printf(text);
#if 0 //BRCM 
  if( config.debug_file[0] ){
	 FILE *fp;
	 fp = fopen( config.debug_file, "a");
	 if(!fp){
		syslog( LOG_ERR, "could not open log file %m" );
		return;
	 }
	 fprintf( fp, "%s", text);
	 fclose(fp);
  }
  

  /** if not in daemon-mode stderr was not closed, use it. */
  if( ! config.daemon_mode ) {
	 fprintf( stderr, "%s", text);
  }
#endif
}

#endif
#endif
/*****************************************************************************
 * print usage informations to stderr.
 * 
 *****************************************************************************/
void usage(char * program , char * message ) {
  fprintf(stderr,"%s\n" , message );
  fprintf(stderr,"usage : %s [-c <config-file>] [-d] [-h] [-P]\n", program );
  fprintf(stderr,"\t-c <config-file>\tread configuration from <config-file>\n");
  fprintf(stderr,"\t-d \t\trun in debug (=non-daemon) mode.\n");
  fprintf(stderr,"\t-D \t\tDomain Name\n");
  fprintf(stderr,"\t-P \t\tprint configuration on stdout and exit.\n");
  fprintf(stderr,"\t-v \t\tset verbosity, where num==0 is LOG_ERROR, 1 is LOG_NOTICE, all others is LOG_DEBUG\n");
  fprintf(stderr,"\t-h \t\tthis message.\n");
}
/*****************************************************************************
 * get commandline options.
 * 
 * @return 0 on success, < 0 on error.
 *****************************************************************************/
int get_options( int argc, char ** argv ) 
{
  char c = 0;
  int not_daemon = 0;
  int want_printout = 0;
  char * progname = argv[0];
  SINT32 logLevelNum;
  CmsLogLevel logLevel=DEFAULT_LOG_LEVEL;
  //UBOOL8 useConfiguredLogLevel=TRUE;

  cmsLog_init(EID_DNSPROXY);

  conf_defaults();
#if 1 
  while( (c = getopt( argc, argv, "cD:dhPv:m:")) != EOF ) {
    switch(c) {
	 case 'c':
  		conf_load(optarg);
		break;
	 case 'd':
		not_daemon = 1;
		break;
     	 case 'D':
        	strcpy(config.domain_name, optarg);
        	break;
	 case 'h':
		usage(progname,"");
		return -1;
	 case 'P':
		want_printout = 1;
		break;
        case 'v':
         	logLevelNum = atoi(optarg);
         	if (logLevelNum == 0)
         	{
            		logLevel = LOG_LEVEL_ERR;
         	}
         	else if (logLevelNum == 1)
         	{
            		logLevel = LOG_LEVEL_NOTICE;
         	}
         	else
         	{
            		logLevel = LOG_LEVEL_DEBUG;
         	}
         	cmsLog_setLevel(logLevel);
         	//useConfiguredLogLevel = FALSE;
         	break;
	 default:
		usage(progname,"");
		return -1;
    }
  }
#endif

#if 0  
  /** unset daemon-mode if -d was given. */
  if( not_daemon ) {
	 config.daemon_mode = 0;
  }
  
  if( want_printout ) {
	 conf_print();
	 exit(0);
  }
#endif 

  return 0;
}
/*****************************************************************************/
void sig_hup (int signo)
{
  signal(SIGHUP, sig_hup); /* set this for the next sighup */
  conf_load (config.config_file);
}
/*****************************************************************************/
int main(int argc, char **argv)
{

  /* get commandline options, load config if needed. */
  if(get_options( argc, argv ) < 0 ) {
	  exit(1);
  }

  /* detach from terminal and detach from smd session group. */
  if (setsid() < 0)
  {
    cmsLog_error("could not detach from terminal");
    exit(-1);
  }

  /* ignore some common, problematic signals */
  signal(SIGINT, SIG_IGN);
  signal(SIGPIPE, SIG_IGN);

  signal(SIGHUP, sig_hup);
  dns_init();

//BRCM: Don't fork a task again!
#if 0 
  if (config.daemon_mode) {
    /* Standard fork and background code */
    switch (fork()) {
	 case -1:	/* Oh shit, something went wrong */
		debug_perror("fork");
		exit(-1);
	 case 0:	/* Child: close off stdout, stdin and stderr */
		close(0);
		close(1);
		close(2);
		break;
	 default:	/* Parent: Just exit */
		exit(0);
    }
  }
#endif
  dns_main_loop();

  return 0;
}

