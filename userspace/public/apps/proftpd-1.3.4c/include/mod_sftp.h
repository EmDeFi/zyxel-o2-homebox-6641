/* mod_sftp.h.  Generated from mod_sftp.h.in by configure.  */
/*
 * ProFTPD - mod_sftp
 * Copyright (c) 2008-2011 TJ Saunders
 *
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
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA 02110-1335, USA.
 *
 * As a special exemption, TJ Saunders and other respective copyright holders
 * give permission to link this program with OpenSSL, and distribute the
 * resulting executable, without including the source code for OpenSSL in the
 * source distribution.
 *
 * $Id: mod_sftp.h.in,v 1.22 2011/10/12 17:15:56 castaglia Exp $
 */

#ifndef MOD_SFTP_H
#define MOD_SFTP_H

#include "conf.h"
#include "privs.h"

#include <signal.h>

#if HAVE_MLOCK
# include <sys/mman.h>
#endif

#if HAVE_SYS_UIO_H
# include <sys/uio.h>
#endif

#ifndef MAX
# define MAX(x, y) (((x) > (y)) ? (x) : (y))
# define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

/* Define if you have OpenSSL with crippled AES support. */
/* #undef HAVE_AES_CRIPPLED_OPENSSL */

/* Define if you have OpenSSL with SHA256 support. */
#define HAVE_SHA256_OPENSSL 1

/* Define if you have OpenSSL with SHA512 support. */
#define HAVE_SHA512_OPENSSL 1

#define MOD_SFTP_VERSION	"mod_sftp/0.9.8"

/* Make sure the version of proftpd is as necessary. */
#if PROFTPD_VERSION_NUMBER < 0x0001030402
# error "ProFTPD 1.3.4rc2 or later required"
#endif

#include <openssl/bio.h>
#include <openssl/blowfish.h>
#include <openssl/bn.h>
#include <openssl/des.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/pem.h>
#include <openssl/dsa.h>
#include <openssl/rsa.h>
#if OPENSSL_VERSION_NUMBER > 0x000907000L
# include <openssl/aes.h>
# include <openssl/engine.h>
# include <openssl/ocsp.h>
#endif

#include <zlib.h>

#define SFTP_ID_STRING	"SSH-2.0-" MOD_SFTP_VERSION

/* mod_sftp session state flags */
#define SFTP_SESS_STATE_HAVE_KEX	0x00001
#define SFTP_SESS_STATE_HAVE_SERVICE	0x00002
#define SFTP_SESS_STATE_HAVE_AUTH	0x00004
#define SFTP_SESS_STATE_REKEYING	0x00008

/* mod_sftp option flags */
#define SFTP_OPT_IGNORE_SFTP_UPLOAD_PERMS	0x0001
#define SFTP_OPT_IGNORE_SCP_UPLOAD_PERMS	0x0002
#define SFTP_OPT_PESSIMISTIC_KEXINIT		0x0004
#define SFTP_OPT_OLD_PROTO_COMPAT		0x0008
#define SFTP_OPT_MATCH_KEY_SUBJECT		0x0010
#define SFTP_OPT_IGNORE_SFTP_SET_PERMS		0x0020
#define SFTP_OPT_IGNORE_SFTP_SET_TIMES		0x0040

/* mod_sftp service flags */
#define SFTP_SERVICE_FL_SFTP		0x0001
#define SFTP_SERVICE_FL_SCP		0x0002
#define SFTP_SERVICE_FL_DATE		0x0004

#define SFTP_SERVICE_DEFAULT \
	(SFTP_SERVICE_FL_SFTP|SFTP_SERVICE_FL_SCP)

/* Miscellaneous */
extern int sftp_logfd;
extern const char *sftp_logname;
extern pool *sftp_pool;
extern conn_t *sftp_conn;
extern unsigned int sftp_sess_state;
extern unsigned long sftp_opts;
extern unsigned int sftp_services;

/* Used by other SFTP modules (e.g. mod_sftp_pam) for sending USERAUTH_BANNER
 * messages to the client.
 */
int sftp_auth_send_banner(const char *);

/* API for modules that which to register "keyboard-interactive" auth
 * drivers (see RFC4256).
 */

