#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>     /* Needed for the module_init macros */
#include <linux/netlink.h>
#include <net/netlink.h>    /* nlmsg_new etc. */
#include <net/net_namespace.h>    /* init_net */

#define MY_NETLINK 31

static struct sock *g_nl_sock = NULL;


static void netlink_recv_msg(struct sk_buff *skb) 
{
    struct sk_buff *skb_out;
    struct nlmsghdr *nlh;
    int msg_size;
    char *msg;
    int pid;
    int res;
    
    nlh = (struct nlmsghdr *)skb->data;
    pid = nlh->nlmsg_pid; /* pid of sending process */
    msg = (char *)nlmsg_data(nlh);
    msg_size = strlen(msg);
    
    printk(KERN_INFO "mynetlink: Received from pid %d: %s\n", pid, msg);
    
    // create reply
    skb_out = nlmsg_new(msg_size, 0);
    if (!skb_out) {
        printk(KERN_ERR "mynetlink: Failed to allocate new skb\n");
        return;
    }
    
    // put received message into reply
    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
    strncpy(nlmsg_data(nlh), msg, msg_size);
    
    printk(KERN_INFO "mynetlink: Send %s\n", msg);
    
    res = nlmsg_unicast(g_nl_sock, skb_out, pid);
    if (res < 0)
      printk(KERN_INFO "mynetlink: Error while sending skb to user\n");
}

static int __init mynetlink_init(void)
{
    struct netlink_kernel_cfg cfg = {
        .input = netlink_recv_msg,
    };

    printk(KERN_INFO "mynetlink: init module\n");

    g_nl_sock = netlink_kernel_create(&init_net, MY_NETLINK, &cfg);
    if (!g_nl_sock) {
        printk(KERN_ALERT "mynetlink: Error creating socket.\n");
        return -10;
    }

    return 0;

}

static void __exit mynetlink_exit(void)
{
    printk(KERN_INFO "mynetlink: exit module\n");

    netlink_kernel_release(g_nl_sock);
}

module_init(mynetlink_init);
module_exit(mynetlink_exit);

MODULE_LICENSE("GPL");
