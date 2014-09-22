/* files.h */
#ifndef _FILES_H
#define _FILES_H

int read_config(char *file);
void write_leases(int dummy);
void read_leases(char *file);
// BRCM
#ifdef MTS_Telefonica_TR69 /*Telefonica, Jason, 20100611*/
void send_VIoption_info(UBOOL8 isDelete, struct dhcpOfferedAddr *lease);
#endif
void send_lease_info(UBOOL8 isDelete, const struct dhcpOfferedAddr *lease);
int read_vendor_id_config(char *file);
void write_decline(void);
void write_viTable(int dummy);

#ifdef MSTC_COMMON_PATCH // __QWEST__, Hong-Yu
void getArpInfo(void);
void freeArpInfo(void);
void send_flash_update(void);
#endif

#ifdef DHCP_RELAY
void set_relays(void);
#endif


#endif /* _FILES_H */
