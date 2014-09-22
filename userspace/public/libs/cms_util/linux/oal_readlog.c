/***********************************************************************
 *
 *  Copyright (c) 2006  Broadcom Corporation
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


#include <stdlib.h>
#include <sys/shm.h> /* for shmat */
#include <sys/sem.h> /* for struct sembuf */
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
/*__MTS__, TengChang Chen, Log, migrate from Common_406*/
#include <fcntl.h>
#include <stddef.h>
#endif /*MSTC_COMMON_PATCH*/
#include "../oal.h"


/*
 * Some of these defines are duplicates of those in busybox/sysklogd, so
 * look there before changing anything here.
 */

static const long KEY_ID = 0x414e4547; /*"GENA"*/
static struct shbuf_ds {
	int size;		// size of data written
	// int head;		// start of message list
	int tail;		// end of message list
	char data[1];		// data/messages
} *buf = NULL;			// shared memory pointer

/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
/*__MTS__, TengChang Chen, Log, migrate from Common_406*/
struct shbuf_ds *sysBuf = NULL;
#endif /*MSTC_COMMON_PATCH*/

// Semaphore operation structures
static struct sembuf SMrup[1] = {{0, -1, IPC_NOWAIT | SEM_UNDO}}; // set SMrup
static struct sembuf SMrdn[2] = {{1, 0, 0}, {0, +1, SEM_UNDO}}; // set SMrdn
static int	log_shmid = -1;	// ipc shared memory id
static int	log_semid = -1;	// ipc semaphore id

#define BCM_SYSLOG_MESSAGE_LEN_BYTES    4


/*
 * sem_up - up()'s a semaphore.
 */
static inline void sem_up(int semid)
{
	if ( semop(semid, SMrup, 1) == -1 ) 
		cmsLog_error("semop[SMrup]");
}

/*
 * sem_down - down()'s a semaphore
 */				
static inline void sem_down(int semid)
{
	if ( semop(semid, SMrdn, 2) == -1 )
		cmsLog_error("semop[SMrdn]");
}

/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
/*__MTS__, TengChang Chen, Log, migrate from Common_406*/
/* try to open up the specified device */
int device_open(const char *device, int mode)
{
	int m, f, fd = -1;

	m = mode | O_NONBLOCK;

	/* Retry up to 5 times */
	for (f = 0; f < 5; f++)
		if ((fd = open(device, m, 0600)) >= 0)
			break;
	if (fd < 0)
		return fd;
	/* Reset original flags. */
	if (m != mode)
		fcntl(fd, F_SETFL, mode);
	return fd;
}

int oal_readLogFromFile(char* filename, int offset, char* buffer)
{
	int fd;
	FILE *stream;
	struct flock fl;
	int nextOffset;
	int ret;

	buffer[0] = '\0';

	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 1;

	if ((fd =
				device_open(filename, O_RDONLY | O_NONBLOCK )) >= 0) {
		fl.l_type = F_RDLCK;
		fcntl(fd, F_SETLKW, &fl);

		stream = fdopen (fd, "r");
		if (!stream){
			cmsLog_error("Can not open log file");
			ret = BCM_SYSLOG_READ_BUFFER_ERROR;
			goto close_fd_exit;
		}
		if(offset == BCM_SYSLOG_FIRST_READ)
			fseek(stream, 0, SEEK_SET);
		else
			fseek(stream, offset, SEEK_SET);
		if (!fgets (buffer, 1024, stream)){
			if (feof(stream)) {
				buffer[0] = '\0';
				fclose(stream);
				ret = BCM_SYSLOG_READ_BUFFER_ERROR;
				goto close_fd_exit;
			}
			cmsLog_error("Read log message from file fail");
			fclose(stream);
			ret = BCM_SYSLOG_READ_BUFFER_ERROR;
			goto close_fd_exit;
		}
		nextOffset = ftell(stream);
		fclose(stream);
		ret = nextOffset;
		goto close_fd_exit;
	}
	else{
		cmsLog_notice("Open log file fail");
		return BCM_SYSLOG_READ_BUFFER_ERROR;
	}

close_fd_exit:
		fl.l_type = F_UNLCK;
		fcntl(fd, F_SETLKW, &fl);
		close(fd);
		return ret;
}

