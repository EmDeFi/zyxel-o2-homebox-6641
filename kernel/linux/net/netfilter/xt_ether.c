
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv6.h>
#include <linux/if_vlan.h>
#include <linux/netfilter/xt_ether.h>
#include <linux/netfilter/x_tables.h>
/*__Verizon__, FuChia, QoS*/
#include <linux/module.h>
#include <linux/skbuff.h>


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Xtables: ether match");
MODULE_ALIAS("ipt_ether");
MODULE_ALIAS("ip6t_ether");

/*__Verizon__, FuChia, QoS*/
static bool ether_mt(const struct sk_buff *skb, const struct xt_match_param *par)
{
	const struct xt_ether_info *info = par->matchinfo;

	struct ethhdr *eth = NULL;
	struct vlan_hdr _frame, *fp;
	unsigned short TCI;	/* Whole TCI, given from parsed frame */
	unsigned short id;	/* VLAN ID, given from frame TCI */
	unsigned char prio;	/* user_priority, given from frame TCI */
#ifdef CONFIG_CLASS_DOWNSTREAM_8021Q  //__ZyXEL__, Jeff
	unsigned short savedEtherType = skb->vtag_word>>16;
#endif
		
	if(info->bitmask & IPT_ETHER_TYPE){
		eth = eth_hdr(skb);
		//printk("\n\nethe->type is 0x%x\n",eth->h_proto);
		//printk("info->ethertype is 0x%x\n",info->ethertype);
		//printk("result is %d\n",((eth->h_proto != info->ethertype) ^ !!(info->invert & IPT_ETHER_TYPE)));

		/*__Verizon__, FuChia, QoS: Valid this packet first.*/
		if(!(skb_mac_header(skb) >= skb->head && skb_mac_header(skb) + ETH_HLEN <= skb->data))
		  return False;

/*__ZyXEL__, Jeff*/
#ifdef CONFIG_CLASS_DOWNSTREAM_8021Q  
		if(info->ethertype == ETH_P_8021Q && skb->protocol != ETH_P_8021Q)
		{
			/* for new broadcom vlan device */
			if( (!(ntohs(((struct vlan_hdr *)(skb->vlan_header))->h_vlan_encapsulated_proto))) ^ !!(info->invert & IPT_ETHER_TYPE))
			{
				if(savedEtherType != ETH_P_8021Q)
					return False;
			}
		}
		else
		{
#endif

      /*__AT&T__, FuChia, QoS*/
      if((skb->protocol!= info->ethertype) ^ !!(info->invert & IPT_ETHER_TYPE))
			return False;

/*__ZyXEL__, Jeff*/
#ifdef CONFIG_CLASS_DOWNSTREAM_8021Q  
		}
#endif
	}

/*__ZyXEL__, Jeff*/
#ifdef CONFIG_CLASS_DOWNSTREAM_8021Q  
	TCI = (unsigned short) skb->vtag_word;
	if(savedEtherType == 0 && TCI == 0){
		if(ntohs(((struct vlan_hdr *)(skb->vlan_header))->h_vlan_encapsulated_proto) == 0){
			if(skb->protocol == ETH_P_8021Q) {
				fp = skb_header_pointer(skb, 0, sizeof(_frame), &_frame);
				if (fp == NULL)
					return False;
				TCI = ntohs(fp->h_vlan_TCI);
			}
		}
		else{
			TCI = ntohs(((struct vlan_hdr *)(skb->vlan_header))->h_vlan_TCI);
		}
	}
#else /*CONFIG_CLASS_DOWNSTREAM_8021Q */
	if(skb->vtag_save==0){
  		fp = skb_header_pointer(skb, 0, sizeof(_frame), &_frame);
		if (fp == NULL)
			return False;
		TCI = ntohs(fp->h_vlan_TCI);
	}
	else{
		TCI = skb->vtag_save;
	}
#endif /*CONFIG_CLASS_DOWNSTREAM_8021Q */

	if(info->bitmask & IPT_ETHER_8021Q){
		id = TCI & VLAN_VID_MASK;
		//printk("id is 0x%x\n",id);
		//printk("info->vlanid is 0x%x\n",info->vlanid);
		//printk("result is %d\n",((id != info->vlanid) ^ !!(info->invert & IPT_ETHER_8021Q)));
		if((id != info->vlanid) ^ !!(info->invert & IPT_ETHER_8021Q))
			return False;
	}
	if(info->bitmask & IPT_ETHER_8021P){
		prio = (TCI >> 13) & 0x7;
		//printk("prio  is 0x%x\n",prio );
		//printk("info->vlanpriority is 0x%x\n",info->vlanpriority);
		//printk("result is %d\n",((prio != info->vlanpriority) ^ !!(info->invert & IPT_ETHER_8021P)));
		if((prio != info->vlanpriority) ^ !!(info->invert & IPT_ETHER_8021P))
			return False;
	}

	return True;
}

/*__Verizon__, FuChia, QoS*/
static struct xt_match xt_ether_match[] __read_mostly = {
	{
		.name		= "ether",
		.family		= NFPROTO_UNSPEC,
		.match		= ether_mt,
		.matchsize	= sizeof(struct xt_ether_info),
		.hooks     = (1 << NF_INET_PRE_ROUTING) | (1 << NF_INET_LOCAL_OUT) |
	               (1 << NF_INET_FORWARD) | (1 << NF_INET_POST_ROUTING),
		.me		= THIS_MODULE,
	},
	{
		.name		= "ether",
		.family		= NFPROTO_UNSPEC,
		.match		= ether_mt,
		.matchsize	= sizeof(struct xt_ether_info),
		.hooks     = (1 << NF_INET_PRE_ROUTING) | (1 << NF_INET_LOCAL_OUT) |
	               (1 << NF_INET_FORWARD) | (1 << NF_INET_POST_ROUTING),
		.me		= THIS_MODULE,
	},
};


static int __init xt_ether_init(void)
{
	return xt_register_matches(xt_ether_match, ARRAY_SIZE(xt_ether_match));
}

static void __exit xt_ether_fini(void)
{
	xt_unregister_matches(xt_ether_match, ARRAY_SIZE(xt_ether_match));
}

module_init(xt_ether_init);
module_exit(xt_ether_fini);
