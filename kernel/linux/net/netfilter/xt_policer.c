/* Kernel module to control the rate in kbps. */
/* This program is free software; you can redistribute it and/or modify
 *  * it under the terms of the GNU General Public License version 2 as
 *   * published by the Free Software Foundation. */
/* ZyXEL Birken, 20100107. */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_policer.h>

/*__ZyXEL__, Jones For compilation*/
#define MODE_TBF   0
#define MODE_SRTCM 1
#define MODE_TRTCM 2

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Herve Eychenne <rv@wallfire.org>");
MODULE_DESCRIPTION("iptables rate policer match");
MODULE_ALIAS("ipt_policer");
MODULE_ALIAS("ip6t_policer");

/* The algorithm used is the Simple Token Bucket Filter (TBF)
 *  * see net/sched/sch_tbf.c in the linux source tree. */

static DEFINE_SPINLOCK(policer_lock);

/*__Verizon__, richard, QoS*/
static bool ipt_policer_match(const struct sk_buff *skb, const struct xt_match_param *par)
{
	struct xt_policerinfo *r = (struct xt_policerinfo *)par->matchinfo;
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
			return true;
			break;
		}
		/* We're limited. (Not Match) */
		spin_unlock_bh(&policer_lock);
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
static bool
ipt_policer_checkentry(const struct xt_mtchk_param *par)                       
{
       struct xt_policerinfo *r = (struct xt_policerinfo *)par->matchinfo;
	/* For SMP, we only want to use one set of counters. */
	r->master = r;

	/* pRate must be equal or greater than crate. */
	if (r->policerMode == 2) {
		if (r->rate > r->pRate) {
			return 0;
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
			return 0;				
		}
	}	
	return 1;
}
/* end ipt_policer_checkentry */


#ifdef CONFIG_COMPAT
struct compat_xt_rateinfo {
	u_int32_t avg;
	u_int32_t burst;

	compat_ulong_t prev;
	u_int32_t credit;
	u_int32_t credit_cap, cost;

	u_int32_t master;
};

/* To keep the full "prev" timestamp, the upper 32 bits are stored in the
 *  * master pointer, which does not need to be preserved. */
static void compat_from_user(void *dst, void *src)
{
	struct compat_xt_rateinfo *cm = src;
	struct xt_policerinfo m = {
		.avg	             = cm->avg,
		.burst            = cm->burst,
		.prev             = cm->prev | (unsigned long)cm->master << 32,
		.credit           = cm->credit,
		.credit_cap     = cm->credit_cap,
		.cost             = cm->cost,
	};
	memcpy(dst, &m, sizeof(m));
}

static int compat_to_user(void __user *dst, void *src)
{
	struct xt_policerinfo *m = src;
	struct compat_xt_rateinfo cm = {
		.avg              = m->avg,
		.burst            = m->burst,
		.prev             = m->prev,
		.credit           = m->credit,
		.credit_cap     = m->creditCap,
		.cost             = m->cost,
		.master          = m->prev >> 32,
	};
	return copy_to_user(dst, &cm, sizeof(cm)) ? -EFAULT : 0;
}
#endif /* CONFIG_COMPAT */

/*__Verizon__, richard, QoS*/
static struct xt_match xt_policer_match __read_mostly = {
        .name              = "policer",
        .family            = NFPROTO_UNSPEC,
        .checkentry        = ipt_policer_checkentry,
        .match             = ipt_policer_match,
        .matchsize         = sizeof(struct xt_policerinfo),
#ifdef CONFIG_COMPAT
        .compatsize        = sizeof(struct compat_xt_rateinfo),
        .compat_from_user  = compat_from_user,
        .compat_to_user    = compat_to_user,
#endif
        .me                = THIS_MODULE,
};

static int __init xt_policer_init(void) 
{
   /*__Verizon__, richard, QoS*/
   return xt_register_match(&xt_policer_match);
}

static void __exit xt_policer_fini(void)
{
   /*__Verizon__, richard, QoS*/
   xt_unregister_match(&xt_policer_match);
}

module_init(xt_policer_init);
module_exit(xt_policer_fini);

