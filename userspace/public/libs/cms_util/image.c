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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <net/if.h>

#include "cms.h"
#include "cms_util.h"
#include "cms_msg.h"
#include "cms_boardcmds.h"
#include "cms_boardioctl.h"
#include "bcmTag.h" /* in shared/opensource/include/bcm963xx, for FILE_TAG */
#include "board.h" /* in bcmdrivers/opensource/include/bcm963xx, for BCM_IMAGE_CFE */
#if 1/*Jayden added for WLAN LED control,MitraStar,20131129*/
#include "prctl.h"
#endif
#define IMPORT_PHONEBOOK_HEADER "Name,First name,Private,Business,Mobile"
static UBOOL8 matchChipId(const char *strTagChipId, const char *signature2);
/*__MTS__, RaynorChung: Support 963268 nand flash, patch form SVN#3597 on http://svn.zyxel.com.tw/svn/CPE_SW1/BCM96368/trunk/P-870HA/branches/cht/fttb8/4.11 */
#ifdef MTS_NAND_FLASH_SUPPORT
static CmsRet verifyBroadcomFileTag(PFILE_TAG pTag);
#else
CmsRet verifyBroadcomFileTag(FILE_TAG *pTag, int imageLen);
#endif /*MTS_NAND_FLASH_SUPPORT*/

/*KeyYang@MSTC: Merge from Telefonica_Common, 20120702*/
#ifdef MSTC_FWUP_FROM_FLASH
static CmsRet verifyBroadcomFileTagFlash(FILE* imageFileFp);
static CmsRet flashImageFlash(FILE* imageFileFp, UINT32 imageLen, CmsImageFormat format, char* filePath);
#endif

static CmsRet flashImage(char *imagePtr, UINT32 imageLen, CmsImageFormat format);
static CmsRet sendConfigMsg(const char *imagePtr, UINT32 imageLen, void *msgHandle, CmsMsgType msgType);
#ifdef SUPPORT_MOD_SW_UPDATE
static void sendStartModupdtdMsg(void *msgHandle);
#endif

/*__MTS__, RaynorChung: Support 963268 nand flash, patch form SVN#3597 on http://svn.zyxel.com.tw/svn/CPE_SW1/BCM96368/trunk/P-870HA/branches/cht/fttb8/4.11 */
#ifdef MTS_NAND_FLASH_SUPPORT
extern int sysGetNVRAMFromFlash(PNVRAM_DATA nvramData);
#endif /*MTS_NAND_FLASH_SUPPORT*/

/**
 * @return TRUE if the modem's chip id matches that of the image.
 */
UBOOL8 matchChipId(const char *strTagChipId, const char *signature2 __attribute__((unused)))
{
    UINT32 tagChipId = 0;
    UINT32 chipId; 
    UBOOL8 match;

    /* this is the image's chip id */
    tagChipId = (UINT32) strtol(strTagChipId, NULL, 16);
    
    /* get the system's chip id */
    devCtl_getChipId(&chipId);

    if (tagChipId == chipId)
    {
        match = TRUE;
    }
    else
    {
        cmsLog_error("Chip Id error.  Image Chip Id = %04x, Board Chip Id = %04x.", tagChipId, chipId);
        match = FALSE;
    }

    return match;
}

// verify the tag of the image
/*__MTS__, RaynorChung: Support 963268 nand flash, patch form SVN#3597 on http://svn.zyxel.com.tw/svn/CPE_SW1/BCM96368/trunk/P-870HA/branches/cht/fttb8/4.11 */
#ifdef MTS_NAND_FLASH_SUPPORT
CmsRet verifyZyXELModelId(FILE_TAG *pTag)
{
//__COMMON__, jchang for model id check
    NVRAM_DATA nvramData;
    char ModelIdPrefix[3]={0};  /* Should be "ZY" Prefix */
    char ModelIdMRD[5]={0};  /* Should be 4 digits */
    char ModelIdTAGIdentity[6]={0};  /* Should be ZyXEL */	
    char ModelIdTAG[5]={0};  /* Should be 4 digits */
    char ModelIdCustomTAG[5]={0};  /* Should be 4 digits */
             
/* 
Here, we only check files with BCM TAG in the begining i.e. fs_kernel and cfe_fs_kernel.
For w image, we skip the model id check so user can still upgrade bootloader with new model id by w image.
*/
    if (!sysGetNVRAMFromFlash( &nvramData )){
        strncpy(ModelIdPrefix, nvramData.FeatureBits, 2);
        sprintf(ModelIdMRD, "%02x%02x",*((char*)(nvramData.FeatureBits+2)),*((char*)(nvramData.FeatureBits+3)));
        strncpy(ModelIdTAGIdentity, pTag->signiture_1, 5);
        strncpy(ModelIdTAG, pTag->signiture_1+6, 4);
        ModelIdPrefix[2] = '\0';
        ModelIdMRD[4] = '\0';
        ModelIdTAGIdentity[5] = '\0';	
        ModelIdTAG[4] = '\0';	   
        /* For Custom Tag, suppose everyone follow the naming rule ZyXEL_xxxx_yyyy */
        memcpy(ModelIdCustomTAG, pTag->signiture_1+11, 4);
        ModelIdCustomTAG[4] = '\0';
		
        cmsLog_debug(" ModelIdPrefix = %s", ModelIdPrefix);
        cmsLog_debug(" ModelIdMRD = %s",ModelIdMRD);   
        cmsLog_debug(" ModelIdTAGIdentity = %s", ModelIdTAGIdentity);
        cmsLog_debug(" ModelIdTAG = %s",ModelIdTAG);   
        cmsLog_debug(" ModelIdCustomTAG = %s",(0 < strlen(ModelIdCustomTAG))?ModelIdCustomTAG:"N/A");

#if 1
/*[IreneWang@MSTC:TO2]$ Reject those illegal firmware if model prefix/identity/id or custome id different, 20130905 + 20130918*/
        if(!strcmp(ModelIdPrefix,"ZY") && !strcmp(ModelIdTAGIdentity,"ZyXEL")
		&& !(strcmp(ModelIdMRD,ModelIdTAG) && strcmp(ModelIdMRD,ModelIdCustomTAG))){
            cmsLog_debug(" Check modelID successfully");
        }else{
            cmsLog_error(" Check modelID fail");
            return CMSRET_INVALID_MODELID;	
        }
    }else{
       cmsLog_error(" Get MRD information fails, please try again");
	 return CMSRET_INVALID_MODELID;
    }
#else
        if(!strcmp(ModelIdPrefix,"ZY") && !strcmp(ModelIdTAGIdentity,"ZyXEL")){
            cmsLog_debug("Finds ZyXEL identity, start checking model ID");
            if(strcmp(ModelIdMRD,ModelIdTAG) && strcmp(ModelIdMRD,ModelIdCustomTAG)){
                cmsLog_error("Check model from TAG is different with MRD");
                return CMSRET_INVALID_MODELID;	
            }else{
                cmsLog_debug("Check modelID successfully");		
            }
        }else{
           cmsLog_error("Not find ZyXEL identify, just fall through");	
        }
    }else{
       cmsLog_error("Get MRD information fails, just fall through");
    }
#endif  /*1*/

    return CMSRET_SUCCESS;
}

