#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cms_params.h>

#include "dproxy.h"
#include "conf.h"
#include "dns_list.h"
#include "dns_probe.h"

#ifdef SUPPORT_TELEFONICA /* 3G redirect page not works on domain name , __TELEFONICA__, MitraStar Curtis, 20120213 */
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#endif

#ifdef DESKTOP_LINUX
#define REAL_DNS_FYI_FILENAME   "./dns.real"
#else
#ifdef MSTC_COMMON_PATCH // __MTS__, Richard Huang
#define REAL_DNS_FYI_FILENAME   "/var/fyi/sys/dns_config"
#else
#define REAL_DNS_FYI_FILENAME   "/var/fyi/sys/dns.real"
#endif
#endif

extern int dns_wanup;
extern int dns_querysock;
static char primary_ns[CMS_IPADDR_LENGTH];
static char secondary_ns[CMS_IPADDR_LENGTH];
static struct sockaddr_in probe_addr;
#ifdef DNS_PROBE
static time_t probe_next_time;
static int probe_tried;
static time_t probe_timeout;
static uint16_t probe_id;
static char probe_pkt[36] =
	{0x0, 0x0, 0x1, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x1, 'a', 0xc, 'r', 'o', 'o', 't', '-', 's', 'e', 'r', 'v',
	 'e', 'r', 's', 0x3, 'n', 'e', 't', 0x0, 0x0, 0x1, 0x0, 0x1, };
#else
time_t dns_recover_time;
#endif

/* WenJuin@MSTC: DNS Route, 20130319 */ 
#if 1
DNSRoute_Entry_t DNSRoute[DNSRoute_MAX_ENTRY];
int num_wan=0;
int num_entry=0;
UBOOL8 isActived = FALSE;
UBOOL8 isNeedBBI = FALSE;
UBOOL8 isVoipUP = FALSE;
UBOOL8 isVoipv6UP = FALSE;
UBOOL8 isBBIUP = FALSE;
UBOOL8 isBBIv6UP = FALSE;
#endif /* 1 */

