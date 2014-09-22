/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:DUAL/GPL:standard

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
 * 
 ************************************************************************/

#include "cms.h"
#include "cms_util.h"
#include "oal.h"
#include <time.h>
#if 1 /*For To2 PIN syntax Check, Mitrastar ChingLung, 20130219*/
UBOOL8 pinSyntaxCheck(char *pin)
{
	cmsLog_debug("Enter");
	UBOOL8 pinok = FALSE;
	int xlen = 8;
	int PINVAR[xlen];
	int CRC;
	int PINCTRL_CRC_START_VAL=9;
	int PINCTRL_CRC_MAX_VAL=99;
	int i,x,y,z;
	if(cmsUtl_strlen(pin)!=10)
	{
		cmsLog_error("Only 10 digits have to be considered");
		return FALSE;
	}
	CRC = atoi(pin+xlen);

	for(i=0;i<xlen;i++)
	{
		PINVAR[i] = pin[i]-48;
		if(PINVAR[i]>9 || PINVAR[i]<0 )
		{
			cmsLog_error("A special character during PIN insertion (*, #) invalidates the PIN");
			return FALSE;
		}		
	}
	if(PINVAR[0] == 0 || (PINVAR[0] == 1 && PINVAR[1] == 1))
	{
		cmsLog_error("A leading ．0・ or ・11・ is dialed");
		return FALSE;
	}

	y=PINCTRL_CRC_START_VAL;
	for(i=0;i<xlen;i++)
	{
		x=PINVAR[i];
		z=(y+x)^y;
		y=z%(PINCTRL_CRC_MAX_VAL+1);
	}
	fprintf(stderr, "x=%d z=%d y=%d CRC=%d\n", x, z, y, CRC);	
	if(y==CRC)
	{
		pinok =TRUE;
	}
	else
	{
		cmsLog_error("PIN Syntax Check fails");
		pinok = FALSE;
	}
	cmsLog_debug("Exit, pinok=%s",(pinok)?"true":"false");
	return pinok;
}
#endif
int i64c(int i)
{
    if (i <= 0)
        return ('.');
    if (i == 1)
        return ('/');
    if (i >= 2 && i < 12)
        return ('0' - 2 + i);
    if (i >= 12 && i < 38)
        return ('A' - 12 + i);
    if (i >= 38 && i < 63)
        return ('a' - 38 + i);
    return ('z');
}


char *cmsUtil_cryptMakeSalt(void)
{
    time_t now;
    static unsigned long x;
    static char result[3];

    time(&now);
    x += now + oal_getPid() + clock();
    result[0] = i64c(((x >> 18) ^ (x >> 6)) & 077);
    result[1] = i64c(((x >> 12) ^ x) & 077);
    result[2] = '\0';
    
    return result;
}


// function to support passowrd hashing
char *cmsUtil_pwEncrypt(const char *clear, const char *salt)
{
    static char cipher[BUFLEN_24];
    char *cp;

#ifdef CONFIG_FEATURE_SHA1_PASSWORDS
    if (strncmp(salt, "$2$", 3) == 0) {
        return sha1_crypt(clear);
    }
#endif

    cp = oalPass_crypt(clear, salt);
    /* if crypt (a nonstandard crypt) returns a string too large,
       truncate it so we don't overrun buffers and hope there is
       enough security in what's left */
    if (strlen(cp) > sizeof(cipher)-1)
    {
        cp[sizeof(cipher)-1] = 0;
    }
    strcpy(cipher, cp);

    return cipher;
}
