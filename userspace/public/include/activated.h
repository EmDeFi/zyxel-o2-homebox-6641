#include <stdio.h>   /* for snprintf */
#include <stdint.h>  /* for the various integer types */
#include <stdlib.h>  /* for NULL */
#include <string.h>  /* for strlen, strncpy */

#if 1
int setActivatedStatus(int SKey, int setVal);
int getActivatedStatus(int SKey);
int setActivatedInfo(int SKey, char *param);
int getActivatedInfo(int SKey, char *buff);
void readData(FILE *fp, char *buff);
int writeData(FILE *fp, char *input);
#endif
#if 1
#define PPP1IFNAME	"ppp0.1"
#define PPP2IFNAME	"ppp0.2"
#define PPP3IFNAME	"ppp0.3"
#define PPP4IFNAME	"ppp0.4"

#define ADSL_SMODE_HANSENETULL	"HANSENET ULL"
#define ADSL_SMODE_WHOLESALETK		"Wholesale/TK-Based"
#define ADSL_SMODE_WHOLESALEPIN	"Wholesale/PIN-Based"
#define ADSL_SMODE_BITSTREAM		"ADSL Bitstream"

#define ETH_SMODE_ETHER			"Hansenet Ethernet"
#define ETH_SMODE_WHOLESALETK		"Wholesale Ethernet/TK-Based"
#define ETH_SMODE_WHOLESALEPIN		"Wholesale Ethernet/PIN-Based"
#define ETH_SMODE_BITSTREAM			"Ethernet Bitstream Access"

#define VDSL_SMODE_HANSENETULL	"HANSENET FTTX"
#define VDSL_SMODE_WHOLESALETK		"Wholesale FTTX/TK-Based"
#define VDSL_SMODE_WHOLESALEPIN	"Wholesale FTTX/PIN-Based"
#define VDSL_SMODE_BITSTREAM		"VDSL2 Bitstream Access"

#define BBICONNMODE_ALWAYSON		"AlwaysOn"
#define BBICONNMODE_ONDEMAND		"OnDemand"
#define BBICONNMODE_MANUAL			"Manual"
#define SMODE_UNKNOWN				"Unknown"

#define AC_HANSENET_XDSL			"HANSENET-XDSL"
#define AC_HANSENET_ETHERNET		"HANSENET-ETHERNET"
#define AC_TK_ENABLED				"TK-ENABLED"



#define	MAXPPPRETRY	4

#define SERTYPE_VOIP	"VoIP"
#define SERTYPE_BBI		"BBI"
#define EMPTY_STR		""

#define ZERO_STR		0
#define FALSE_STR		0
#define TRUE_STR		1

#define ETH_DISCOVERY		"ETH"
#define ADSL_DISCOVERY		"ADSL"	/*for Annex B*/
#define ADSL_J_DISCOVERY	"ADSL_J"	/*for Annex J*/
#define VDSL_DISCOVERY		"VDSL"

/*For TO2 XDSL Delay mechanism, __TO2__, MSTC Memphis Chung, 20130428*/
#define AnnexB_DELAY_TIME	10000
#define AnnexJ_DELAY_TIME	13000
#define VDSL_DELAY_TIME	5000
/* chris */
#define FILEPATH		"/var/ppp/"

typedef enum
{
	PHY_DETECT_STAGE = 1,                                         // Physical Layer Detection
   	SERVICE_MODE_DETECT_STAGE,                        // Service Model Detection
   	PPP_ACTIVATION_SESS_STAGE,                             // PPP Activation Session
   	PPP_ACTIVATION_ACTIVATED                        // RG Activated
} ACTIVATION_STAGE;

typedef enum
{
	ACTIVATED_STATUS_001 = 1,
	ACTIVATED_STATUS_002,
	ACTIVATED_STATUS_003,
	ACTIVATED_STATUS_004,
	ACTIVATED_STATUS_005,
	ACTIVATED_STATUS_006,
	ACTIVATED_STATUS_007,
	ACTIVATED_STATUS_008,
	ACTIVATED_STATUS_009	
}ACTIVATED_STATUS;

typedef enum
{
/*Used for creating record file, status part*/
ACTIVATED_STATUS_STAGE = 1,
ACTIVATED_STATUS_STATUS,
OPEN_ACCESS,
ACTIVATION_ACTIVATED,
ACTIVATION_PINCHECKED,
ACTIVATION_ISDUAL,
ACTIVATION_FIRSTBOOTUP,
ACTIVATION_ON_DEMAND_TIME,
ACTIVATION_PPPRETRYNUM,
ACTIVATION_ISPPPRETRY,
BLOCKING_EVENTS,
ACTIVATION_IS_BBI_ACTIVATED,
ACTIVATION_IS_BBI_CONNECTED,
ACTIVATION_IS_BBI_AUTH_FAIL,
ACTIVATION_IS_VOIP_CONNECTED,
PPP1_AUTH_FAIL_RETRY_INDEX,
PPP2_AUTH_FAIL_RETRY_INDEX,
DDNS_REGISTER_WAN_UP_INTERFACE,		/*[IreneWang@MSTC:TO2]$ For record ddns correct interface, default value is 0 means register all wan up, the file will update in inadyn process, 20130917*/
/*Info part*/
ACTIVATION_ETH_VID,
ACTIVATION_VDSL_VID,
ACTIVATION_ADSL_PVC,
ACTIVATION_USERNAME_VOIP,
ACTIVATION_PASSWORD_VOIP,
ACTIVATION_BBICONNMODE,
ACTIVATION_USERNAME_BBI,
ACTIVATION_PASSWORD_BBI,
ACTIVATION_RETRYIFNAME,
ACTIVATION_PIN,
ACTIVATION_L2_DISCOVER_STATUS,
ACTIVATION_SERVICEMODE,
FILE_LIST_FINAL
}FILE_LIST;

#endif
