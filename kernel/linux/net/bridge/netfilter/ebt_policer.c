/* Kernel module to control the rate in kbps. */
/* This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation. */
/*  ZyXEL Stan, 20100105*/

#include <linux/module.h>

/*__Verizon__, ZyXEL richard, QoS*/
#include <linux/netfilter/x_tables.h>

#include <linux/netfilter_bridge/ebtables.h>
#include <linux/netfilter_bridge/ebt_policer.h>

#include <linux/netdevice.h>
#include <linux/spinlock.h>

/*___ZyXEL__, Jones For compilation*/
#define MODE_TBF   0
#define MODE_SRTCM 1
#define MODE_TRTCM 2

static DEFINE_SPINLOCK(policer_lock);
/*__ZyXEL__, Jones For compilation*/
static bool ebt_policer_match(const struct sk_buff *skb, const struct xt_match_param *par)
                             
{
       struct ebt_policer_info *r = (struct ebt_policer_info *)par->matchinfo;
	unsigned long now = jiffies;
	unsigned long timePassed = 0;
	struct sk_buff *tmp;
	u_int32_t cost = 0;
	u_int32_t extraCredit = 0; 
	spin_lock_bh(&policer_lock);
	
/*__OBM__, Jones*/
#if defined(CONFIG_MIPS_BRCM) && defined(CONFIG_BLOG)
	blog_skip((struct sk_buff *)skb);
#endif

	//printk(KERN_EMERG "111__skb->mark=%x\n\r", skb->mark);
	switch(r->policerMode) {
	/* Token Bucket Filter (tbf) mode */
	/* The algorithm used is the Simple Token Bucket Filter (TBF)
	   see net/sched/sch_tbf.c in the linux source tree. */
	case MODE_TBF: 
		r->credit += (now - xchg(&r->prev, now)) * r->rate; /* Add TBF cerdit */ 
		if (r->credit > r->creditCap) {
			r->credit = r->creditCap;
		}
		cost = (skb->len + skb->mac_len) * BITS_PER_BYTE;
		if (r->credit >= cost) {
			/* We're not limited. (Match) */
			r->credit -= cost;						        /* Take out credit */ 	
			spin_unlock_bh(&policer_lock);
		//printk(KERN_EMERG "222__skb->mark=%x\n\r", skb->mark);
			return true;
			break;
		}
		/* We're limited. (Not Match) */
		spin_unlock_bh(&policer_lock);
		//printk(KERN_EMERG "333__skb->mark=%x\n\r", skb->mark);
		return false;
		break;

	/* Single Rate Three Color Marker (srTCM) Mode */
	case MODE_SRTCM:
		/* Add CBS first */
		r->credit += (now - xchg(&r->prev, now)) * r->rate; /* Add CBS cerdit */
		if (r->credit > r->creditCap) {
			extraCredit = r->credit - r->creditCap;
			r->credit = r->creditCap;
		}
		if (r->pbsCredit < r->pbsCreditCap && extraCredit > 0) {
			r->pbsCredit += extraCredit;                        /* Add EBS cerdit */
			if (r->pbsCredit > r->pbsCreditCap) {
				r->pbsCredit = r->pbsCreditCap;
			}
		}
		cost = (skb->len + skb->mac_len) * BITS_PER_BYTE;
		tmp = (struct sk_buff *)skb;
		if (r->credit >= cost) {
			tmp->mark &= ~(0x300000); /* Reset 2 color bit */
			tmp->mark |= 0x300000;    /* Green */
			r->credit -= cost;
		}
		else if (r->pbsCredit >= cost) {
			tmp->mark &= ~(0x300000); /* Reset 2 color bit */
			tmp->mark |= 0x200000;    /* Yellow */
			r->pbsCredit -= cost;
		}
		else {
			tmp->mark &= ~(0x300000); /* Reset 2 color bit */
			tmp->mark |= 0x100000;    /* Red */
		}
		spin_unlock_bh(&policer_lock);
		return true;
		break;

	/* Two Rate Three Color Marker (srTCM) Mode */
	case MODE_TRTCM:
		timePassed = (now - xchg(&r->prev, now));
		r->credit += timePassed * r->rate;            /* Add CBS cerdit */
		r->pbsCredit += timePassed * r->pRate;        /* Add PBS cerdit */
		if (r->credit > r->creditCap) {
			r->credit = r->creditCap;
		}
		if (r->pbsCredit > r->pbsCreditCap) {
			r->pbsCredit = r->pbsCreditCap;
		}
		cost = (skb->len + skb->mac_len) * BITS_PER_BYTE;
		tmp = (struct sk_buff *)skb;
		if (r->pbsCredit < cost) {
			tmp->mark &= ~(0x300000); /* Reset 2 color bit */
			tmp->mark |= 0x100000;    /* Red */
		}
		else if (r->credit < cost) {
			tmp->mark &= ~(0x300000); /* Reset 2 color bit */
			tmp->mark |= 0x200000;    /* Yellow */
			r->pbsCredit -= cost;
		}
		else {
			tmp->mark &= ~(0x300000); /* Reset 2 color bit */
			tmp->mark |= 0x300000;    /* Green */
			r->pbsCredit -= cost;
			r->credit -= cost;
		}
		spin_unlock_bh(&policer_lock);
		return true;
		break;

	default:
		return false;
	}
}