int oal_readLogPartial_MTS(int ptr, char* buffer, int logType)
{
  int i=BCM_SYSLOG_READ_BUFFER_ERROR;
  int len;
  int end=0;
  const char *shbuf_data;
  unsigned head;

  if ( (log_shmid = shmget(KEY_ID, 0, 0)) == -1) {
    cmsLog_debug("Syslog disabled or log buffer not allocated\n");
    goto output_end;
  }
  // Attach shared memory to our char*
  if ( (buf = shmat(log_shmid, NULL, SHM_RDONLY)) == NULL) {
    cmsLog_error("Can't get access to circular buffer from syslogd\n");
    end = 1;
    goto output_end;
  }
  
  if ( (log_semid = semget(KEY_ID, 0, 0)) == -1) {
    cmsLog_error("Can't get access to semaphone(s) for circular buffer from syslogd\n");
    end = 1;
    goto output_end;
  }
  
  sem_down(log_semid);
  /*__MTS__, TengChang, separate broadcom log, system log, security log in circular buffer*/
  struct shbuf_ds *nextHead = NULL;
  
  sysBuf = buf;
  nextHead = (struct shbuf_ds *)((char*)sysBuf + sysBuf->size + offsetof(struct shbuf_ds, data) + 1);
   
  if(logType == SYSTEM_TYPE)
    buf = sysBuf;

  // Read Memory
  if (ptr == BCM_SYSLOG_FIRST_READ){
    shbuf_data = buf->data; /* pointer! */
    head = buf->tail;
    head += strlen(shbuf_data);
    i = head;
  }
  else
  {
    i = ptr;
  }

  if (head == buf->tail) {
    cmsLog_debug("<empty syslog buffer>\n");
    i = BCM_SYSLOG_READ_BUFFER_END;
    end = 1;
    goto nothing2display;
  }

readnext:
  if ( i != buf->tail) {
    if (i >= buf->size )
      i = 0;
    snprintf(buffer, BCM_SYSLOG_MAX_LINE_SIZE, "%s", buf->data+i);
    i += strlen(buf->data+i) + 1;
    if (i >= buf->size )
      i = 0;
    len = strlen(buffer);
    if (!((buffer[len] == '\0') &&
      (buffer[len-1] == '\n'))) {
        snprintf(&buffer[len], BCM_SYSLOG_MAX_LINE_SIZE-len, "%s", buf->data+i);
        len = strlen(buffer);
        i += strlen(buf->data+i) + 1;
        if (i >= buf->size )
          i = 0;
      }
    /* work around for syslogd.c bug which generate first log without timestamp */
    /*__MTS__, TengChang, Modify format of timestamp*/
    if (strlen(buffer) < 20 || buffer[4] != '-' || buffer[7] != '-' ||
      buffer[10] != 'T' || buffer[13] != ':' || buffer[16] != ':') {
        goto readnext;
      }
  }
  else {
    /* read to the end already */
    i = BCM_SYSLOG_READ_BUFFER_END;
    end = 1;
  }

nothing2display:
  sem_up(log_semid);

output_end:
  if (log_shmid != -1)
    shmdt(buf);

  if (end) {
    i=BCM_SYSLOG_READ_BUFFER_END;
    buffer[0]='\0';
  }
  return i;
}
#endif /*MSTC_COMMON_PATCH*/

int oal_readLogPartial(int ptr, char* buffer)
{
  int i=BCM_SYSLOG_READ_BUFFER_ERROR;
  int len;
  int end=0;
  const char *shbuf_data;
  unsigned head;

  if ( (log_shmid = shmget(KEY_ID, 0, 0)) == -1) {
    cmsLog_debug("Syslog disabled or log buffer not allocated\n");
    goto output_end;
  }
  // Attach shared memory to our char*
  if ( (buf = shmat(log_shmid, NULL, SHM_RDONLY)) == NULL) {
    cmsLog_error("Can't get access to circular buffer from syslogd\n");
    end = 1;
    goto output_end;
  }
  
  if ( (log_semid = semget(KEY_ID, 0, 0)) == -1) {
    cmsLog_error("Can't get access to semaphone(s) for circular buffer from syslogd\n");
    end = 1;
    goto output_end;
  }
  
  sem_down(log_semid);
  // Read Memory
  if (ptr == BCM_SYSLOG_FIRST_READ){
    shbuf_data = buf->data; /* pointer! */
    head = buf->tail;
    head += strlen(shbuf_data);
    i = head;
  }
  else
  {
    i = ptr;
  }

  if (head == buf->tail) {
    cmsLog_debug("<empty syslog buffer>\n");
    i = BCM_SYSLOG_READ_BUFFER_END;
    end = 1;
    goto nothing2display;
  }

readnext:
  if ( i != buf->tail) {
    if (i >= buf->size )
      i = 0;
    snprintf(buffer, BCM_SYSLOG_MAX_LINE_SIZE, "%s", buf->data+i);
    i += strlen(buf->data+i) + 1;
    if (i >= buf->size )
      i = 0;
    len = strlen(buffer);
    if (!((buffer[len] == '\0') &&
      (buffer[len-1] == '\n'))) {
        snprintf(&buffer[len], BCM_SYSLOG_MAX_LINE_SIZE-len, "%s", buf->data+i);
        len = strlen(buffer);
        i += strlen(buf->data+i) + 1;
        if (i >= buf->size )
          i = 0;
      }
    /* work around for syslogd.c bug which generate first log without timestamp */
/*KeyYang@MSTC: Merge from Telefonica_Common, 20120712*/
#ifdef MSTC_COMMON_PATCH
    if (strlen(buffer) < 20 || buffer[4] != '-' || buffer[7] != '-' ||
      buffer[10] != 'T' || buffer[13] != ':' || buffer[16] != ':') {
        goto readnext;
      }
#else /*MSTC_COMMON_PATCH*/
    if (strlen(buffer) < 16 || buffer[3] != ' ' || buffer[6] != ' ' ||
      buffer[9] != ':' || buffer[12] != ':' || buffer[15] != ' ') {
        goto readnext;
      }
    buffer[len-BCM_SYSLOG_MESSAGE_LEN_BYTES-1] = '\n';
    buffer[len-BCM_SYSLOG_MESSAGE_LEN_BYTES] = '\0';
#endif /*MSTC_COMMON_PATCH*/
  }
  else {
    /* read to the end already */
    i = BCM_SYSLOG_READ_BUFFER_END;
    end = 1;
  }

nothing2display:
  sem_up(log_semid);

output_end:
  if (log_shmid != -1)
    shmdt(buf);

  if (end) {
    i=BCM_SYSLOG_READ_BUFFER_END;
    buffer[0]='\0';
  }
  return i;
}

