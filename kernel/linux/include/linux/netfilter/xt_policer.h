#ifndef _XT_RATE_H
#define _XT_RATE_H

#define BITS_PER_BYTE 8
#define KILO_SCALE 1000

struct xt_policerinfo {
    /*__ZyXEL__, Jones For compilation*/
    int policerMode;
	
    /* For srTCM and trTCM, rate means cRate and burst means cbsBurst.
    For srTCM, pbsBurst means ebsBurst. */
    u_int32_t rate, pRate;    
    u_int32_t burst, pbsBurst;  /* Period multiplier for upper limit. */
    
    /* Used internally by the kernel */
    unsigned long prev;
    
    /* For srTCM and trTCM, credit means cbsCredit and creditCap means cbsCreditCap.
    For srTCM, pbsCreditCap means ebsCreditCap. */
    u_int32_t credit, pbsCredit;
    u_int32_t creditCap, pbsCreditCap;
    
    struct xt_policerinfo *master;
};

#endif /*_XT_RATE_H*/