/* WenJuin@MSTC: DNS Route, 20130320 */ 
#if 1
/* Load Domain Name table and related WAN Interface*/
int load_domainRoute_table(void)
{
	FILE * fp;
	char line[256];
	int i=0, k=0;
	char *second = NULL;
	char dns_primary[DNSRoute_MAX_WAN_CONN][16]={{0},{0},{0},{0}};   // MitraStar, Paul Ho, Fix exception if number of WAN Connections with DSN Route over than 5
	char dns_secondary[DNSRoute_MAX_WAN_CONN][16]={{0},{0},{0},{0}};
	char dns_ifc[DNSRoute_MAX_WAN_CONN][8]={{0},{0},{0},{0}};
	char gw[DNSRoute_MAX_WAN_CONN][16]={{0},{0},{0},{0}};
	char *token = NULL;
	int len;

	if((fp = fopen(STATIC_DNS_FILE_WAN, "r")) == NULL)
	{
		cmsLog_error("could not open tmp file %s", STATIC_DNS_FILE_WAN);
		return 0;
	}
	
	while(fgets(line, sizeof(line), fp)!=NULL)
	{
	   len = strlen(line);
	   if (line[len-1] == '\n'){
	        line[len-1] = '\0';
	   }
	
	   if (i < DNSRoute_MAX_WAN_CONN) // MitraStar, Paul Ho, Fix exception if number of WAN Connections with DSN Route over than 5
	   {
   		memset(dns_ifc[i], 0, sizeof(dns_ifc[i]));
   		memset(gw[i], 0, sizeof(gw[i]));
   		memset(dns_primary[i], 0, sizeof(dns_primary[i]));
   		memset(dns_secondary[i], 0, sizeof(dns_secondary[i]));

   		debug("%s %d : line[%s]\n",__FUNCTION__,__LINE__, line);
   		
   		line[sizeof(line)-1] = 0; /* over size string */
   		token = strtok(line, ":");
   		snprintf(dns_ifc[i], sizeof(dns_ifc[i]), "%s", token);


   		if((token = strtok(NULL, ":")) != NULL) 
   		{
   			snprintf(gw[i], sizeof(gw[i]), "%s", token);
   			if((token = strtok(NULL, ":")) != NULL)
   			{
   				second = strstr(token, ",");
   				if(second!=NULL)
   				{
   					*second = 0;
   					second++;
   					while((isspace(*second)) && (*second!=0))
   						second++;
   					snprintf(dns_secondary[i], sizeof(dns_secondary[i]),"%s", second);
   				}
   				snprintf(dns_primary[i], sizeof(dns_primary[i]),"%s", token);
   			}				
   		}				
   			
   		debug("WAN[%d]:dns_ifc[%s], gw[%s], dns_primary[%s], dns_primary[%s]\n",i, dns_ifc[i], gw[i], dns_primary[i], dns_secondary[i]);

   		memset(line, 0, 256);
   		i++;
	   }
	   else
	   {
   		debug("number of WAN connections with DNS Route over than %d, ignore.\n", DNSRoute_MAX_WAN_CONN);
   		break;
	   }
	}
	num_wan = i;

	fclose(fp);

	FILE *fp2;
	char *token2=NULL;

	if((fp2 = fopen(STATIC_DNS_FILE_ROUTE_TABLE, "r")) == NULL)
	{
		cmsLog_error("could not open tmp file %s", STATIC_DNS_FILE_ROUTE_TABLE);
		return 0;
	}

	/* init DNSRoute */
	for(	i=0; i<DNSRoute_MAX_ENTRY; i++)
	{
		memset(DNSRoute[i].domainName, 0, sizeof(DNSRoute[i].domainName));
		memset(DNSRoute[i].ifcName, 0, sizeof(DNSRoute[i].ifcName));
		memset(DNSRoute[i].subnetMask, 0, sizeof(DNSRoute[i].subnetMask));
		memset(DNSRoute[i].type, 0, sizeof(DNSRoute[i].type));
	}

	int NTP_NUM = 0;  //record NTP server number
	i=0;
	while(fgets(line, sizeof(line), fp2)!=NULL && i<DNSRoute_MAX_ENTRY)
	{
		line[sizeof(line)-1]=0;

		memset(DNSRoute[i].domainName, 0, sizeof(DNSRoute[i].domainName));
		memset(DNSRoute[i].ifcName, 0, sizeof(DNSRoute[i].ifcName));
		memset(DNSRoute[i].subnetMask, 0, sizeof(DNSRoute[i].subnetMask));
		memset(DNSRoute[i].type, 0, sizeof(DNSRoute[i].type));
	
		token2 = strtok(line, ":");
		snprintf(DNSRoute[i].domainName, sizeof(DNSRoute[i].domainName), "%s", token2);	
		
		if((token2 = strtok(NULL, ":")) != NULL)
		{
			snprintf(DNSRoute[i].ifcName, sizeof(DNSRoute[i].ifcName),"%s", token2);			

			if((token2 = strtok(NULL, ":")) != NULL)
			{
				snprintf(DNSRoute[i].subnetMask, sizeof(DNSRoute[i].subnetMask), "%s", token2);
				
				if((token2 = strtok(NULL, ";")) != NULL)
				{
					snprintf(DNSRoute[i].type, sizeof(DNSRoute[i].type), "%s", token2);	
				}				
			}
		}
		memset(line, 0, 256);
		cmsLog_notice("DNSRoute[%d]:domainName[%s], ifcName[%s], subnetMask[%s] type[%s]",i, DNSRoute[i].domainName, DNSRoute[i].ifcName, DNSRoute[i].subnetMask,DNSRoute[i].type);
		
		/* check NTP Server 2 is need DNS route or not */
		if(!cmsUtl_strcmp(DNSRoute[i].type, "NTP")){
			NTP_NUM++;
			if(NTP_NUM == 2)
			{	
				if(isBBIUP == FALSE && isBBIv6UP == FALSE)
				{
					/* BBI interface is down, add NTP Server 2 to dnsRoute list */
					fprintf(stderr,"add this NTP Server 2\n\n");				
					i++;
				}
			}
			else
			{
				i++;
			}			
		}
		else
		{
			i++;		
		}	
	}
	num_entry = i;
	debug("%s %d : #entry[%d]\n",__FUNCTION__,__LINE__, num_entry);

	fclose(fp2);
	
	for(i=0;i<num_entry;i++)
	{
		for(k=0;k<num_wan;k++)
		{
			if( memcmp(DNSRoute[i].ifcName, dns_ifc[k], strlen(dns_ifc[k]))==0 )
			{			
				memset(DNSRoute[i].primary, 0, sizeof(DNSRoute[i].primary));
				memset(DNSRoute[i].secondary, 0, sizeof(DNSRoute[i].secondary));
				memset(DNSRoute[i].gateway, 0, sizeof(DNSRoute[i].gateway));
				
				memcpy(DNSRoute[i].primary, dns_primary[k], 16);
				memcpy(DNSRoute[i].secondary, dns_secondary[k], 16);
				memcpy(DNSRoute[i].gateway, gw[k], 16);

				debug("DNSRouteEntry[%d]:DNSRoute.domainName[%s], DNSRoute.ifcName[%s], DNSRoute.subnetMask[%s]\n", i, DNSRoute[i].domainName, DNSRoute[i].ifcName, DNSRoute[i].subnetMask);
				debug("DNSRouteEntry[%d]:DNSRoute.primary[%s], DNSRoute.secondary[%s], DNSRoute.gateway[%s]\n", i, DNSRoute[i].primary, DNSRoute[i].secondary, DNSRoute[i].gateway);
				
				break;
			}
		}
	}

	return 1;
}
#endif /* 1 */