typedef struct {
  const char *challenge;

  /* TRUE or FALSE, depending on whether the user's response should be
   * displayed back to them (TRUE), or hidden (FALSE).
   */
  char display_response;

} sftp_kbdint_challenge_t;

typedef struct kbdint_st {
  const char *driver_name;

  /* Memory pool for this driver */
  pool *driver_pool;

  /* Arbitrary driver-specific data */
  void *driver_data;

  /* Open the driver for authentication. The user argument is the name of the
   * user being authenticated.  Returns zero on success, -1 otherwise
   * (with errno set appropriately).
   */
  int (*open)(struct kbdint_st *driver, const char *user);

  /* Authenticate the given user.  The driver can use the following
   * functions for writing and reading the driver challenge/response data
   * to and from the client:
   *
   *   sftp_kbdint_send_challenge()
   *   sftp_kbdint_read_response() 
   *
   * The array_header argument to the read_response() function will be
   * populated with strings from the client.
   *
   * Returns zero on success, -1 otherwise (with errno set appropriately).
   */
  int (*authenticate)(struct kbdint_st *driver, const char *user);

  /* Close the driver, clean up any allocated resources.  Returns
   * zero on success, -1 otherwise (with errno set appropriately).
   */
  int (*close)(struct kbdint_st *driver);

} sftp_kbdint_driver_t;

int sftp_kbdint_register_driver(const char *name, sftp_kbdint_driver_t *driver);
int sftp_kbdint_unregister_driver(const char *name);
int sftp_kbdint_send_challenge(const char *, const char *, unsigned int,
  sftp_kbdint_challenge_t *);
int sftp_kbdint_recv_response(pool *, unsigned int *, const char ***);

/* API for modules that which to register keystores, for the
 * SFTPAuthorizedHostKeys and SFTPAuthorizedUserKeys directives.
 */

typedef struct keystore_st {
  /* Memory pool for this keystore. */
  pool *keystore_pool;

  /* Arbitrary keystore-specific data. */
  void *keystore_data;

  /* The type of keys (host, user) that this store can handle. */
  unsigned int store_ktypes;

  /* Verify the given host key.  Returns zero on success, -1 otherwise. */
  int (*verify_host_key)(struct keystore_st *store, pool *p, const char *user,
    const char *host_fqdn, const char *host_user, char *host_key,
    uint32_t host_keylen);

  /* Verify the given user key.  Returns zero on success, -1 otherwise. */
  int (*verify_user_key)(struct keystore_st *store, pool *p, const char *user,
    char *user_key, uint32_t user_keylen);

  /* Close this keystore, clean up any allocated resources.  Returns
   * zero on success, -1 otherwise (with errno set appropriately).
   */
  int (*store_close)(struct keystore_st *store);

} sftp_keystore_t;

#define SFTP_SSH2_HOST_KEY_STORE	0x01
#define SFTP_SSH2_USER_KEY_STORE	0x02

int sftp_keystore_register_store(const char *,
  sftp_keystore_t *(*store_open)(pool *, int, const char *, const char *),
  unsigned int);
int sftp_keystore_unregister_store(const char *, unsigned int);

/* For use by keystore backend modules. */
int sftp_keys_compare_keys(pool *, char *, uint32_t, char *, uint32_t);

/* These strings are part of any RFC4716 key; thus they will be needed by
 * any keystore backend modules.
 */
#define SFTP_SSH2_PUBKEY_BEGIN_MARKER   "---- BEGIN SSH2 PUBLIC KEY ----"
#define SFTP_SSH2_PUBKEY_END_MARKER     "---- END SSH2 PUBLIC KEY ----"

/* For use by other SFTP modules. */
const char *sftp_crypto_get_errors(void);

/* For use by other modules to register 'exec' channel request handlers.
 *
 * Note that the implementation currently assumes that the registering module
 * is registering its callbacks from a session process, i.e. post-fork(2).
 * That is why there is no corresponding unregister function at present.
 */
int sftp_channel_register_exec_handler(module *, const char *,
  int (*set_params)(pool *, uint32_t, array_header *),
  int (*prep_chan)(uint32_t),
  int (*postopen_chan)(uint32_t),
  int (*handle_pkt)(pool *, void *, uint32_t, char *, uint32_t),
  int (*finish_chan)(uint32_t),
  int (**write_data)(pool *, uint32_t, char *, uint32_t));

#endif