// verify the tag of the image
CmsRet verifyBroadcomFileTag(FILE_TAG *pTag)
{
    UINT32 crc;
    int totalImageSize;
    int tagVer, curVer;
    UINT32 tokenCrc, imageCrc;

        
    cmsLog_debug("start of pTag=%p tagversion %02x %02x %02x %02x", pTag,
                  pTag->tagVersion[0], 
                  pTag->tagVersion[1], 
                  pTag->tagVersion[2], 
                  pTag->tagVersion[3]);

    tokenCrc = *((UINT32 *)pTag->tagValidationToken);
    imageCrc = *((UINT32 *)pTag->imageValidationToken);                  
#ifdef DESKTOP_LINUX
    /* assume desktop is running on little endien intel, but the CRC has been
     * written for big endien mips, so swap before compare.
     */
    tokenCrc = htonl(tokenCrc);
    imageCrc = htonl(imageCrc);
#endif
                  
                  
    // check tag validate token first
    crc = CRC_INITIAL_VALUE;
    crc = cmsCrc_getCrc32((UINT8 *) pTag, (UINT32)TAG_LEN-TOKEN_LEN, crc);
    if (crc != tokenCrc)
    {
        /* this function is called even when we are not sure the image is
         * a broadcom image.  So if crc fails, it is not a big deal.  It just
         * means this is not a broadcom image.
         */
        cmsLog_debug("token crc failed, this is not a valid broadcom image");
        cmsLog_debug("calculated crc=0x%x tokenCrc=0x%x", crc, tokenCrc);
        return CMSRET_INVALID_IMAGE;
    }
    cmsLog_debug("header CRC is OK.");

    
    // check imageCrc
    totalImageSize = atoi((char *) pTag->totalImageLen);
    cmsLog_debug("totalImageLen=%d", totalImageSize);

    crc = CRC_INITIAL_VALUE;
// __Verizon__, zongyue
    crc = cmsCrc_getCrc32(((UINT8 *)pTag + TAG_BLOCK_LEN), (UINT32) totalImageSize, crc);
    if (crc != imageCrc)
    {
        /*
         * This should not happen.  We already passed the crc check on the header,
         * so we should pass the crc check on the image.  If this fails, something
         * is wrong.
         */
        cmsLog_error("image crc failed after broadcom header crc succeeded");
        cmsLog_error("calculated crc=0x%x imageCrc=0x%x totalImageSize=%d", crc, imageCrc, totalImageSize); 
        return CMSRET_INVALID_IMAGE;
    }
    cmsLog_debug("image crc is OK");


    tagVer = atoi((char *) pTag->tagVersion);
    curVer = atoi(BCM_TAG_VER);

    if (tagVer != curVer)

    {
       cmsLog_error("Firmware tag version [%d] is not compatible with the current Tag version [%d]", tagVer, curVer);
       return CMSRET_INVALID_IMAGE;
    }
    cmsLog_debug("tarVer=%d, curVar=%d", tagVer, curVer);

    if (!matchChipId((char *) pTag->chipId, (char *) pTag->signiture_2))
    {
       cmsLog_error("chipid check failed");
       return CMSRET_INVALID_IMAGE;
    }

    cmsLog_debug("Good broadcom image");

    return CMSRET_SUCCESS;
}
#else /*MTS_NAND_FLASH_SUPPORT*/
CmsRet verifyBroadcomFileTag(FILE_TAG *pTag, int imageLen)
{
    UINT32 crc;
    int totalImageSize;
    int tagVer, curVer;
    UINT32 tokenCrc, imageCrc;

        
    cmsLog_debug("start of pTag=%p tagversion %02x %02x %02x %02x", pTag,
                  pTag->tagVersion[0], 
                  pTag->tagVersion[1], 
                  pTag->tagVersion[2], 
                  pTag->tagVersion[3]);

    tokenCrc = *((UINT32 *)pTag->tagValidationToken);
    imageCrc = *((UINT32 *)pTag->imageValidationToken);                  
#ifdef DESKTOP_LINUX
    /* assume desktop is running on little endien intel, but the CRC has been
     * written for big endien mips, so swap before compare.
     */
    tokenCrc = htonl(tokenCrc);
    imageCrc = htonl(imageCrc);
#endif
                  
                  
    // check tag validate token first
    crc = CRC_INITIAL_VALUE;
    crc = cmsCrc_getCrc32((UINT8 *) pTag, (UINT32)TAG_LEN-TOKEN_LEN, crc);
    if (crc != tokenCrc)
    {
        /* this function is called even when we are not sure the image is
         * a broadcom image.  So if crc fails, it is not a big deal.  It just
         * means this is not a broadcom image.
         */
        cmsLog_debug("token crc failed, this is not a valid broadcom image");
        cmsLog_debug("calculated crc=0x%x tokenCrc=0x%x", crc, tokenCrc);
        return CMSRET_INVALID_IMAGE;
    }
    cmsLog_debug("header CRC is OK.");

    
    // check imageCrc
    totalImageSize = atoi((char *) pTag->totalImageLen);
    cmsLog_debug("totalImageLen=%d, imageLen=%d, TAG_LEN=%d\n", totalImageSize, imageLen, TAG_LEN);

    if (totalImageSize > (imageLen -TAG_LEN)) {
	 cmsLog_error("invalid size\n");
        return CMSRET_INVALID_IMAGE;
	}
    crc = CRC_INITIAL_VALUE;
    crc = cmsCrc_getCrc32(((UINT8 *)pTag + TAG_LEN), (UINT32) totalImageSize, crc);      
    if (crc != imageCrc)
    {
        /*
         * This should not happen.  We already passed the crc check on the header,
         * so we should pass the crc check on the image.  If this fails, something
         * is wrong.
         */
        cmsLog_error("image crc failed after broadcom header crc succeeded");
        cmsLog_error("calculated crc=0x%x imageCrc=0x%x totalImageSize", crc, imageCrc, totalImageSize); 
        return CMSRET_INVALID_IMAGE;
    }
    cmsLog_debug("image crc is OK");


    tagVer = atoi((char *) pTag->tagVersion);
    curVer = atoi(BCM_TAG_VER);

    if (tagVer != curVer)

    {
       cmsLog_error("Firmware tag version [%d] is not compatible with the current Tag version [%d]", tagVer, curVer);
       return CMSRET_INVALID_IMAGE;
    }
    cmsLog_debug("tarVer=%d, curVar=%d", tagVer, curVer);

    if (!matchChipId((char *) pTag->chipId, (char *) pTag->signiture_2))
    {
       cmsLog_error("chipid check failed");
       return CMSRET_INVALID_IMAGE;
    }

    cmsLog_debug("Good broadcom image");

    return CMSRET_SUCCESS;
}
#endif /*MTS_NAND_FLASH_SUPPORT*/

/*KeyYang@MSTC: Merge from Telefonica_Common, 20120702*/
#ifdef MSTC_FWUP_FROM_FLASH
static CmsRet verifyBroadcomFileTagFlash(FILE* imageFileFp)
{
	UINT32 crc;
	int totalImageSize;
	int tagVer, curVer;
	UINT32 tokenCrc, imageCrc;
	UINT32 tagLen = 0;
	FILE_TAG *pTag = NULL;
	UINT8 c = 0;
	UINT32 i;

	if(imageFileFp == NULL) {
		cmsLog_error("File point is NULL.");
		return CMS_IMAGE_FORMAT_INVALID;
	}

	tagLen = sizeof(FILE_TAG);
	pTag = cmsMem_alloc(tagLen, 0);
	if(pTag == NULL) {
		cmsLog_error("Failed to allocate memory for the tag buffer. Size required %d", tagLen);
		return CMSRET_RESOURCE_EXCEEDED;
	}
	memset(pTag, 0, tagLen);
	fseek(imageFileFp, 0, SEEK_SET);
	fread(pTag, 1, tagLen, imageFileFp);

	cmsLog_debug("start of pTag=%p tagversion %02x %02x %02x %02x", pTag,
		pTag->tagVersion[0], 
		pTag->tagVersion[1], 
		pTag->tagVersion[2], 
		pTag->tagVersion[3]);

	tokenCrc = *((UINT32 *)pTag->tagValidationToken);
	imageCrc = *((UINT32 *)pTag->imageValidationToken);            
#ifdef DESKTOP_LINUX
	/* assume desktop is running on little endien intel, but the CRC has been
	* written for big endien mips, so swap before compare.
	*/
	tokenCrc = htonl(tokenCrc);
	imageCrc = htonl(imageCrc);
#endif

	// check tag validate token first
	crc = CRC_INITIAL_VALUE;
	crc = cmsCrc_getCrc32((UINT8 *)pTag, (UINT32)TAG_LEN-TOKEN_LEN, crc);
	if(crc != tokenCrc) {
		/* this function is called even when we are not sure the image is
		* a broadcom image.  So if crc fails, it is not a big deal.  It just
		* means this is not a broadcom image.
		*/
		cmsLog_debug("token crc failed, this is not a valid broadcom image");
		cmsLog_debug("calculated crc=0x%x tokenCrc=0x%x", crc, tokenCrc);
		return CMSRET_INVALID_IMAGE;
	}
	cmsLog_debug("header CRC is OK.");

	/* Need to enhance CRC check time */
	// check imageCrc
	totalImageSize = atoi((char *) pTag->totalImageLen);
	cmsLog_debug("totalImageLen=%d", totalImageSize);

	fseek(imageFileFp, TAG_BLOCK_LEN, SEEK_SET);
	crc = CRC_INITIAL_VALUE;
	i = totalImageSize;
	while(i-- > 0) {
		c = (UINT8)fgetc(imageFileFp);
		crc = cmsCrc_getCrc32(&c, 1, crc);
	}

	if (crc != imageCrc) {
		/*
		* This should not happen.  We already passed the crc check on the header,
		* so we should pass the crc check on the image.  If this fails, something
		* is wrong.
		*/
		cmsLog_error("image crc failed after broadcom header crc succeeded");
		cmsLog_error("calculated crc=0x%x imageCrc=0x%x totalImageSize=%d", crc, imageCrc, totalImageSize); 
		return CMSRET_INVALID_IMAGE;
	}
	cmsLog_debug("image crc is OK");

	tagVer = atoi((char *) pTag->tagVersion);
	curVer = atoi(BCM_TAG_VER);

	if (tagVer != curVer) {
		cmsLog_error("Firmware tag version [%d] is not compatible with the current Tag version [%d]", tagVer, curVer);
		return CMSRET_INVALID_IMAGE;
	}
	cmsLog_debug("tarVer=%d, curVar=%d", tagVer, curVer);

	if (!matchChipId((char *) pTag->chipId, (char *) pTag->signiture_2)) {
		cmsLog_error("chipid check failed");
		return CMSRET_INVALID_IMAGE;
	}

	fseek(imageFileFp, 0, SEEK_SET);
	cmsLog_debug("Good broadcom image");

    return CMSRET_SUCCESS;
}