/* Load name servers' addresses from /etc/resolv.conf */
static int load_brcm_name_servers(void)
{
	FILE * fp;
	char line[256];
	char addr[CMS_IPADDR_LENGTH];
	char domain[256];
#ifdef MSTC_COMMON_PATCH // __MTS__, Richard Huang
   char lan[CMS_IPADDR_LENGTH];
#endif

/* WenJuin@MSTC: DNS Route, 20130320 */ 
#if 1
	load_domainRoute_table();
#endif /* 1 */

	memset(primary_ns, 0, sizeof(primary_ns));
	memset(secondary_ns, 0, sizeof(secondary_ns));

    //name server of resolv.conf is 127.0.0.1, it redirects ont dns query
    //to dns proxy to record dns query failed of ipHost PM
/* WenJuin@MSTC: DNS Route, 20130322 */ 
#if 1
	if((fp = fopen(STATIC_DPROXY_DNS_FILE_DEFAULT, "r")) == NULL) 
	{
		debug("could not open file %s", STATIC_DPROXY_DNS_FILE_DEFAULT);
		return 0;
	}
#else /* 1 */    
	if((fp = fopen(REAL_DNS_FYI_FILENAME, "r")) == NULL) {
		debug("Opend %s failed", REAL_DNS_FYI_FILENAME);
        // if dns.real does not exist, open /etc/resolv.conf
    	if((fp = fopen("/etc/resolv.conf", "r")) == NULL) {
	    	debug("Opend /etc/resolv.conf failed");
		    return 0;
	    }
	}
#endif /* 1 */    

	while(fgets(line, sizeof(line), fp)) {
		if (sscanf(line, "nameserver %15s", addr) == 1) {
			if ( strcmp( addr, "0.0.0.0") && strcmp( addr, "127.0.0.1")) {
				if (primary_ns[0] == 0) {
					strncpy(primary_ns, addr, sizeof(primary_ns)-1);
				} else if (secondary_ns[0] == 0) {
					strncpy(secondary_ns, addr, sizeof(secondary_ns)-1);
				} else {
					printf("dnsproxy supports a max of two name servers.  "
							"Additional name servers are ignored.\n");
				}
			}
		} else if (sscanf(line, "domain %s", domain) == 1) {
			strncpy(config.domain_name, domain, sizeof(config.domain_name)-1);
		}
#ifdef MSTC_COMMON_PATCH // __MTS__, Richard Huang
      else if (sscanf(line, "LAN %s", lan) == 1) {
         strncpy(config.lan_ip, lan, sizeof(config.lan_ip) - 1);
      }
#endif
	}
	fclose(fp);

#ifdef SUPPORT_TELEFONICA /* 3G redirect page not works on domain name , __TELEFONICA__, MitraStar Curtis, 20120213 */
    struct ifreq ifreqs[32]; /* Maximum 32 network interfaces */
    struct ifconf ic;
    int num = 0;             /* interface number */
    int ancount = 0;         /* answer count */
    int fd, i;        
    
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        cmsLog_error("%s:%d create socket failed", __FUNCTION__, __LINE__);
        //die(_("failed to create socket: %s"), NULL, EC_BADNET);
    }
    
    ic.ifc_len = sizeof(ifreqs);
    ic.ifc_req = ifreqs;
    if (ioctl(fd, SIOCGIFCONF, &ic) == -1)
    {
        cmsLog_error("%s:%d SIOCGIFCONF failed", __FUNCTION__, __LINE__);
        //die(_("failed to get interface: %s"), NULL, EC_BADNET);
    }
    num = ic.ifc_len / sizeof(struct ifreq);
    for (i = 0; i < num; i++)
    {
        if (strncmp(ifreqs[i].ifr_name, "br0", strlen("br0")) == 0) /* looking for LAN IP */
        {
            strcpy(config.lan_ip , inet_ntoa(((struct sockaddr_in *)&ifreqs[i].ifr_addr)->sin_addr));
            ancount++;
        }
    }
    
    close(fd);
