#ifndef _SERVERPACKET_H
#define _SERVERPACKET_H


int sendOffer(struct dhcpMessage *oldpacket);
#ifdef MSTC_COMMON_PATCH // __Verizon__, Hong-Yu
struct dhcpOfferedAddr *sendNAK(struct dhcpMessage *oldpacket);
#else
int sendNAK(struct dhcpMessage *oldpacket);
#endif
int sendACK(struct dhcpMessage *oldpacket, u_int32_t yiaddr);
int send_inform(struct dhcpMessage *oldpacket);


#endif