static CmsRet flashImageFlash(FILE* imageFileFp, UINT32 imageLen, CmsImageFormat format, char* filepath)
{
	FILE_TAG Tag, *pTag = NULL;
	int cfeSize, rootfsSize, kernelSize;
	unsigned long cfeAddr, rootfsAddr, kernelAddr;
	CmsRet ret;
/* Image Default, __CHT__, MitraStar Curtis, 20111124. */
#ifdef ZYXEL_OBM_IMAGE_DEFAULT  
	PIMAGE_TAG pTag_Img = NULL;
	unsigned long imageSize = 0;
	unsigned long search_tag_addr = 0;
#endif
	unsigned char *buf = NULL;
	fwup_flash_parm_t fwup_flash_parm;

	if(imageFileFp == NULL) {
		cmsLog_error("File pointer is NULL.");
		return CMS_IMAGE_FORMAT_INVALID;
	}

	pTag = &Tag;
	memset(pTag, 0, sizeof(FILE_TAG));
	fseek(imageFileFp, 0, SEEK_SET);
	fread(pTag, 1, sizeof(FILE_TAG), imageFileFp);

	if (format != CMS_IMAGE_FORMAT_BROADCOM) {
		cmsLog_error("invalid image format %d", format);
		return CMSRET_INVALID_IMAGE;
	}

	/* this must be a broadcom format image */
	// check imageCrc
	cfeSize = rootfsSize = kernelSize = 0;

	// check cfe's existence
	cfeAddr = (unsigned long) strtoul((char *) pTag->cfeAddress, NULL, 10);
	cfeSize = atoi((char *) pTag->cfeLen);
	// check kernel existence
	kernelAddr = (unsigned long) strtoul((char *) pTag->kernelAddress, NULL, 10);
	kernelSize = atoi((char *) pTag->kernelLen);
	// check root filesystem existence
	rootfsAddr = (unsigned long) strtoul((char *) pTag->rootfsAddress, NULL, 10);
	rootfsSize = atoi((char *) pTag->rootfsLen);
	cmsLog_debug("cfeSize=%d kernelSize=%d rootfsSize=%d", cfeSize, kernelSize, rootfsSize);
	
	if (cfeAddr) {
		printf("Flashing CFE...\n");
		buf = cmsMem_alloc(cfeSize, 0);
		if(buf == NULL) {
			cmsLog_error("Failed to allocate memory for the tag buffer. Size required %d", cfeSize);
			return CMSRET_RESOURCE_EXCEEDED;
		}
		memset(buf, 0, cfeSize);
		fseek(imageFileFp, TAG_BLOCK_LEN, SEEK_SET);
		fread(buf, 1, cfeSize, imageFileFp);

		ret = devCtl_boardIoctl(BOARD_IOCTL_FLASH_WRITE,
				BCM_IMAGE_CFE,
				(char*)buf,
				cfeSize,
				(int) cfeAddr, 0);
		if (ret != CMSRET_SUCCESS) {
			if(buf) {
				cmsMem_free(buf);
				buf = NULL;
			}
			cmsLog_error("Failed to flash CFE");
			return CMSRET_IMAGE_FLASH_FAILED;
		}
		if(buf) {
			cmsMem_free(buf);
			buf = NULL;
		}
	}

/*__MTS__, RaynorChung: Support 963268 nand flash, patch form SVN#3597 on http://svn.zyxel.com.tw/svn/CPE_SW1/BCM96368/trunk/P-870HA/branches/cht/fttb8/4.11 */
#if (INC_NAND_FLASH_DRIVER == 1)    
	if (rootfsAddr) 
#else
	if (rootfsAddr && kernelAddr)
#endif
	{
		char *tagFs = NULL;

		// tag is alway at the sector start of fs
		if (cfeAddr) {
			tagFs = cmsMem_alloc(TAG_BLOCK_LEN, 0);
			if(tagFs == NULL) {
				cmsLog_error("Failed to allocate memory for the tag buffer. Size required %d", TAG_BLOCK_LEN);
				return CMSRET_RESOURCE_EXCEEDED;
			}
			memset(tagFs, 0, TAG_BLOCK_LEN);
			fseek(imageFileFp, 0, SEEK_SET);
			fread(tagFs, 1, TAG_BLOCK_LEN, imageFileFp);
			fseek(imageFileFp, cfeSize, SEEK_SET);
			fwrite(tagFs, 1, TAG_BLOCK_LEN, imageFileFp);
			if(tagFs) {
				cmsMem_free(tagFs);
				tagFs = NULL;
			}
		}

/* Image Default, __CHT__, MitraStar Curtis, 20111124. */
#ifdef ZYXEL_OBM_IMAGE_DEFAULT  
		if (0x1 == pTag->imageNext[0]) {
			unsigned int TagCrc, crc;

			pTag_Img = cmsMem_alloc(sizeof(IMAGE_TAG), 0);
			if(pTag_Img == NULL) {
				cmsLog_error("Failed to allocate memory for the tag buffer. Size required %d", sizeof(IMAGE_TAG));
				return CMSRET_RESOURCE_EXCEEDED;
			}
			memset(pTag_Img, 0, sizeof(IMAGE_TAG));

			search_tag_addr = cfeSize + TAG_BLOCK_LEN + rootfsSize + kernelSize;
			do {
				fseek(imageFileFp, search_tag_addr, SEEK_SET);
				fread(pTag_Img, 1, sizeof(IMAGE_TAG), imageFileFp);
				/* 4-byte boundary protection */
				memcpy(&TagCrc, pTag_Img->tagValidationToken, CRC_LEN);
				crc = CRC32_INIT_VALUE;
				crc = cmsCrc_getCrc32((unsigned char *)pTag_Img, (UINT32)(IMAGE_TAG_LEN-CRC_LEN), crc);
				if (crc != TagCrc) {
					printf("IMAGE_TAG CRC error. Corrupted image?\n");
					break;
				}
				/* flashFsKernelImage function only process ImageDefault and ROM-D images */
				if (IMAGE_TYPE_IMGDEF == strtoul(pTag_Img->imageType, NULL, 10)) {
					/* first extra image should be ImageDefault then ROM-D */
					imageSize += (strtoul(pTag_Img->imageLen, NULL, 10) + IMAGE_TAG_LEN);
					search_tag_addr += (IMAGE_TAG_LEN + strtoul(pTag_Img->imageLen, NULL, 10));
				} else if (IMAGE_TYPE_ROMD == strtoul(pTag_Img->imageType, NULL, 10)) {
					/* ROM-D is last extra image we want to process at flashFsKernelImage */
					imageSize += (strtoul(pTag_Img->imageLen, NULL, 10) + IMAGE_TAG_LEN);
					search_tag_addr += (IMAGE_TAG_LEN + strtoul(pTag_Img->imageLen, NULL, 10));
					break;
				} else {
					search_tag_addr += (IMAGE_TAG_LEN + strtoul(pTag_Img->imageLen, NULL, 10));;
				}

				if (search_tag_addr > imageLen) {
					break;
				}
			} while (0x1 == pTag_Img->imageNext[0]);

			if(pTag_Img) {
				cmsMem_free(pTag_Img);
				pTag_Img = NULL;
			}
		}
#endif /*ZYXEL_OBM_IMAGE_DEFAULT*/

		printf("Flashing root file system and kernel...\n");
		/* only the buf pointer and length is needed, the offset parameter
		* was present in the legacy code, but is not used. */
		memset(&fwup_flash_parm, 0, sizeof(fwup_flash_parm_t));
		sprintf(fwup_flash_parm.filename, "%s", filepath);

#ifdef ZYXEL_OBM_IMAGE_DEFAULT
		fwup_flash_parm.size = (TAG_BLOCK_LEN + rootfsSize + kernelSize + imageSize);
#else
		fwup_flash_parm.size = (TAG_BLOCK_LEN + rootfsSize+kernelSize);
#endif
		if(cfeAddr) {
			fwup_flash_parm.offset = cfeSize;
		} else {
			fwup_flash_parm.offset = 0;
		}

		ret = devCtl_boardIoctl(BOARD_IOCTL_FLASH_WRITE, IMAGE_FS_FLASH, NULL, 0, 0, &fwup_flash_parm);
		if (ret != CMSRET_SUCCESS) {
			cmsLog_error("Failed to flash root file system and kernel");
			return CMSRET_IMAGE_FLASH_FAILED;
		}
	}

	cmsLog_notice("Image flash done.");

	return CMSRET_SUCCESS;
}
#endif /*MSTC_FWUP_FROM_FLASH*/

// depending on the image type, do the brcm image or whole flash image
CmsRet flashImage(char *imagePtr, UINT32 imageLen, CmsImageFormat format)
{
    FILE_TAG *pTag = (FILE_TAG *) imagePtr;
/*__MTS__, RaynorChung: Support 963268 nand flash, patch form SVN#3597 on http://svn.zyxel.com.tw/svn/CPE_SW1/BCM96368/trunk/P-870HA/branches/cht/fttb8/4.11 */
#ifdef MTS_NAND_FLASH_SUPPORT
    int cfeSize, rootfsSize, kernelSize;
#else
    int cfeSize, rootfsSize, kernelSize, noReboot;
#endif
    unsigned long cfeAddr, rootfsAddr, kernelAddr;
    CmsRet ret;

/* Image Default, __CHT__, MitraStar Curtis, 20111124. */
#ifdef ZYXEL_OBM_IMAGE_DEFAULT  
    PIMAGE_TAG pTag_Img = NULL;
    unsigned long imageSize = 0;
    unsigned long search_tag_addr = 0;
#endif        
    
/*__MTS__, RaynorChung: Support 963268 nand flash, patch form SVN#3597 on http://svn.zyxel.com.tw/svn/CPE_SW1/BCM96368/trunk/P-870HA/branches/cht/fttb8/4.11 */
#ifndef MTS_NAND_FLASH_SUPPORT
    if( (format & CMS_IMAGE_FORMAT_PART1) == CMS_IMAGE_FORMAT_PART1 )
    {
        noReboot = ((format & CMS_IMAGE_FORMAT_NO_REBOOT) == 0)
            ? FLASH_PART1_REBOOT : FLASH_PART1_NO_REBOOT;
    }
    else if( (format & CMS_IMAGE_FORMAT_PART2) == CMS_IMAGE_FORMAT_PART2 )
    {
        noReboot = ((format & CMS_IMAGE_FORMAT_NO_REBOOT) == 0)
            ? FLASH_PART2_REBOOT : FLASH_PART2_NO_REBOOT;
    }
    else
        noReboot = ((format & CMS_IMAGE_FORMAT_NO_REBOOT) == 0)
            ? FLASH_PARTDEFAULT_REBOOT : FLASH_PARTDEFAULT_NO_REBOOT;
 
    format &= ~(CMS_IMAGE_FORMAT_NO_REBOOT | CMS_IMAGE_FORMAT_PART1 |
       CMS_IMAGE_FORMAT_PART2); 
#endif /*MTS_NAND_FLASH_SUPPORT*/

    if (format != CMS_IMAGE_FORMAT_FLASH && format != CMS_IMAGE_FORMAT_BROADCOM)
    {
       cmsLog_error("invalid image format %d", format);
       return CMSRET_INVALID_IMAGE;
    }

    if (format == CMS_IMAGE_FORMAT_FLASH)  
    {
        cmsLog_notice("Flash whole image...");
        // Pass zero for the base address of whole image flash. It will be filled by kernel code
        // was sysFlashImageSet
/*__MTS__, RaynorChung: Support 963268 nand flash, patch form SVN#3597 on http://svn.zyxel.com.tw/svn/CPE_SW1/BCM96368/trunk/P-870HA/branches/cht/fttb8/4.11 */
#ifdef MTS_NAND_FLASH_SUPPORT
        ret = devCtl_boardIoctl(BOARD_IOCTL_FLASH_WRITE,
                                BCM_IMAGE_WHOLE,
                                imagePtr,
                                imageLen-TOKEN_LEN,
                                0, 0);
#else
        ret = devCtl_boardIoctl(BOARD_IOCTL_FLASH_WRITE,
                                BCM_IMAGE_WHOLE,
                                imagePtr,
                                imageLen-TOKEN_LEN,
                                noReboot, 0);
#endif /*MTS_NAND_FLASH_SUPPORT*/
        if (ret != CMSRET_SUCCESS)
        {
           cmsLog_error("Failed to flash whole image");
           return CMSRET_IMAGE_FLASH_FAILED;
        }
        else
        {
           return CMSRET_SUCCESS;
        }
    }

    /* this must be a broadcom format image */
    // check imageCrc
    cfeSize = rootfsSize = kernelSize = 0;

    // check cfe's existence
    cfeAddr = (unsigned long) strtoul((char *) pTag->cfeAddress, NULL, 10);
    cfeSize = atoi((char *) pTag->cfeLen);
    // check kernel existence
    kernelAddr = (unsigned long) strtoul((char *) pTag->kernelAddress, NULL, 10);
    kernelSize = atoi((char *) pTag->kernelLen);
    // check root filesystem existence
    rootfsAddr = (unsigned long) strtoul((char *) pTag->rootfsAddress, NULL, 10);
    rootfsSize = atoi((char *) pTag->rootfsLen);
    cmsLog_debug("cfeSize=%d kernelSize=%d rootfsSize=%d", cfeSize, kernelSize, rootfsSize);
    
    if (cfeAddr) 
    {
        printf("Flashing CFE...\n");
        ret = devCtl_boardIoctl(BOARD_IOCTL_FLASH_WRITE,
                                BCM_IMAGE_CFE,
/*__MTS__, RaynorChung: Support 963268 nand flash, patch form SVN#3597 on http://svn.zyxel.com.tw/svn/CPE_SW1/BCM96368/trunk/P-870HA/branches/cht/fttb8/4.11 */
#ifdef MTS_NAND_FLASH_SUPPORT
                                imagePtr+TAG_BLOCK_LEN,
#else
                                imagePtr+TAG_LEN,
#endif /*MTS_NAND_FLASH_SUPPORT*/
                                cfeSize,
                                (int) cfeAddr, 0);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to flash CFE");
            return CMSRET_IMAGE_FLASH_FAILED;
        }
    }
