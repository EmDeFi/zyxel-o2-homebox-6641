/* static_leases.h */
#ifndef _STATIC_LEASES_H
#define _STATIC_LEASES_H

#include "dhcpd.h"

/* Config file will pass static lease info to this function which will add it
 * to a data structure that can be searched later */
int addStaticLease(struct static_lease **lease_struct, uint8_t *mac, uint32_t *ip);
#ifdef MSTC_TO2_GERMANY_CUSTOMIZATION /*For To2 fix ipaddress by device name, Mitrastar ChingLung, 20130301*/
int addStaticLeaseDevname(struct static_lease_devname **lease_struct, char *devname, uint32_t *ip);
#endif
/* Check to see if a mac has an associated static lease */
uint32_t getIpByMac(struct static_lease *lease_struct, void *arg);

/* Check to see if an ip is reserved as a static ip */
uint32_t reservedIp(struct static_lease *lease_struct, uint32_t ip);

#ifdef UDHCP_DEBUG
/* Print out static leases just to check what's going on */
void printStaticLeases(struct static_lease **lease_struct);
/*For To2 fix ipaddress by device name, Mitrastar ChingLung, 20130301*/
void printStaticLeasesDevname(struct static_lease_devname**lease_struct);
#endif

#endif