#endif

	debug("dproxy: load nameserver %s %s", primary_ns, secondary_ns);
	debug("dproxy: load domain %s", config.domain_name);

#ifdef SUPPORT_TELEFONICA_WEBSSO /*Redirect to Wizzard page for Brazil , __TELEFONICA__, MitraStar Walter, 20120322*/
	int pppStatus=0;
	char status_buf[1024] ={0};
	FILE *status_fp = NULL;

	system("ifconfig > /var/checkPPPStatus");
	status_fp = fopen("/var/checkPPPStatus", "r");
	if ( status_fp != NULL ) 
	{
		while (fgets(status_buf, 1024, status_fp) != NULL)
		{
			if (strncmp(status_buf, "ppp", 3) == 0){
				pppStatus = 1;
				break;
			}
			else{
				pppStatus = 0;
			}
		}
		// Close the file and remove it.
		fclose(status_fp);
		system("rm /var/checkPPPStatus");
	}
	if ((primary_ns[0] == 0) ||(pppStatus == 0))  /* not even a single valid nameserver found or PPP is down when a static DNS set*/
#else
	if (primary_ns[0] == 0)  /* not even a single valid nameserver found */
#endif
	{
		return 0;
	}
	else {
	return 1;
}
}

/* Initialization before probing */
int dns_probe_init(void)
{
	int ret;
	
	debug("dns_probe_init");

	/* Try to read name servers from /var/fyi/sys/dns */
	if ((ret = load_brcm_name_servers())) {
		strcpy(config.name_server, primary_ns);
	} else {
      /*
       * ret == 0 means wan is not up. 
       * BRCM: Use the magic ppp on demand address.
       */
      debug("wan is not up, set name_server to 128.9.0.107");
		strcpy(config.name_server, "128.9.0.107");
   }

	/* Set primary server as the probing address */
	memset(&probe_addr, 0, sizeof(probe_addr));
	probe_addr.sin_family = AF_INET;
	inet_aton(primary_ns, &probe_addr.sin_addr);
	probe_addr.sin_port = ntohs(PORT);

#ifdef DNS_PROBE
	/* Initialize request id */
	srandom(time(NULL));
	probe_id = (uint16_t)random();
	probe_next_time = 0;
	probe_tried = 0;
	probe_timeout = 0;
#endif
	return ret;
}

/* Send, resend probing request, check timeout, and switch name servers */
int dns_probe(void)
{
	int ret = 0;
	time_t now = time(NULL);

	debug("dns_probe: entered, dns_wanup=%d now=%d dns_recover_time=%d\n",
	      dns_wanup, now, dns_recover_time);

	if (!dns_wanup) { /* No need to probe if no WAN interface */
		return ret;
	}

#ifdef DNS_PROBE
	if (probe_tried) { /* Probing */
		if (now >= probe_timeout) { /* Timed out */
			if (probe_tried >= DNS_PROBE_MAX_TRY) {
				/* Probe failed */
				debug("dproxy: probing failed\n");
				if (secondary_ns[0] &&
				    strcmp(config.name_server, secondary_ns)) {
					printf("Primary DNS server Is Down... "
					       "Switching To Secondary DNS "
					       "server \n");
					strcpy(config.name_server,
					       secondary_ns);
				}
				probe_tried = 0;
				probe_next_time = now + DNS_PROBE_INTERVAL;
				ret = DNS_PROBE_INTERVAL;
			} else { /* Retry */
				sendto(dns_querysock, probe_pkt, sizeof(probe_pkt),
				       0, (struct sockaddr*)&probe_addr,
				       sizeof(probe_addr));
				probe_timeout = now + DNS_PROBE_TIMEOUT;
				probe_tried++;
				ret = DNS_PROBE_TIMEOUT;
			}
		} else {
			ret = probe_timeout - now;
		}
	} else if (now >= probe_next_time) { /* Time to probe */
		*((uint16_t*)probe_pkt) = htons(++probe_id);
		sendto(dns_querysock, probe_pkt, sizeof(probe_pkt), 0,
		       (struct sockaddr*)&probe_addr, sizeof(probe_addr));
		probe_tried = 1;
		probe_timeout = now + DNS_PROBE_TIMEOUT;
		ret = DNS_PROBE_TIMEOUT;
	} else {
		ret = probe_next_time - now;
	}
#else
	if (dns_recover_time) { /* Recovering */
		if (now >= dns_recover_time)
			dns_probe_switchback();
		else
			ret = dns_recover_time - now;
	}

#endif

	debug("dns_probe: done, ret=%d\n", ret);

	return ret;
}