/*__MTS__, RaynorChung: Support 963268 nand flash, patch form SVN#3597 on http://svn.zyxel.com.tw/svn/CPE_SW1/BCM96368/trunk/P-870HA/branches/cht/fttb8/4.11 */
#ifdef MTS_NAND_FLASH_SUPPORT
#if (INC_NAND_FLASH_DRIVER==1)   
    if (rootfsAddr) 
#else
    if (rootfsAddr && kernelAddr) 
#endif /*INC_NAND_FLASH_DRIVER*/
#else /*MTS_NAND_FLASH_SUPPORT*/
    if (rootfsAddr && kernelAddr) 
#endif /*MTS_NAND_FLASH_SUPPORT*/
    {
        char *tagFs = imagePtr;

        // tag is alway at the sector start of fs
        if (cfeAddr)
        {
            tagFs = imagePtr + cfeSize;       // will trash cfe memory, but cfe is already flashed
/*__MTS__, RaynorChung: Support 963268 nand flash, patch form SVN#3597 on http://svn.zyxel.com.tw/svn/CPE_SW1/BCM96368/trunk/P-870HA/branches/cht/fttb8/4.11 */
#ifdef MTS_NAND_FLASH_SUPPORT
            memcpy(tagFs, imagePtr, TAG_BLOCK_LEN);
#else
            memcpy(tagFs, imagePtr, TAG_LEN);
#endif /*MTS_NAND_FLASH_SUPPORT*/
        }

/* Image Default, __CHT__, MitraStar Curtis, 20111124. */
#ifdef ZYXEL_OBM_IMAGE_DEFAULT  
        if (0x1 == pTag->imageNext[0]) {
            unsigned int TagCrc, crc;
        
            search_tag_addr = cfeSize + TAG_BLOCK_LEN + rootfsSize + kernelSize;
            do {
                pTag_Img = (PIMAGE_TAG)(imagePtr + search_tag_addr);
                /* 4-byte boundary protection */
                memcpy(&TagCrc, pTag_Img->tagValidationToken, CRC_LEN);
                crc = CRC32_INIT_VALUE;
                crc = cmsCrc_getCrc32((unsigned char *)pTag_Img, (UINT32)(IMAGE_TAG_LEN-CRC_LEN), crc);
                if (crc != TagCrc) {
                    printf("IMAGE_TAG CRC error. Corrupted image?\n");
                    break;
                }
                /* flashFsKernelImage function only process ImageDefault and ROM-D images */
                if (IMAGE_TYPE_IMGDEF == strtoul(pTag_Img->imageType, NULL, 10)) {
                    /* first extra image should be ImageDefault then ROM-D */
                    imageSize += (strtoul(pTag_Img->imageLen, NULL, 10) + IMAGE_TAG_LEN);
                    search_tag_addr += (IMAGE_TAG_LEN + strtoul(pTag_Img->imageLen, NULL, 10));
                }
                else if (IMAGE_TYPE_ROMD == strtoul(pTag_Img->imageType, NULL, 10)) {
                    /* ROM-D is last extra image we want to process at flashFsKernelImage */
                    imageSize += (strtoul(pTag_Img->imageLen, NULL, 10) + IMAGE_TAG_LEN);
                    search_tag_addr += (IMAGE_TAG_LEN + strtoul(pTag_Img->imageLen, NULL, 10));
                    break;
                }
                else {
                    search_tag_addr += (IMAGE_TAG_LEN + strtoul(pTag_Img->imageLen, NULL, 10));;
                }
                
                if (search_tag_addr > imageLen)
                    break;
            } while (0x1 == pTag_Img->imageNext[0]);
        }
#endif /*ZYXEL_OBM_IMAGE_DEFAULT*/

        printf("Flashing root file system and kernel...\n");
        /* only the buf pointer and length is needed, the offset parameter
         * was present in the legacy code, but is not used. */
        ret = devCtl_boardIoctl(BOARD_IOCTL_FLASH_WRITE,
                                BCM_IMAGE_FS,
                                tagFs,
/*__MTS__, RaynorChung: Support 963268 nand flash, patch form SVN#3597 on http://svn.zyxel.com.tw/svn/CPE_SW1/BCM96368/trunk/P-870HA/branches/cht/fttb8/4.11 */
#ifdef MTS_NAND_FLASH_SUPPORT
#ifdef ZYXEL_OBM_IMAGE_DEFAULT  /* Image Default, __CHT__, MitraStar Curtis, 20111124. */
                                TAG_BLOCK_LEN+rootfsSize+kernelSize+imageSize,
#else /*ZYXEL_OBM_IMAGE_DEFAULT*/
                                TAG_BLOCK_LEN+rootfsSize+kernelSize,
#endif /*ZYXEL_OBM_IMAGE_DEFAULT*/
                                0, 0); 
#else /*MTS_NAND_FLASH_SUPPORT*/
                                TAG_LEN+rootfsSize+kernelSize,
                                noReboot, 0);
#endif /*MTS_NAND_FLASH_SUPPORT*/
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to flash root file system and kernel");
            return CMSRET_IMAGE_FLASH_FAILED;
        }
    }

    cmsLog_notice("Image flash done.");
    
    return CMSRET_SUCCESS;
}

/*__MTS__, RaynorChung: Support 963268 nand flash, patch form SVN#3597 on http://svn.zyxel.com.tw/svn/CPE_SW1/BCM96368/trunk/P-870HA/branches/cht/fttb8/4.11 */
#ifdef MTS_NAND_FLASH_SUPPORT
UINT32 cmsImg_getImageFlashSizeLimit(void)
{   
   return FLASH_IMAGE_DOWNLOAD_SIZE;
}
#endif

UINT32 cmsImg_getImageFlashSize(void)
{
   UINT32 flashSize=0;
   CmsRet ret;
   
   ret = devCtl_boardIoctl(BOARD_IOCTL_FLASH_READ,
                           FLASH_SIZE,
                           0, 0, 0, &flashSize);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get flash size, return 0");
      flashSize = 0;
   }
   
   return flashSize;
}


UINT32 cmsImg_getBroadcomImageTagSize(void)
{
   return TOKEN_LEN;
}


UINT32 cmsImg_getConfigFlashSize(void)
{
   UINT32 realSize;

   realSize = cmsImg_getRealConfigFlashSize();

#ifdef COMPRESSED_CONFIG_FILE   
   /*
    * A multiplier of 2 is now too small for some of the big voice and WLAN configs,   
    * so allow for the possibility of 4x compression ratio.  In a recent test on the
    * 6368 with wireless enabled, I got a compression ratio of 3.5.
    * The real test comes in management.c: after we do the
    * compression, writeValidatedConfigBuf will verify that the compressed buffer can
    * fit into the flash.
    * A 4x multiplier should be OK for small memory systems such as the 6338.
    * The kernel does not allocate physical pages until they are touched.
    * However, allocating an overly large buffer could be rejected immediately by the
    * kernel because it does not know we don't actually plan to use the entire buffer.
    * So if this is a problem on the 6338, we could improve this algorithm to
    * use a smaller factor on low memory systems.
    */
   realSize = realSize * 4;
#endif

   return realSize;
}


UINT32 cmsImg_getRealConfigFlashSize(void)
{
   CmsRet ret;
   UINT32 size=0;

   ret = devCtl_boardIoctl(BOARD_IOCTL_GET_PSI_SIZE, 0, NULL, 0, 0, (void *)&size);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("boardIoctl to get config flash size failed.");
      size = 0;
   }

   return size;
}


UBOOL8 cmsImg_willFitInFlash(UINT32 imageSize)
{
   UINT32 flashSize;
   
   flashSize = cmsImg_getImageFlashSize();

   cmsLog_debug("flash size is %u bytes, imageSize=%u bytes", flashSize, imageSize);
                     
   return (flashSize > (imageSize + CMS_IMAGE_OVERHEAD));
}


