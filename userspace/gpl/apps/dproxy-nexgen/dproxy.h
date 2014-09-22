#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef DPROXY_H
#define DPROXY_H

#define PORT 53
#define PORT_STR "53"
#define NAME_SIZE 255
#define MAX_PACKET_SIZE 512
#define BUF_SIZE NAME_SIZE

typedef u_int16_t uint16;
typedef u_int32_t uint32;

#include "dns.h"

#ifndef DNS_TIMEOUT 
#define DNS_TIMEOUT 240
#endif

#define MAX_RETX_COUNT  100

#ifndef NAME_SERVER_DEFAULT
#define NAME_SERVER_DEFAULT "0.0.0.0"
#endif
#ifndef CONFIG_FILE_DEFAULT 
#define CONFIG_FILE_DEFAULT "/etc/dproxy.conf"
#endif
#ifndef DENY_FILE_DEFAULT 
#define DENY_FILE_DEFAULT "/var/cache/dproxy.deny"
#endif
#ifndef CACHE_FILE_DEFAULT 
#define CACHE_FILE_DEFAULT "/var/cache/dproxy.cache"
#endif
#ifndef HOSTS_FILE_DEFAULT 
//BRCM
//#define HOSTS_FILE_DEFAULT "/etc/hosts"
#define HOSTS_FILE_DEFAULT "/var/hosts"
#endif
#ifndef PURGE_TIME_DEFAULT 
#define PURGE_TIME_DEFAULT 48 * 60 * 60
#endif
#ifndef PPP_DEV_DEFAULT 
#define PPP_DEV_DEFAULT "/var/run/ppp0.pid"
#endif
#ifndef DHCP_LEASES_DEFAULT 
//BRCM
//#define DHCP_LEASES_DEFAULT "/var/state/dhcp/dhcpd.leases"
#define DHCP_LEASES_DEFAULT "/var/udhcpd/udhcpd.leases"
#endif
#ifndef PPP_DETECT_DEFAULT 
#define PPP_DETECT_DEFAULT 1
#endif
#ifndef DEBUG_FILE_DEFAULT 
#define DEBUG_FILE_DEFAULT "/var/log/dproxy.debug.log"
#endif

#ifndef DOMAIN_NAME_DEFAULT
#define DOMAIN_NAME_DEFAULT "Home"
#endif

//BRCM
#define STATIC_DNS_FILE_DEFAULT "/var/fyi/sys/dns"
/* WenJuin@MSTC: DNS Route, 20130320 */ 
#ifdef MSTC_TO2_GERMANY_CUSTOMIZATION
#define STATIC_DPROXY_DNS_FILE_DEFAULT "/var/fyi/sys/dns"
#define STATIC_DNS_FILE_WAN "/var/fyi/sys/wanDnsInfo"
#define STATIC_DNS_FILE_ROUTE_TABLE "/var/fyi/sys/dnsRouteTable"
#endif /* MSTC_TO2_GERMANY_CUSTOMIZATION */


#if 0
#ifdef DPROXY_DEBUG
void debug_perror( char *msg );
void debug(char *fmt, ...);
#else
#define debug_perror(msg) 
#define debug(fmt,arg...)
#endif
#endif
#include "cms.h"
#include "cms_eid.h"
#include "cms_util.h"
#include "cms_msg.h"
void debug_perror( char *msg );
#define debug cmsLog_debug

extern void get_date_time_str(char *buf, unsigned int buflen);
#define DATE_TIME_BUFLEN    256

#define PRIMARY_SERVER      1
#define SECONDARY_SERVER    2
#define PURGE_TIMEOUT       30

/* WenJuin@MSTC: DNS Route, 20130319 */ 
#ifdef MSTC_TO2_GERMANY_CUSTOMIZATION
#define DNSRoute_MAX_ENTRY  10
#define DNSRoute_MAX_WAN_CONN 5

#define VOIP_RETRY_MAX			10
#define BBI_RETRY_MAX			10
#define VOIP_SINGLE_RETYR_MAX 	2
#define BBI_SINGLE_RETYR_MAX 	2

/* WenJuin@MSTC: DNS query order, 20130429 */ 
typedef enum
{
   NOT_RESET	= 0,
   RESET_VOIP	= 1,
   RESET_BBI		= 2,
   RESET_ALL		= 3,
} Reset_Type;

/* WenJuin@MSTC: DNS query order, 20130429 */ 
typedef enum
{
   DNSv6_1	= 0,
   DNSv6_2	= 1,
   DNSv4_1	= 2,
   DNSv4_2	= 3,
} DnsOrder;

typedef struct DNSRoute_Entry_s{
	char domainName[128];
	char subnetMask[16];
	char ifcName[8];
	char gateway[16];
	char primary[16];
	char secondary[16];
	char type[16];	
}DNSRoute_Entry_t;

void Reset_list_count(Reset_Type action);
DnsOrder isChangeVoipDnsOrder(dns_request_t *retryEntry);
DnsOrder isChangeBBIDnsOrder(dns_request_t *retryEntry);
UBOOL8 VoipStatusChange(UBOOL8 isDual ,UBOOL8 status, UBOOL8 v6Status);
UBOOL8 BBIStatusChange(UBOOL8 isDual ,UBOOL8 status, UBOOL8 v6Status);
void Init_wanStatus(UINT32 status,int *InterfaceStatus);
UBOOL8 isDualMode(void);
int lookup_domain_name_table(dns_request_t *m);
UBOOL8 isSpecialDomain(char *domain, char *type);
int rut_translateSubnetMask(char *strSubnetMask);
void addDnsRoute(const char* ip, const char* cname);
UBOOL8 DNS_getArpEntryMAC(char *queryip, char* macaddr);
#endif /* MSTC_TO2_GERMANY_CUSTOMIZATION */

#endif
