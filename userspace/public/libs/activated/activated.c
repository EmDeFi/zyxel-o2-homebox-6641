#include "activated.h"

#if 1
int setActivatedStatus(int SKey, int setVal)
{
	int key = SKey;
	char param[256] = {'\0'};
	snprintf( param, 256, "%d", setVal);
	if(SKey < ACTIVATED_STATUS_STAGE || SKey > FILE_LIST_FINAL)
	{		
		perror("Invalid key");
		return -1;
	}

	FILE *fp=NULL;	
	char semFile[256]={0}; /* kycheng: check buffer size */	
	
	switch(key)
	{
		case ACTIVATED_STATUS_STAGE:
		sprintf(semFile, "%s%s",FILEPATH, "Activated_Stage");
		break;
		case ACTIVATED_STATUS_STATUS:
		sprintf(semFile, "%s%s",FILEPATH, "Activated_Status");
		break;
		case OPEN_ACCESS:
		sprintf(semFile, "%s%s",FILEPATH, "Open_access");
		break;
		case ACTIVATION_L2_DISCOVER_STATUS:
		sprintf(semFile, "%s%s",FILEPATH, "L2_DISCOVER_STATUS");
		break;
		case ACTIVATION_ETH_VID:
		sprintf(semFile, "%s%s",FILEPATH, "ETH_VID");
		break;
		case ACTIVATION_VDSL_VID:
		sprintf(semFile, "%s%s",FILEPATH, "VDSL_VID");
		break;
		case ACTIVATION_ADSL_PVC:
		sprintf(semFile, "%s%s",FILEPATH, "ADSL_PVC");
		break;
		case ACTIVATION_SERVICEMODE:
		sprintf(semFile, "%s%s",FILEPATH, "ServiceMode");
		break;
		case ACTIVATION_ACTIVATED:
		sprintf(semFile, "%s%s",FILEPATH, "Activated");
		break;
		case ACTIVATION_PIN:
		sprintf(semFile, "%s%s",FILEPATH, "PIN");
		break;
		case ACTIVATION_PINCHECKED:
		sprintf(semFile, "%s%s",FILEPATH, "PINCheck");
		break;
		case ACTIVATION_ISDUAL:
		sprintf(semFile, "%s%s",FILEPATH, "isDual");
		break;
		case ACTIVATION_FIRSTBOOTUP:
		sprintf(semFile, "%s%s",FILEPATH, "firstBootUP");
		break;
		case ACTIVATION_USERNAME_VOIP:
		sprintf(semFile, "%s%s",FILEPATH, "UserName_VoIP");
		break; 
		case ACTIVATION_PASSWORD_VOIP:
		sprintf(semFile, "%s%s",FILEPATH, "Password_VoIP");
		break; 
		case ACTIVATION_BBICONNMODE:
		sprintf(semFile, "%s%s",FILEPATH, "BBIConnMode");
		break; 
		case ACTIVATION_USERNAME_BBI:
		sprintf(semFile, "%s%s",FILEPATH, "UserName_BBI");
		break; 
		case ACTIVATION_PASSWORD_BBI:
		sprintf(semFile, "%s%s",FILEPATH, "Password_BBI");
		break; 
		case ACTIVATION_ON_DEMAND_TIME:
		sprintf(semFile, "%s%s",FILEPATH, "ON_DEMAND_TIME");
		break;
		case ACTIVATION_PPPRETRYNUM: 
		sprintf(semFile, "%s%s",FILEPATH, "PPP_RetryNum");
		break;
		case ACTIVATION_ISPPPRETRY: 
		sprintf(semFile, "%s%s",FILEPATH, "PPP_isRetry");
		break;
		case ACTIVATION_RETRYIFNAME: 
		sprintf(semFile, "%s%s",FILEPATH, "retryIfName");
		break;
		case BLOCKING_EVENTS: /*chris 20130325  for blocking events*/
		sprintf(semFile, "%s%s",FILEPATH, "BlockingEvents");
		break;
		case ACTIVATION_IS_BBI_ACTIVATED: /*chris 20130403  for BBI dial-on demand*/
		sprintf(semFile, "%s%s",FILEPATH, "isBBIActivated");
		break;
		case ACTIVATION_IS_BBI_CONNECTED: /*chinglung 20130527  display the BBI connection*/
		sprintf(semFile, "%s%s",FILEPATH, "isBBIConnected");
		break;	
		case ACTIVATION_IS_BBI_AUTH_FAIL: /*chinglung 20130606  display the BBI connection error message*/
		sprintf(semFile, "%s%s",FILEPATH, "isBBIAuthFail");
		break;	
		case ACTIVATION_IS_VOIP_CONNECTED: /*chinglung 20130606  display the VoIP connection*/
		sprintf(semFile, "%s%s",FILEPATH, "isVoIPConnected");
		break;	
		case PPP1_AUTH_FAIL_RETRY_INDEX: /*chris 20130723 for ppp0.1 authentication fail retry*/
		sprintf(semFile, "%s%s",FILEPATH, "PPP1_AuthFailRetry_Index");
		break;
		case PPP2_AUTH_FAIL_RETRY_INDEX: /*chris 20130723 for ppp0.2 authentication fail retry*/
		sprintf(semFile, "%s%s",FILEPATH, "PPP2_AuthFailRetry_Index");
		break;
		case DDNS_REGISTER_WAN_UP_INTERFACE:	/*[IreneWang@MSTC:TO2]$ For record ddns correct interface, default value is 0 means register all wan up, the file will update in inadyn process, 20130917*/
		sprintf(semFile, "%s%s",FILEPATH, "DDNS_WAN_UP_Interface");
		break;
		default:
		    printf("Incorrect semaphore key!");
		break;
	}

	fp = fopen(semFile, "w");
	writeData(fp, param);
	//printf("======>setActivatedStatus: %s=%s\n", semFile, param);
	return 0;
}