UBOOL8 cmsImg_isBackupConfigFlashAvailable(void)
{
   static UBOOL8 firstTime=TRUE;
   static UBOOL8 avail=FALSE;
   UINT32 size=0;
   CmsRet ret;

   if (firstTime)
   {
      ret = devCtl_boardIoctl(BOARD_IOCTL_GET_BACKUP_PSI_SIZE, 0, NULL, 0, 0, (void *)&size);
      if (ret == CMSRET_SUCCESS && size > 0)
      {
         avail = TRUE;
      }

      firstTime = FALSE;
   }

   return avail;
}


UBOOL8 cmsImg_isConfigFileLikely(const char *buf)
{
   const char *header = "<?xml version";
   const char *dslCpeConfig = "<DslCpeConfig";
   UINT32 len, i;
   UBOOL8 likely=FALSE;

#if 1
/*[BUG FIX]Define UTF8 header for importing phonebook CSV file including German*/
	char tmpUTF8PhonebookHeader[256];
	snprintf(tmpUTF8PhonebookHeader, sizeof(tmpUTF8PhonebookHeader),"%c%c%c%s",0xEF,0xBB,0xBF,IMPORT_PHONEBOOK_HEADER);
#endif   
/*[IreneWang@MSTC:TO2]$ The output config file must be encrypted, 20130228*/
#if defined(COMPRESSED_CONFIG_FILE) && 1
   if ((!strncmp(buf, COMPRESSED_CONFIG_HEADER, strlen(COMPRESSED_CONFIG_HEADER))))
   {
	 likely = TRUE;
	 cmsLog_debug("returning likely for compress type=%d", likely);
	 return likely;
   }
   else if (strncmp(buf, IMPORT_PHONEBOOK_HEADER, strlen(IMPORT_PHONEBOOK_HEADER))==0
	   ||(!strncmp(buf, tmpUTF8PhonebookHeader, strlen(tmpUTF8PhonebookHeader)))
   	)
   {
   	likely = TRUE;
	cmsLog_debug("returning likely for phonebook import=%d", likely);
	return likely;
   }
   else if (strncmp(buf, "<?xml version", strlen(header)) == 0)
#else
   else if (strncmp(buf, IMPORT_PHONEBOOK_HEADER, strlen(IMPORT_PHONEBOOK_HEADER))==0
	   ||(!strncmp(buf, tmpUTF8PhonebookHeader, strlen(tmpUTF8PhonebookHeader)))
   	)
   {
   	likely = TRUE;
	cmsLog_debug("returning likely for phonebook import=%d", likely);
	return likely;
   }
   else if (strncmp(buf, "<?xml version", strlen(header)) == 0)
#endif  /*1*/
   {
      len = strlen(dslCpeConfig);
      for (i=20; i<50 && !likely; i++)
      {
         if (strncmp(&(buf[i]), dslCpeConfig, len) == 0)
         {
            likely = TRUE;
         }
      }
   }
   
   cmsLog_debug("returning likely=%d", likely);
   
   return likely;
}

/*KeyYang@MSTC: Merge from Telefonica_Common, 20120702*/
#ifdef MSTC_FWUP_FROM_FLASH
CmsRet cmsImg_writeImageFile(FILE *imageFileFp, UINT32 imageLen, void *msgHandle, char* filepath)
{
	CmsImageFormat format;
	CmsRet ret;

	if(imageFileFp == NULL) {
		cmsLog_error("File point is NULL.");
		return CMS_IMAGE_FORMAT_INVALID;
	}

	if ((format = cmsImg_validateImageFile(imageFileFp, imageLen, msgHandle)) == CMS_IMAGE_FORMAT_INVALID) {
		ret = CMSRET_INVALID_IMAGE;
	} else {
		ret = cmsImg_writeValidatedImageFile(imageFileFp, imageLen, format, msgHandle, filepath);
	}

	return ret;
}

CmsRet cmsImg_writeValidatedImageFile(FILE *imageFileFp, UINT32 imageLen, CmsImageFormat format, void *msgHandle, char* filepath)
{
	CmsRet ret = CMSRET_SUCCESS;

	if(imageFileFp == NULL) {
		cmsLog_error("File point is NULL.");
		return CMS_IMAGE_FORMAT_INVALID;
	}

	#if 1
	if(format != CMS_IMAGE_FORMAT_CSV_CFG) {
		system("killall callmngr_bspal");
		system("callmngr_cli command system.shutdown all");
	}
	#endif

	switch(format & ~(CMS_IMAGE_FORMAT_NO_REBOOT | CMS_IMAGE_FORMAT_PART1 | CMS_IMAGE_FORMAT_PART2)) {
		case CMS_IMAGE_FORMAT_BROADCOM:
			// BcmNtwk_unInit(); mwang_todo: is it important to let Wireless do some
			// uninit before we write the flash image?
			ret = flashImageFlash(imageFileFp, imageLen, format, filepath);
			break;

		default:
			cmsLog_error("Unrecognized image format=%d", format);
			ret = CMSRET_INVALID_IMAGE;
			break;
	}

	return ret;
}

CmsImageFormat cmsImg_validateImageFile(FILE* imageFileFp, UINT32 imageLen, void *msgHandle)
{
	CmsImageFormat result = CMS_IMAGE_FORMAT_INVALID;
	UINT32 tagLen = 0; 
	UINT8 *buf = NULL;
	UINT32 maxLen = 0;
	   
	if(imageFileFp == NULL) {
		return CMS_IMAGE_FORMAT_INVALID;
	}

	if ((imageLen > sizeof(FILE_TAG)) && (verifyBroadcomFileTagFlash(imageFileFp) == CMSRET_SUCCESS)) {
		/* Found a valid Broadcom defined TAG record at the beginning of the image */
		cmsLog_debug("Broadcom format verified.");
		maxLen = cmsImg_getImageFlashSize() + cmsImg_getBroadcomImageTagSize();
		if (imageLen > maxLen) {
			cmsLog_error("broadcom image is too large for flash, got %u, max %u", imageLen, maxLen);
		} else {
			result = CMS_IMAGE_FORMAT_BROADCOM;
		}
	}

	if(result == CMS_IMAGE_FORMAT_BROADCOM) {
		tagLen = sizeof(FILE_TAG);
		buf = cmsMem_alloc(tagLen, 0);
		if(buf == NULL) {
			cmsLog_error("Failed to allocate memory for the tag buffer. Size required %d", tagLen);
			return CMSRET_RESOURCE_EXCEEDED;
		}
		fseek(imageFileFp, 0, SEEK_SET);
		fread(buf, 1, tagLen, imageFileFp);
		if(verifyZyXELModelId((FILE_TAG *)buf) != CMSRET_SUCCESS) {
			result = CMS_IMAGE_MODELID_INVALID;
		}
		fseek(imageFileFp, 0, SEEK_SET);
		cmsMem_free(buf);
		buf = NULL;
	}

	cmsLog_debug("returning image format %d", result);

	return result;
}
#endif /*MSTC_FWUP_FROM_FLASH*/

/** General entry point for writing the image.
 *  The image can be a flash image or a config file.
 *  This function will first determine the image type, which has the side
 *  effect of validating it.
 */
CmsRet cmsImg_writeImage(char *imagePtr, UINT32 imageLen, void *msgHandle)
{
   CmsImageFormat format;
   CmsRet ret;
   
   if ((format = cmsImg_validateImage(imagePtr, imageLen, msgHandle)) == CMS_IMAGE_FORMAT_INVALID)
   {
      ret = CMSRET_INVALID_IMAGE;
   }
   else
   {
      ret = cmsImg_writeValidatedImage(imagePtr, imageLen, format, msgHandle);
   }

   return ret;
}