#ifndef DNS_PROBE
int dns_probe_is_primary_up(void)
{
	if (dns_recover_time == 0)
		return 1;
	else
		return 0;
}

void dns_probe_set_recover_time(void)
{

   if (dns_recover_time) {
      /* We are currently using the secondary nameserver.  Do nothing */
      debug("still using secondary_ns, dns_recover_time=%d\n",
            dns_recover_time);
      return;
   }

   /* Only switch to the 2nd dns server when it is configured */
   if (secondary_ns[0]) {
     char date_time_buf[DATE_TIME_BUFLEN];
     get_date_time_str(date_time_buf, sizeof(date_time_buf));

     printf("%s Primary DNS server (%s) Is Down..."
          "Switching To Secondary DNS server (%s) "
          "for %d seconds.  \n",
          date_time_buf, primary_ns, secondary_ns, DNS_RECOVER_TIMEOUT);

     strncpy(config.name_server, secondary_ns, sizeof(config.name_server)-1);
     dns_recover_time = time(NULL) + DNS_RECOVER_TIMEOUT;
     dns_list_unarm_all_requests();
   }
   else
   {
      debug("no valid secondary_ns to switch to.\n");
   }
}
#endif

/* Switch back to primary server */
void dns_probe_switchback(void)
{
	char date_time_buf[DATE_TIME_BUFLEN];

#ifdef DNS_PROBE
	static int first_time = 1;
#else
	dns_recover_time = 0;
#endif

	debug("config.name_server=%s primary_ns=%s",
	      config.name_server, primary_ns);

	if (!strcmp(config.name_server, primary_ns))
	{
		debug("dns_probe_switchback called even though primary is already up!\n");
		return;
	}

#ifdef DNS_PROBE
	if (first_time)
		first_time = 0;
	else
#endif

	get_date_time_str(date_time_buf, sizeof(date_time_buf));
	printf("%s Switching back to primary dns server (%s).\n",
	       date_time_buf, primary_ns);

	strncpy(config.name_server, primary_ns, sizeof(config.name_server)-1);
	dns_list_unarm_all_requests();
}

/* Activate primary server */
int dns_probe_activate(uint32_t name_server)
{
	debug("name_server=0x%x probe_addr=0x%x\n", name_server, probe_addr.sin_addr.s_addr);

	if (name_server != probe_addr.sin_addr.s_addr)
		return 0;

#ifdef DNS_PROBE
	probe_tried = 0;
	probe_next_time = time(NULL) + DNS_PROBE_INTERVAL;
#endif

	dns_probe_switchback();
	debug("probe_activate: done\n");
	return 1;
}

/* Activate name server if it's the response for probing request */
int dns_probe_response(dns_request_t *m)
{
#ifdef DNS_PROBE
	if (m->message.header.flags.flags & 0x8000 &&
	    m->message.header.id != probe_id)
		return 0;
	return dns_probe_activate(m->src_addr.s_addr);
#else
	return 0;
#endif
}

#ifdef SUPPORT_DEBUG_TOOLS
void dns_probe_print(void)
{
   printf("primary_ns=%s secondary_ns=%s\n", primary_ns, secondary_ns);
#ifndef DNS_PROBE
   printf("dns_recover_time=%d\n", (int) dns_recover_time);
#endif
}
#endif  /* SUPPORT_DEBUG_TOOLS */