int getActivatedStatus(int SKey)
{	
	int key = SKey;
	/*check the SKey is available or not*/
	if(SKey < ACTIVATED_STATUS_STAGE || SKey > FILE_LIST_FINAL)
	{				
		perror("Invalid key");
		return -1;
	}

	FILE *fp=NULL;	
	char semFile[256]={0}; /* kycheng: check buffer size */	
	char buff[256]={0};
	int returnVal = 0;
	
	switch(key)
	{
		case ACTIVATED_STATUS_STAGE:
		sprintf(semFile, "%s%s",FILEPATH, "Activated_Stage");
		break;
		case ACTIVATED_STATUS_STATUS:
		sprintf(semFile, "%s%s",FILEPATH, "Activated_Status");
		break;
		case OPEN_ACCESS:
		sprintf(semFile, "%s%s",FILEPATH, "Open_access");
		break;
		case ACTIVATION_L2_DISCOVER_STATUS:
		sprintf(semFile, "%s%s",FILEPATH, "L2_DISCOVER_STATUS");
		break;
		case ACTIVATION_ETH_VID:
		sprintf(semFile, "%s%s",FILEPATH, "ETH_VID");
		break;
		case ACTIVATION_VDSL_VID:
		sprintf(semFile, "%s%s",FILEPATH, "VDSL_VID");
		break;
		case ACTIVATION_ADSL_PVC:
		sprintf(semFile, "%s%s",FILEPATH, "ADSL_PVC");
		break;
		case ACTIVATION_SERVICEMODE:
		sprintf(semFile, "%s%s",FILEPATH, "ServiceMode");
		break;
		case ACTIVATION_ACTIVATED:
		sprintf(semFile, "%s%s",FILEPATH, "Activated");
		break;
		case ACTIVATION_PIN:
		sprintf(semFile, "%s%s",FILEPATH, "PIN");
		break;
		case ACTIVATION_PINCHECKED:
		sprintf(semFile, "%s%s",FILEPATH, "PINCheck");
		break;
		case ACTIVATION_ISDUAL:
		sprintf(semFile, "%s%s",FILEPATH, "isDual");
		break;
		case ACTIVATION_FIRSTBOOTUP:
		sprintf(semFile, "%s%s",FILEPATH, "firstBootUP");
		break;
		case ACTIVATION_USERNAME_VOIP:
		sprintf(semFile, "%s%s",FILEPATH, "UserName_VoIP");
		break; 
		case ACTIVATION_PASSWORD_VOIP:
		sprintf(semFile, "%s%s",FILEPATH, "Password_VoIP");
		break; 
		case ACTIVATION_BBICONNMODE:
		sprintf(semFile, "%s%s",FILEPATH, "BBIConnMode");
		break; 
		case ACTIVATION_USERNAME_BBI:
		sprintf(semFile, "%s%s",FILEPATH, "UserName_BBI");
		break; 
		case ACTIVATION_PASSWORD_BBI:
		sprintf(semFile, "%s%s",FILEPATH, "Password_BBI");
		break; 
		case ACTIVATION_ON_DEMAND_TIME:
		sprintf(semFile, "%s%s",FILEPATH, "ON_DEMAND_TIME");
		break;
		case ACTIVATION_PPPRETRYNUM: 
		sprintf(semFile, "%s%s",FILEPATH, "PPP_RetryNum");
		break;
		case ACTIVATION_ISPPPRETRY: 
		sprintf(semFile, "%s%s",FILEPATH, "PPP_isRetry");
		break;
		case ACTIVATION_RETRYIFNAME: 
		sprintf(semFile, "%s%s",FILEPATH, "retryIfName");
		break;
		case BLOCKING_EVENTS: /*chris 20130325  for blocking events*/
		sprintf(semFile, "%s%s",FILEPATH, "BlockingEvents");
		break;
		case ACTIVATION_IS_BBI_ACTIVATED: /*chris 20130403  for BBI dial-on demand*/
		sprintf(semFile, "%s%s",FILEPATH, "isBBIActivated");
		break;
		case ACTIVATION_IS_BBI_CONNECTED: /*chinglung 20130527  display the BBI connection*/
		sprintf(semFile, "%s%s",FILEPATH, "isBBIConnected");
		break;	
		case ACTIVATION_IS_BBI_AUTH_FAIL: /*chinglung 20130606  display the BBI connection error message*/
		sprintf(semFile, "%s%s",FILEPATH, "isBBIAuthFail");
		break;	
		case ACTIVATION_IS_VOIP_CONNECTED: /*chinglung 20130606  display the VoIP connection*/
		sprintf(semFile, "%s%s",FILEPATH, "isVoIPConnected");
		break;	
		case PPP1_AUTH_FAIL_RETRY_INDEX: /*chris 20130723 for ppp0.1 authentication fail retry*/
		sprintf(semFile, "%s%s",FILEPATH, "PPP1_AuthFailRetry_Index");
		break;
		case PPP2_AUTH_FAIL_RETRY_INDEX: /*chris 20130723 for ppp0.2 authentication fail retry*/
		sprintf(semFile, "%s%s",FILEPATH, "PPP2_AuthFailRetry_Index");
		break;
		case DDNS_REGISTER_WAN_UP_INTERFACE:	/*[IreneWang@MSTC:TO2]$ For record ddns correct interface, default value is 0 means register all wan up, the file will update in inadyn process, 20130917*/
		sprintf(semFile, "%s%s",FILEPATH, "DDNS_WAN_UP_Interface");
		break;
		default:
		    printf("Incorrect semaphore key!");
		break;
	}

	if((fp = fopen(semFile, "r")) == NULL)
	{			
		/*printf("Error to read the data file , getKey = [%d]\n",getKey);	*/ /* ChenHe remark for MFG */
		return -1;
	}
	readData(fp, buff);
	returnVal = atoi(buff);	
	//printf("======>getActivatedStatus: %s=%d\n", semFile, returnVal);
	return returnVal;
}