CmsRet cmsImg_writeValidatedImage(char *imagePtr, UINT32 imageLen, CmsImageFormat format, void *msgHandle)
{
   CmsRet ret=CMSRET_SUCCESS;

   /*[IreneWang@MSTC:TO2]$ Remove the call manager shutdown so that autodisconnect process can take over the job, 20130605*/
   #if 0
   if(format != CMS_IMAGE_FORMAT_CSV_CFG) {
	system("killall callmngr_bspal");
   	system("callmngr_cli command system.shutdown all");
   }
   #endif
   
   switch(format & ~(CMS_IMAGE_FORMAT_NO_REBOOT | CMS_IMAGE_FORMAT_PART1 | CMS_IMAGE_FORMAT_PART2))
   {
   case CMS_IMAGE_FORMAT_BROADCOM:
   case CMS_IMAGE_FORMAT_FLASH:
      // BcmNtwk_unInit(); mwang_todo: is it important to let Wireless do some
      // uninit before we write the flash image?
      ret = flashImage(imagePtr, imageLen, format);
      break;
      
   case CMS_IMAGE_FORMAT_XML_CFG:
      ret = sendConfigMsg(imagePtr, imageLen, msgHandle, CMS_MSG_WRITE_CONFIG_FILE);
      if (ret == CMSRET_SUCCESS)
      {
         /*
          * Emulate the behavior of the driver when a flash image is written.
          * When we write a config image, also request immediate reboot
          * because we don't want to let any other app save the config file
          * to flash, thus wiping out what we just written.
          */
         cmsLog_debug("config file download written, request reboot");
         cmsUtil_sendRequestRebootMsg(msgHandle);
      }
      break;

/*[IreneWang@MSTC:TO2]$ Partial configuration and restore + Power led light for different case, 20121227 + 20130205*/
#if 1
    case CMS_IMAGE_FORMAT_ACSXML_CFG:
	ret = sendConfigMsg(imagePtr, imageLen, msgHandle, CMS_MSG_WRITE_ACSCONFIG_FILE);
      if (ret == CMSRET_SUCCESS)
      {
	  prctl_runCommandInShellBlocking("wlctl -i wl1 ledbh 10 1");/*5G light,to disable 5G LED data function,20131129*/
	 prctl_runCommandInShellBlocking("wlctl -i wl0 ledbh 0 0");/*2.4G light,to disable 2.4G LED data function,20131129*/	
         sendEventtoBSPALMsg(msgHandle);/*Jayden added for disable bspl led control,MitraStar,20131112*/
         cmsLog_debug("ACS partial config formate success, request reboot");
	   devCtl_setAllGreenLed(kLedStateSlowBlinkContinues);
         cmsUtil_sendRequestRebootMsg(msgHandle);	
      }
      break;
	  
    case CMS_IMAGE_FORMAT_GUIXML_CFG:
	ret = sendConfigMsg(imagePtr, imageLen, msgHandle, CMS_MSG_WRITE_GUICONFIG_FILE);
      if (ret == CMSRET_SUCCESS)
      {
	  prctl_runCommandInShellBlocking("wlctl -i wl1 ledbh 10 1");/*5G light,to disable 5G LED data function,20131129*/
	 prctl_runCommandInShellBlocking("wlctl -i wl0 ledbh 0 0");/*2.4G light,to disable 2.4G LED data function,20131129*/	
         sendEventtoBSPALMsg(msgHandle);/*Jayden added for disable bspl led control,MitraStar,20131112*/
         cmsLog_debug("GUI partial config formate success, request reboot");
 	   devCtl_setAllGreenLed(kLedStateSlowBlinkContinues);
         cmsUtil_sendRequestRebootMsg(msgHandle);
      }
      break;
	  
    case CMS_IMAGE_FORMAT_AREAXML_CFG:
	ret = sendConfigMsg(imagePtr, imageLen, msgHandle, CMS_MSG_WRITE_AREACONFIG_FILE);
      if (ret == CMSRET_SUCCESS)
      {
	  prctl_runCommandInShellBlocking("wlctl -i wl1 ledbh 10 1");/*5G light,to disable 5G LED data function,20131129*/
	 prctl_runCommandInShellBlocking("wlctl -i wl0 ledbh 0 0");/*2.4G light,to disable 2.4G LED data function,20131129*/	
         sendEventtoBSPALMsg(msgHandle);/*Jayden added for disable bspl led control,MitraStar,20131112*/
         cmsLog_debug("Area Detection partial config formate success, request reboot");
	   devCtl_setAllGreenLed(kLedStateSlowBlinkContinues);
         cmsUtil_sendRequestRebootMsg(msgHandle);	
      }
      break;   
	case CMS_IMAGE_FORMAT_CSV_CFG:
	{
		char filename[BUFLEN_1024]={0};

		//cmsFil_removeDir(SW_UPDATE_DATA_DIR);
		unlink("/tmp/importPhonebook.csv");
		//cmsFil_makeDir(SW_UPDATE_DATA_DIR);
		cmsLog_debug("phonebook csv import");

		snprintf(filename, sizeof(filename)-1, "/tmp/importPhonebook.csv");
		cmsFil_writeBufferToFile(filename, (UINT8 *)imagePtr, imageLen);
		  /*
		   * modupdtd will unpack the sw package, and then send a graceful
		   * shutdown msg to smd.
		   */
		  //sendStartModupdtdMsg(msgHandle);
		  break;
	}
#endif  /*1*/
	  
#ifdef SUPPORT_MOD_SW_UPDATE
   case CMS_IMAGE_FORMAT_MODSW_PKG:
   {
      char filename[BUFLEN_1024]={0};

      cmsFil_removeDir(SW_UPDATE_DATA_DIR);
      cmsFil_makeDir(SW_UPDATE_DATA_DIR);

      snprintf(filename, sizeof(filename)-1, "%s/%s",
                         SW_UPDATE_DATA_DIR, SW_UPDATE_PKG_FILE);
      cmsFil_writeBufferToFile(filename, (UINT8 *)imagePtr, imageLen);
      /*
       * modupdtd will unpack the sw package, and then send a graceful
       * shutdown msg to smd.
       */
      sendStartModupdtdMsg(msgHandle);
      break;
   }
#endif

   default:
       cmsLog_error("Unrecognized image format=%d", format);
       ret = CMSRET_INVALID_IMAGE;
       break;
    }
   
   return ret;
}

#if 1 //LingChun
int cmsImg_pre_validateImage(const char *imageBuf, UINT32 imageLen, void *msgHandle)
{
	if((0 != imageLen) && (CMS_CONFIG_FILE_MAXIMUM_LENGTH <= imageLen))
	{
		cmsLog_error("The length of config file is too large.");
		return 1 ;
	}else
		return 0;
}
#endif
 
CmsImageFormat cmsImg_validateImage(const char *imageBuf, UINT32 imageLen, void *msgHandle)
{
   CmsImageFormat result = CMS_IMAGE_FORMAT_INVALID;
   CmsRet ret;
#if 1
/*[BUG FIX][#748]Define UTF8 header for exporting/importing phonebook CSV file including German,TO2 German, MSTC Walter,20130628*/
	char tmpUTF8PhonebookHeader[256];
	snprintf(tmpUTF8PhonebookHeader, sizeof(tmpUTF8PhonebookHeader),"%c%c%c%s",0xEF,0xBB,0xBF,IMPORT_PHONEBOOK_HEADER);
#endif   
   if (imageBuf == NULL)
   {
      return CMS_IMAGE_FORMAT_INVALID;
   }

/*[BUGFIXED]phonebook import fail if size of file(importPhonebook.csv) small than CMS_CONFIG_FILE_DETECTION_LENGTH
	KeyYang@MSTC 20130627*/
/*[BUG FIX][#748]Define UTF8 header for exporting/importing phonebook CSV file including German,TO2 German, MSTC Walter,20130628*/
#if 1
	if((0 != imageLen) &&
		(CMS_CONFIG_FILE_DETECTION_LENGTH >= imageLen) &&
		(TRUE == cmsImg_isConfigFileLikely(imageBuf)) &&
		((!strncmp(imageBuf, IMPORT_PHONEBOOK_HEADER, strlen(IMPORT_PHONEBOOK_HEADER)))
		||(!strncmp(imageBuf, tmpUTF8PhonebookHeader, strlen(tmpUTF8PhonebookHeader))))
		){
		cmsLog_debug("CVS config format verified.");
		return CMS_IMAGE_FORMAT_CSV_CFG ;
	}
#endif
   
   if (imageLen > CMS_CONFIG_FILE_DETECTION_LENGTH &&
       cmsImg_isConfigFileLikely(imageBuf))
   {
/*[IreneWang@MSTC:TO2]$ The output config file must be encrypted, 20130228*/
#if defined(COMPRESSED_CONFIG_FILE) && 1
   	  if ((!strncmp(imageBuf, COMPRESSED_CONFIG_HEADER, strlen(COMPRESSED_CONFIG_HEADER))))
   	  {
   	      cmsLog_debug("possible Compress CMS XML config file format detected");
	      ret = sendConfigMsg(imageBuf, imageLen, msgHandle, CMS_MSG_VALIDATE_CONFIG_FILE);
	      if (ret == CMSRET_SUCCESS)
	      { 
	         cmsLog_debug("CMS XML config format verified.");
	         return CMS_IMAGE_FORMAT_GUIXML_CFG;
	      }
   	  }
   	  if ((!strncmp(imageBuf, IMPORT_PHONEBOOK_HEADER, strlen(IMPORT_PHONEBOOK_HEADER)))
#if 1 
		/*[BUG FIX][#748]Define UTF8 header for exporting/importing phonebook CSV file including German,TO2 German, MSTC Walter,20130628*/
		||(!strncmp(imageBuf, tmpUTF8PhonebookHeader, strlen(tmpUTF8PhonebookHeader)))
#endif
	  )
   	  {
   	      cmsLog_debug("possible Compress CMS XML config file format detected");
	      //ret = sendConfigMsg(imageBuf, imageLen, msgHandle, CMS_MSG_VALIDATE_CONFIG_FILE);
	      //if (ret == CMSRET_SUCCESS)
	      //{ 
	        // cmsLog_debug("CMS XML config format verified.");
	         return CMS_IMAGE_FORMAT_CSV_CFG;
	      //}
   	  }	  
#endif  /*1*/
      cmsLog_debug("possible CMS XML config file format detected");
      ret = sendConfigMsg(imageBuf, imageLen, msgHandle, CMS_MSG_VALIDATE_CONFIG_FILE);
      if (ret == CMSRET_SUCCESS)
      { 
         cmsLog_error("CMS XML config format verified.");
         return CMS_IMAGE_FORMAT_XML_CFG;
      }
   } 
   
   cmsLog_debug("not a config file");
   
#ifdef SUPPORT_MOD_SW_UPDATE
   if (cmsImg_isModSwPkg((unsigned char *) imageBuf, imageLen))
   {
      cmsLog_debug("detected modular sw pkg format!");
      return CMS_IMAGE_FORMAT_MODSW_PKG;
   }

   cmsLog_debug("not a modular sw pkg");
#endif

/*__MTS__, RaynorChung: Support 963268 nand flash, patch form SVN#3597 on http://svn.zyxel.com.tw/svn/CPE_SW1/BCM96368/trunk/P-870HA/branches/cht/fttb8/4.11 */
#ifdef MTS_NAND_FLASH_SUPPORT
   if ((imageLen > sizeof(FILE_TAG)) 
        && (verifyBroadcomFileTag((FILE_TAG *) imageBuf) == CMSRET_SUCCESS))
#else
   if ((imageLen > sizeof(FILE_TAG)) 
        && (verifyBroadcomFileTag((FILE_TAG *) imageBuf, imageLen) == CMSRET_SUCCESS))
#endif /*MTS_NAND_FLASH_SUPPORT*/
   {
      UINT32 maxLen;
      
      /* Found a valid Broadcom defined TAG record at the beginning of the image */
      cmsLog_debug("Broadcom format verified.");
      maxLen = cmsImg_getImageFlashSize() + cmsImg_getBroadcomImageTagSize();
      if (imageLen > maxLen)
      {
         cmsLog_error("broadcom image is too large for flash, got %u, max %u", imageLen, maxLen);
      }
      else
      {
         result = CMS_IMAGE_FORMAT_BROADCOM;
      }
   }
   else
   {
      /* if It is not a Broadcom flash format file.  Now check if it is a
       * flash image format file.  A flash image format file must have a
       * CRC at the end of the image.
       */
      UINT32 crc = CRC_INITIAL_VALUE;
      UINT32 imageCrc;
      UINT8 *crcPtr;
      
      if (imageLen > TOKEN_LEN)
      {
         crcPtr = (UINT8 *) (imageBuf + (imageLen - TOKEN_LEN));
         /*
          * CRC may not be word aligned, so extract the bytes out one-by-one.
          * Whole image CRC is calculated, then htonl, then written out using
          * fwrite (see addvtoken.c in hostTools).  Because of the way we are
          * extracting the CRC here, we don't have to swap for endieness when
          * doing compares on desktop Linux and modem (?).
          */
         imageCrc = (crcPtr[0] << 24) | (crcPtr[1] << 16) | (crcPtr[2] << 8) | crcPtr[3];
      
         crc = cmsCrc_getCrc32((unsigned char *) imageBuf, imageLen - TOKEN_LEN, crc);      
         if (crc == imageCrc)
         {
            UINT32 maxLen;
          
            cmsLog_debug("Whole flash image format [%ld bytes] verified.", imageLen);
            maxLen = cmsImg_getImageFlashSize();
            if (imageLen > maxLen)
            {
               cmsLog_error("whole image is too large for flash, got %u, max %u", imageLen, maxLen);
            }
            else
            {
               result = CMS_IMAGE_FORMAT_FLASH;
            }
         }
         else
         {
#if defined(EPON_SDK_BUILD)
            cmsLog_debug("Could not determine image format [%d bytes]", imageLen);
#else
            cmsLog_error("Could not determine image format [%d bytes]", imageLen);
#endif
            cmsLog_debug("calculated crc=0x%x image crc=0x%x", crc, imageCrc);
         }
      }
   }

/*__MTS__, RaynorChung: Support 963268 nand flash, patch form SVN#3597 on http://svn.zyxel.com.tw/svn/CPE_SW1/BCM96368/trunk/P-870HA/branches/cht/fttb8/4.11 */
#ifdef MTS_NAND_FLASH_SUPPORT
   if ( result == CMS_IMAGE_FORMAT_BROADCOM ) {
      if ( verifyZyXELModelId((FILE_TAG *) imageBuf) != CMSRET_SUCCESS ) {
	     result = CMS_IMAGE_MODELID_INVALID;
      }
   }
#endif /*MTS_NAND_FLASH_SUPPORT*/
   cmsLog_debug("returning image format %d", result);

   return result;
}