/*__ZyXEL__, Jones For compilation*/
/* Precision saver. */
/* As a policer rule added, this function will be executed */ 
static bool ebt_policer_check(const struct xt_mtchk_param *par)
{
	struct ebt_policer_info *r = par->matchinfo;
	
	/* pRate must be equal or greater than crate. */
	if (r->policerMode == 2) {
		if (r->rate > r->pRate) {
			return false;	
		}
	}

	if (r->creditCap == 0) { /* Check if policer initiate or not. */ 
		switch(r->policerMode) {
		case MODE_TBF:
			r->prev = jiffies;
			r->creditCap = r->burst * BITS_PER_BYTE * KILO_SCALE;       /* TBF Credits full */
			r->credit = r->creditCap;                                   /* TBF Credits full */		
			break;

		case MODE_SRTCM:
			r->prev = jiffies;
			r->creditCap = r->burst * BITS_PER_BYTE * KILO_SCALE;       /* CBS Credits full */
			r->credit = r->creditCap;                                   /* CBS Credits full */
			r->pbsCreditCap = r->pbsBurst * BITS_PER_BYTE * KILO_SCALE; /* EBS Credits full */
			r->pbsCredit = r->pbsCreditCap;                             /* EBS Credits full */
			break;
			
		case MODE_TRTCM:	
			r->prev = jiffies;
			r->creditCap = r->burst * BITS_PER_BYTE * KILO_SCALE;       /* CBS Credits full. */
			r->credit = r->creditCap;                                   /* CBS Credits full. */
			r->pbsCreditCap = r->pbsBurst * BITS_PER_BYTE * KILO_SCALE; /* PBS Credits full. */
			r->pbsCredit = r->pbsCreditCap;                             /* PBS Credits full. */ 
			break;

		default:
			return false;				
		}
	}
	return true;;
}


/*__Verizon__, ZyXEL richard, QoS*/
static struct xt_match ebt_policer_reg __read_mostly =
{
    .name = EBT_POLICER_MATCH,
    .revision	= 0,
    .family		= NFPROTO_BRIDGE,
    .match  = ebt_policer_match,
    .checkentry  = ebt_policer_check,
    .matchsize	= XT_ALIGN(sizeof(struct ebt_policer_info)),
    .me     = THIS_MODULE,
};

static int __init ebt_policer_init(void)
{
   /*__Verizon__, ZyXEL richard, QoS*/
   return xt_register_match(&ebt_policer_reg);

}

static void __exit ebt_policer_fini(void)
{
   /*__Verizon__, ZyXEL richard, QoS*/
   xt_unregister_match(&ebt_policer_reg);
}

module_init(ebt_policer_init);
module_exit(ebt_policer_fini);
MODULE_LICENSE("GPL");