int setActivatedInfo(int SKey, char *param)
{
	int key = SKey;
	if(SKey < ACTIVATED_STATUS_STAGE || SKey > FILE_LIST_FINAL)
	{		
		perror("Invalid key");
		return -1;
	}

	FILE *fp=NULL;
	char semFile[256]={0}; /* kycheng: check buffer size */	

	switch(key)
	{
		case ACTIVATED_STATUS_STAGE:
		sprintf(semFile, "%s%s",FILEPATH, "Activated_Stage");
		break;
		case ACTIVATED_STATUS_STATUS:
		sprintf(semFile, "%s%s",FILEPATH, "Activated_Status");
		break;
		case OPEN_ACCESS:
		sprintf(semFile, "%s%s",FILEPATH, "Open_access");
		break;
		case ACTIVATION_L2_DISCOVER_STATUS:
		sprintf(semFile, "%s%s",FILEPATH, "L2_DISCOVER_STATUS");
		break;
		case ACTIVATION_ETH_VID:
		sprintf(semFile, "%s%s",FILEPATH, "ETH_VID");
		break;
		case ACTIVATION_VDSL_VID:
		sprintf(semFile, "%s%s",FILEPATH, "VDSL_VID");
		break;
		case ACTIVATION_ADSL_PVC:
		sprintf(semFile, "%s%s",FILEPATH, "ADSL_PVC");
		break;
		case ACTIVATION_SERVICEMODE:
		sprintf(semFile, "%s%s",FILEPATH, "ServiceMode");
		break;
		case ACTIVATION_ACTIVATED:
		sprintf(semFile, "%s%s",FILEPATH, "Activated");
		break;
		case ACTIVATION_PIN:
		sprintf(semFile, "%s%s",FILEPATH, "PIN");
		break;
		case ACTIVATION_PINCHECKED:
		sprintf(semFile, "%s%s",FILEPATH, "PINCheck");
		break;
		case ACTIVATION_ISDUAL:
		sprintf(semFile, "%s%s",FILEPATH, "isDual");
		break;
		case ACTIVATION_FIRSTBOOTUP:
		sprintf(semFile, "%s%s",FILEPATH, "firstBootUP");
		break;
		case ACTIVATION_USERNAME_VOIP:
		sprintf(semFile, "%s%s",FILEPATH, "UserName_VoIP");
		break; 
		case ACTIVATION_PASSWORD_VOIP:
		sprintf(semFile, "%s%s",FILEPATH, "Password_VoIP");
		break; 
		case ACTIVATION_BBICONNMODE:
		sprintf(semFile, "%s%s",FILEPATH, "BBIConnMode");
		break; 
		case ACTIVATION_USERNAME_BBI:
		sprintf(semFile, "%s%s",FILEPATH, "UserName_BBI");
		break; 
		case ACTIVATION_PASSWORD_BBI:
		sprintf(semFile, "%s%s",FILEPATH, "Password_BBI");
		break; 
		case ACTIVATION_ON_DEMAND_TIME:
		sprintf(semFile, "%s%s",FILEPATH, "ON_DEMAND_TIME");
		break;
		case ACTIVATION_PPPRETRYNUM: 
		sprintf(semFile, "%s%s",FILEPATH, "PPP_RetryNum");
		break;
		case ACTIVATION_ISPPPRETRY: 
		sprintf(semFile, "%s%s",FILEPATH, "PPP_isRetry");
		break;
		case ACTIVATION_RETRYIFNAME: 
		sprintf(semFile, "%s%s",FILEPATH, "retryIfName");
		break;
		case BLOCKING_EVENTS: /*chris 20130325  for blocking events*/
		sprintf(semFile, "%s%s",FILEPATH, "BlockingEvents");
		break;
		case ACTIVATION_IS_BBI_ACTIVATED: /*chris 20130403  for BBI dial-on demand*/
		sprintf(semFile, "%s%s",FILEPATH, "isBBIActivated");
		break;
		case ACTIVATION_IS_BBI_CONNECTED: /*chinglung 20130527  display the BBI connection*/
		sprintf(semFile, "%s%s",FILEPATH, "isBBIConnected");
		break;	
		case ACTIVATION_IS_BBI_AUTH_FAIL: /*chinglung 20130606  display the BBI connection error message*/
		sprintf(semFile, "%s%s",FILEPATH, "isBBIAuthFail");
		break;	
		case ACTIVATION_IS_VOIP_CONNECTED: /*chinglung 20130606  display the VoIP connection*/
		sprintf(semFile, "%s%s",FILEPATH, "isVoIPConnected");
		break;	
		case PPP1_AUTH_FAIL_RETRY_INDEX: /*chris 20130723 for ppp0.1 authentication fail retry*/
		sprintf(semFile, "%s%s",FILEPATH, "PPP1_AuthFailRetry_Index");
		break;
		case PPP2_AUTH_FAIL_RETRY_INDEX: /*chris 20130723 for ppp0.2 authentication fail retry*/
		sprintf(semFile, "%s%s",FILEPATH, "PPP2_AuthFailRetry_Index");
		break;
		case DDNS_REGISTER_WAN_UP_INTERFACE:	/*[IreneWang@MSTC:TO2]$ For record ddns correct interface, default value is 0 means register all wan up, the file will update in inadyn process, 20130917*/
		sprintf(semFile, "%s%s",FILEPATH, "DDNS_WAN_UP_Interface");
		break;
		default:
		    printf("Incorrect semaphore key!");
		break;
	}

	fp = fopen(semFile, "w");
	writeData(fp, param);
	//printf("======>setActivatedInfo: %s=%s\n", semFile, param);	
	return 0;
}