CmsRet sendConfigMsg(const char *imagePtr, UINT32 imageLen, void *msgHandle, CmsMsgType msgType)
{
   char *buf=NULL;
   char *body=NULL;
   CmsMsgHeader *msg;
   CmsRet ret;

   if ((buf = cmsMem_alloc(sizeof(CmsMsgHeader) + imageLen, ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("failed to allocate %d bytes for msg 0x%x", 
                   sizeof(CmsMsgHeader) + imageLen, msgType);
      return CMSRET_RESOURCE_EXCEEDED;
   }
   
   msg = (CmsMsgHeader *) buf;
   body = (char *) (msg + 1);
    
   msg->type = msgType;
   msg->src = cmsMsg_getHandleEid(msgHandle);
   msg->dst = EID_SMD;
   msg->flags_request = 1;
   msg->dataLength = imageLen;
   
   memcpy(body, imagePtr, imageLen);

   ret = cmsMsg_sendAndGetReply(msgHandle, msg);
   
   cmsMem_free(buf);
   
   return ret;
}


#if 1	//irenetmp
/* WenJuin@MSTC: solution2: add/del policy in tr069c, 20140606 */
void processDNSRoute(UBOOL8 isAdd, char *host, void *msgHandle)
{
	FILE *fp=NULL;
	char line[512]={0};
	char *token=NULL;
	UBOOL8 found=FALSE;
	
	char *tmp_file;
	FILE* fs = NULL;
	char tmp[256]={0};
	CmsMsgHeader msg=EMPTY_MSG_HEADER;

	cmsLog_debug("start %s, host:%s", (isAdd==TRUE)?"Add":"Del", host);	

	if(cmsUtl_isValidIpAddress(AF_INET,host))
	{
		/* IP address case */
		if(isAdd == TRUE)
		{
			/* add route rule */
			sprintf(tmp, "ip route replace %s/32 dev %s", host, "ppp0.1");
			cmsLog_debug("[add route rule]cmd:%s\n",tmp);		  		  	
			system(tmp);	
		}else
		{
			/* del route rule */
			sprintf(tmp, "ip route del %s/32 dev %s", host, "ppp0.1");
			cmsLog_debug("[del route rule]cmd:%s\n",tmp);		  		  	
			system(tmp);	
		}	
	}
	else
	{
		if(isAdd == TRUE)
		{
			/* add case */
			/* Domain name case */
			if((fp = fopen("/var/fyi/sys/dnsRouteTable", "r")) == NULL)
			{
				cmsLog_error("could not open tmp file %s", "/var/fyi/sys/dnsRouteTable");
			}
			else
			{
				while(!found && fgets(line, sizeof(line), fp)!=NULL)
				{
					line[sizeof(line)-1]=0;

					token = strtok(line, ":");
					cmsLog_debug("%s", token);	

					if(!cmsUtl_strcmp(token, host))
					{
						found = TRUE;
						cmsLog_debug("found in DNS Route file.");
					}

					memset(line, 0, 512);
				}	
				fclose(fp);	
			}	

			if(!found)
			{
				if((fp = fopen("/var/fyi/sys/srvRouteTable", "r")) == NULL)
				{
					cmsLog_error("could not open tmp file %s", "/var/fyi/sys/srvRouteTable");
				}
				else
				{
					while(fgets(line, sizeof(line), fp)!=NULL)
					{
						line[sizeof(line)-1]=0;

						token = strtok(line, ":");
						cmsLog_debug("%s", token);	

						if(!cmsUtl_strcmp(token, host))
						{
							found = TRUE;
							cmsLog_debug("found in DNS Route file.");
						}

						memset(line, 0, 512);
					}	
					fclose(fp);	
				}
			
			}

			if(!found)
			{
				if((fp = fopen("/var/fyi/sys/tmpRouteTable", "r")) == NULL)
				{
					cmsLog_error("could not open tmp file %s", "/var/fyi/sys/tmpRouteTable");
				}
				else
				{
					while(fgets(line, sizeof(line), fp)!=NULL)
					{
						line[sizeof(line)-1]=0;

						token = strtok(line, ":");
						cmsLog_debug("%s", token);	

						if(!cmsUtl_strcmp(token, host))
						{
							found = TRUE;
							cmsLog_debug("found in DNS Route file.");
						}

						memset(line, 0, 512);
					}	
					fclose(fp);	
				}						
			}			
				
			if(found == FALSE)
			{
				/* add host to file */
				if((fs = fopen("/var/fyi/sys/tmpRouteTable", "a+")) == NULL)
				{
				   cmsLog_error("could not open tmp file %s", "/var/fyi/sys/tmpRouteTable");
				   return;
				}
				
				/* not found in file, add host to tmp file */
				cmsLog_debug("Add case and not found in file, add host to tmp file.");
				sprintf(tmp, "%s:%s:%s:%s;\n", host, "ppp0.1", "255.255.255.255", "Msg");			
				fputs(tmp, fs);

				fclose(fs);
			}				
		}
		else
		{
			/* del case */
			if((fp = fopen("/var/fyi/sys/tmpRouteTable", "r")) == NULL)
			{
				cmsLog_error("could not open tmp file %s", "/var/fyi/sys/tmpRouteTable");
			}
			else
			{
				/*
				 * Record the Routing Interface when special domain name dns query
				 */
				tmp_file = tempnam("/var/fyi/sys", "tmpRouteTable");
				
				if ((fs = fopen(tmp_file, "w+")) == NULL)
				{
				   cmsLog_error("could not open tmp file %s", tmp_file);

				   fclose(fp);
				   return;
				}
				
				while(fgets(line, sizeof(line), fp)!=NULL)
				{
					cmsUtl_strncpy(tmp, line, sizeof(line)+1);
					line[sizeof(line)-1]=0;

					token = strtok(line, ":");
					cmsLog_debug("%s", token);	

					if(!cmsUtl_strcmp(token, host))
					{
						found = TRUE;
						cmsLog_debug("found in tmp DNS Route file, ignore this Entry.");
					}else
					{
						/* copy to tmp file */
						fputs(tmp, fs);
					}
					
					memset(line, 0, 512);
					memset(tmp, 0, 256);
				}	
				
				fclose(fp);
				fclose(fs);		 
				rename(tmp_file, "/var/fyi/sys/tmpRouteTable");	
			}			
		
		}	
	}


	/* Notify dproxy reload policy route table */
	msg.dst = EID_DNSPROXY;
	msg.src = cmsMsg_getHandleEid(msgHandle);
	msg.type = CMS_MSG_DNSPROXY_POLICYROUTE_RELOAD;
	msg.flags_request = 1;
	msg.wordData = 0;
	msg.dataLength = 0; 	

	/*
	* During dhcp stress tests, this function is called a lot.  So don't
	* bother waiting for a reply from dnsproxy.
	*/
	if ((cmsMsg_send(msgHandle, &msg)) != CMSRET_SUCCESS)
	{
		cmsLog_error("could not send CMS_MSG_DNSPROXY_RELOAD msg to dnsproxy");
		return;
	}
	else
	{
		cmsLog_debug("CMS_MSG_DNSPROXY_RELOAD sent successfully");
	}
	
	return;
}
#else
/* Irene@MSTC: dynamic policy routing, 20140606 */
void cmsImg_sendDNSRouteAddMsg(void *msgHandle, const char *host)
{
   CmsMsgHeader *msg;
   char *data;
   void *msgBuf;
   UINT32 msgDataLen=0;

   /* for the msg and the connIfName */
   if (host)
   {
      msgDataLen = strlen(host) + 1;
      msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader) + msgDataLen, ALLOC_ZEROIZE);
   } 
   else
   {
      cmsLog_error("msg without host");
      msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader), ALLOC_ZEROIZE);
   }
   
   msg = (CmsMsgHeader *)msgBuf;
   msg->src = cmsMsg_getHandleEid(msgHandle);
   msg->dst = EID_DNSPROXY;
   msg->flags_request = 1;
   msg->type = CMS_MSG_DNS_ROUTE_ADD;

   if (host)
   {
      data = (char *) (msg + 1);
      msg->dataLength = msgDataLen;
      memcpy(data, (char *)host, msgDataLen);      
   }
   
   cmsLog_debug("host=%s", host);

   cmsMsg_sendAndGetReplyWithTimeout(msgHandle, msg, 3000);

   CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);
}

void cmsImg_sendDNSRouteDeleteMsg(void *msgHandle, const char *host)
{
   CmsMsgHeader *msg;
   char *data;
   void *msgBuf;
   UINT32 msgDataLen=0;

   /* for the msg and the connIfName */
   if (host)
   {
      msgDataLen = strlen(host) + 1;
      msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader) + msgDataLen, ALLOC_ZEROIZE);
   } 
   else
   {
      cmsLog_error("msg without host");
      msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader), ALLOC_ZEROIZE);
   }
   
   msg = (CmsMsgHeader *)msgBuf;
   msg->src = cmsMsg_getHandleEid(msgHandle);
   msg->dst = EID_DNSPROXY;
   msg->flags_request = 1;
   msg->type = CMS_MSG_DNS_ROUTE_DELETE;

   if (host)
   {
      data = (char *) (msg + 1);
      msg->dataLength = msgDataLen;
      memcpy(data, (char *)host, msgDataLen);      
   }
   
   cmsLog_debug("host=%s", host);

   cmsMsg_sendAndGetReplyWithTimeout(msgHandle, msg, 3000);

   CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);
}
#endif

void cmsImg_sendLoadStartingMsg(void *msgHandle, const char *connIfName)
{
   CmsMsgHeader *msg;
   char *data;
   void *msgBuf;
   UINT32 msgDataLen=0;

   /* for the msg and the connIfName */
   if (connIfName)
   {
      msgDataLen = strlen(connIfName) + 1;
      msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader) + msgDataLen, ALLOC_ZEROIZE);
   } 
   else
   {
      cmsLog_error("msg without connIfName");
      msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader), ALLOC_ZEROIZE);
   }
   
   msg = (CmsMsgHeader *)msgBuf;
   msg->src = cmsMsg_getHandleEid(msgHandle);
   msg->dst = EID_SMD;
   msg->flags_request = 1;
   msg->type = CMS_MSG_LOAD_IMAGE_STARTING;

   if (connIfName)
   {
      data = (char *) (msg + 1);
      msg->dataLength = msgDataLen;
      memcpy(data, (char *)connIfName, msgDataLen);      
   }
   
   cmsLog_debug("connIfName=%s", connIfName);

   cmsMsg_sendAndGetReply(msgHandle, msg);

   CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);

}


void cmsImg_sendLoadDoneMsg(void *msgHandle)
{
   CmsMsgHeader msg = EMPTY_MSG_HEADER;

   msg.type = CMS_MSG_LOAD_IMAGE_DONE;
   msg.src = cmsMsg_getHandleEid(msgHandle);
   msg.dst = EID_SMD;
   msg.flags_request = 1;
   
   cmsMsg_sendAndGetReply(msgHandle, &msg);
}


#ifdef SUPPORT_MOD_SW_UPDATE
void sendStartModupdtdMsg(void *msgHandle)
{
   CmsMsgHeader msg = EMPTY_MSG_HEADER;

   msg.type = CMS_MSG_START_APP;
   msg.src = cmsMsg_getHandleEid(msgHandle);
   msg.dst = EID_SMD;
   msg.wordData = EID_MODUPDTD;
   msg.flags_request = 1;

   cmsMsg_sendAndGetReply(msgHandle, &msg);
}
#endif

void cmsUtil_sendCleanCallListMsg(void *msgHandle)
{
   CmsRet ret =CMSRET_SUCCESS ;
   
   CmsMsgHeader msg = EMPTY_MSG_HEADER;

   msg.type = CMS_MSG_VOICE_CALLMNGR_RESTORE_TO_DEFAULT_REQUEST;
   msg.src = cmsMsg_getHandleEid(msgHandle);
   msg.dst = EID_CALLMNGR_BSPAL;
   msg.flags_request = 1;

   if(CMSRET_SUCCESS != (ret = cmsMsg_send(msgHandle, &msg))){
		cmsLog_error("could not send CMS_MSG_VOICE_CALLMNGR_RESTORE_TO_DEFAULT_REQUEST to CallMngr Bspal!");
	}

}

void cmsUtil_sendCleanCallQuaMsg(void *msgHandle)
{
   CmsRet ret =CMSRET_SUCCESS ;
   
	unsigned char tmpBuf[sizeof(CmsMsgHeader) + sizeof(128)] = {0} ;
	CmsMsgHeader* msg = (CmsMsgHeader *)tmpBuf ;
	char Quafile[128]="";

	memset(&Quafile, 0, sizeof(128)) ;
	strcpy(Quafile,"/tmp/voiceQua.csv");
	memcpy((msg+1), &Quafile, sizeof(Quafile)) ;


   msg->type = CMS_MSG_VOICE_CALLMNGR_CALL_QUALITY_REPORT_REQUEST;
   msg->src = cmsMsg_getHandleEid(msgHandle);
   msg->dst = EID_CALLMNGR_BSPAL;
   msg->flags_request = 1;

   if(CMSRET_SUCCESS != (ret = cmsMsg_send(msgHandle, msg))){
		cmsLog_error("could not send CMS_MSG_VOICE_CALLMNGR_CALL_QUALITY_REPORT_REQUEST to CallMngr Bspal!");
	}

}


/* using a cmsUtil_ prefix because this can be used in non-upload scenarios */
void cmsUtil_sendRequestRebootMsg(void *msgHandle)
{
   CmsMsgHeader msg = EMPTY_MSG_HEADER;

   msg.type = CMS_MSG_REBOOT_SYSTEM;
   msg.src = cmsMsg_getHandleEid(msgHandle);
   #if 0 /*MSTC, chris */
   msg.dst = EID_SMD;
   #else
   msg.dst = EID_AUTODISCONNECT;
   #endif
   msg.flags_request = 1;

   cmsMsg_sendAndGetReply(msgHandle, &msg);
}


CmsRet cmsImg_saveIfNameFromSocket(SINT32 socketfd, char *connIfName)
{
  
   SINT32 i = 0;
   SINT32 fd = 0;
   SINT32 numifs = 0;
   UINT32 bufsize = 0;
   struct ifreq *all_ifr = NULL;
   struct ifconf ifc;
   struct sockaddr local_addr;
   socklen_t local_len = sizeof(struct sockaddr_in);

   if (socketfd < 0 || connIfName == NULL)
   {
      cmsLog_error("cmsImg_saveIfNameFromSocket: Invalid parameters: socket=%d, connIfName=%s", socketfd, connIfName);
      return CMSRET_INTERNAL_ERROR;
   }
   memset(&ifc, 0, sizeof(struct ifconf));
   memset(&local_addr, 0, sizeof(struct sockaddr));
   
   if (getsockname(socketfd, &local_addr,&local_len) < 0) 
   {
      cmsLog_error("cmsImg_saveIfNameFromSocket: Error in getsockname.");
      return CMSRET_INTERNAL_ERROR;
   }

   /* cmsLog_error("cmsImg_saveIfNameFromSocket: Session comes from: %s", inet_ntoa(((struct sockaddr_in *)&local_addr)->sin_addr)); */
   
   if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
   {
      cmsLog_error("cmsImg_saveIfNameFromSocket: Error openning socket when getting socket intface info");
      return CMSRET_INTERNAL_ERROR;
   }
   
   numifs = 16;

   bufsize = numifs*sizeof(struct ifreq);
   all_ifr = (struct ifreq *)cmsMem_alloc(bufsize, ALLOC_ZEROIZE);
   if (all_ifr == NULL) 
   {
      cmsLog_error("cmsImg_saveIfNameFromSocket: out of memory");
      close(fd);
      return CMSRET_INTERNAL_ERROR;
   }

   ifc.ifc_len = bufsize;
   ifc.ifc_buf = (char *)all_ifr;
   if (ioctl(fd, SIOCGIFCONF, &ifc) < 0) 
   {
      cmsLog_error("cmsImg_saveIfNameFromSocket: Error getting interfaces\n");
      close(fd);
      cmsMem_free(all_ifr);
      return CMSRET_INTERNAL_ERROR;
   }

   numifs = ifc.ifc_len/sizeof(struct ifreq);
   /* cmsLog_error("cmsImg_saveIfNameFromSocket: numifs=%d\n",numifs); */
   for (i = 0; i < numifs; i ++) 
   {
	  /* cmsLog_error("cmsImg_saveIfNameFromSocket: intface name=%s\n", all_ifr[i].ifr_name); */
	  struct in_addr addr1,addr2;
	  addr1 = ((struct sockaddr_in *)&(local_addr))->sin_addr;
	  addr2 = ((struct sockaddr_in *)&(all_ifr[i].ifr_addr))->sin_addr;
	  if (addr1.s_addr == addr2.s_addr) 
	  {
	      strcpy(connIfName, all_ifr[i].ifr_name);
	  	break;
	  }
   }

   close(fd);
   cmsMem_free(all_ifr);

   cmsLog_debug("connIfName=%s", connIfName);

   return CMSRET_SUCCESS;
   
}
#if 1	/*Jayden added for disable bspl led control,MitraStar,20131112*/
void sendEventtoBSPALMsg(void *msgHandle)
{
   CmsMsgHeader msg = EMPTY_MSG_HEADER;
   CmsRet ret= CMSRET_INTERNAL_ERROR;
   msg.type = CMS_MSG_RESTOREDEFAULT_SENDTO_BSPAL;
   msg.src = cmsMsg_getHandleEid(msgHandle);
   msg.dst = EID_CALLMNGR_BSPAL;
   msg.flags_event = 1;
   msg.flags_request = 0;
   msg.wordData = CMSRET_SUCCESS;
   if ((ret = cmsMsg_send(msgHandle, &msg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("msg toBsplMsg send failed, ret=%d",ret);
   }
}
#endif