int getActivatedInfo(int SKey, char *buff)
{	
	int key = SKey;
	/*check the SKey is available or not*/
	if(SKey < ACTIVATED_STATUS_STAGE || SKey > FILE_LIST_FINAL)
	{				
		perror("Invalid key");
		return -1;
	}

	FILE *fp=NULL;	
	char semFile[256]={0}; /* kycheng: check buffer size */
	
	switch(key)
	{
		case ACTIVATED_STATUS_STAGE:
		sprintf(semFile, "%s%s",FILEPATH, "Activated_Stage");
		break;
		case ACTIVATED_STATUS_STATUS:
		sprintf(semFile, "%s%s",FILEPATH, "Activated_Status");
		break;
		case OPEN_ACCESS:
		sprintf(semFile, "%s%s",FILEPATH, "Open_access");
		break;
		case ACTIVATION_L2_DISCOVER_STATUS:
		sprintf(semFile, "%s%s",FILEPATH, "L2_DISCOVER_STATUS");
		break;
		case ACTIVATION_ETH_VID:
		sprintf(semFile, "%s%s",FILEPATH, "ETH_VID");
		break;
		case ACTIVATION_VDSL_VID:
		sprintf(semFile, "%s%s",FILEPATH, "VDSL_VID");
		break;
		case ACTIVATION_ADSL_PVC:
		sprintf(semFile, "%s%s",FILEPATH, "ADSL_PVC");
		break;
		case ACTIVATION_SERVICEMODE:
		sprintf(semFile, "%s%s",FILEPATH, "ServiceMode");
		break;
		case ACTIVATION_ACTIVATED:
		sprintf(semFile, "%s%s",FILEPATH, "Activated");
		break;
		case ACTIVATION_PIN:
		sprintf(semFile, "%s%s",FILEPATH, "PIN");
		break;
		case ACTIVATION_PINCHECKED:
		sprintf(semFile, "%s%s",FILEPATH, "PINCheck");
		break;
		case ACTIVATION_ISDUAL:
		sprintf(semFile, "%s%s",FILEPATH, "isDual");
		break;
		case ACTIVATION_FIRSTBOOTUP:
		sprintf(semFile, "%s%s",FILEPATH, "firstBootUP");
		break;
		case ACTIVATION_USERNAME_VOIP:
		sprintf(semFile, "%s%s",FILEPATH, "UserName_VoIP");
		break; 
		case ACTIVATION_PASSWORD_VOIP:
		sprintf(semFile, "%s%s",FILEPATH, "Password_VoIP");
		break; 
		case ACTIVATION_BBICONNMODE:
		sprintf(semFile, "%s%s",FILEPATH, "BBIConnMode");
		break; 
		case ACTIVATION_USERNAME_BBI:
		sprintf(semFile, "%s%s",FILEPATH, "UserName_BBI");
		break; 
		case ACTIVATION_PASSWORD_BBI:
		sprintf(semFile, "%s%s",FILEPATH, "Password_BBI");
		break; 
		case ACTIVATION_ON_DEMAND_TIME:
		sprintf(semFile, "%s%s",FILEPATH, "ON_DEMAND_TIME");
		break;
		case ACTIVATION_PPPRETRYNUM: 
		sprintf(semFile, "%s%s",FILEPATH, "PPP_RetryNum");
		break;
		case ACTIVATION_ISPPPRETRY: 
		sprintf(semFile, "%s%s",FILEPATH, "PPP_isRetry");
		break;
		case ACTIVATION_RETRYIFNAME: 
		sprintf(semFile, "%s%s",FILEPATH, "retryIfName");
		break;
		case BLOCKING_EVENTS: /*chris 20130325  for blocking events*/
		sprintf(semFile, "%s%s",FILEPATH, "BlockingEvents");
		break;
		case ACTIVATION_IS_BBI_ACTIVATED: /*chris 20130403  for BBI dial-on demand*/
		sprintf(semFile, "%s%s",FILEPATH, "isBBIActivated");
		break;
		case ACTIVATION_IS_BBI_CONNECTED: /*chinglung 20130527  display the BBI connection*/
		sprintf(semFile, "%s%s",FILEPATH, "isBBIConnected");
		break;	
		case ACTIVATION_IS_BBI_AUTH_FAIL: /*chinglung 20130606  display the BBI connection error message*/
		sprintf(semFile, "%s%s",FILEPATH, "isBBIAuthFail");
		break;	
		case ACTIVATION_IS_VOIP_CONNECTED: /*chinglung 20130606  display the VoIP connection*/
		sprintf(semFile, "%s%s",FILEPATH, "isVoIPConnected");
		break;	
		case PPP1_AUTH_FAIL_RETRY_INDEX: /*chris 20130723 for ppp0.1 authentication fail retry*/
		sprintf(semFile, "%s%s",FILEPATH, "PPP1_AuthFailRetry_Index");
		break;
		case PPP2_AUTH_FAIL_RETRY_INDEX: /*chris 20130723 for ppp0.2 authentication fail retry*/
		sprintf(semFile, "%s%s",FILEPATH, "PPP2_AuthFailRetry_Index");
		break;
		case DDNS_REGISTER_WAN_UP_INTERFACE:	/*[IreneWang@MSTC:TO2]$ For record ddns correct interface, default value is 0 means register all wan up, the file will update in inadyn process, 20130917*/
		sprintf(semFile, "%s%s",FILEPATH, "DDNS_WAN_UP_Interface");
		break;
		default:
		    printf("Incorrect semaphore key!");
		break;
	}

	if((fp = fopen(semFile, "r")) == NULL)
	{			
		/*printf("Error to read the data file , getKey = [%d]\n",getKey);	*/ /* ChenHe remark for MFG */
		return -1;
	}
	readData(fp, buff);
	//printf("======>getActivatedInfo: %s=%s\n", semFile, buff);
	return 0;
}

void readData(FILE *fp, char *buff)
{
	char tmp[128];	

	if (fgets(tmp, 128, fp) != 0){
		strcpy(buff, tmp);
	}
	else {
		buff[0] = '\0';
	}
	fclose(fp);
}

int writeData(FILE *fp, char *input) /* kycheng: return prototype int */
{	
	char replace[128] = "";
	if(input == NULL)
	{
		//input = replace;	/* kycheng: put some error message */ 
		strcpy(input, replace);
		fprintf(stderr, "=====Warning: set null into file=====\n");
	}
	fputs(input, fp);	
	fclose(fp);	
	return 0;
}
#endif
