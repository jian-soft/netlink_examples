#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>     /* Needed for the module_init macros */

#include <net/genetlink.h>

#define FAMILY_NAME "my_genl"

/* Step1: 定义操作 */
/* attributes */
enum {
    EXMPL_A_UNSPEC,
    EXMPL_A_MSG,
    _EXMPL_A_MAX,
};
#define EXMPL_A_MAX (_EXMPL_A_MAX - 1)
// define attribute policy
// policy的数组大小为最大有效属性值+1
static struct nla_policy exmpl_genl_policy[EXMPL_A_MAX + 1] = {
    [EXMPL_A_MSG] = {.type = NLA_NUL_STRING},
};
// handler
static int exmpl_echo(struct sk_buff *skb, struct genl_info *info);
// commands
enum {
    EXMPL_C_UNSPEC,
    EXMPL_C_ECHO,
    _EXMPL_C_MAX,
};
#define EXMPL_C_MAX (_EXMPL_C_MAX - 1)
// operation definition
struct genl_ops exmpl_genl_ops[EXMPL_C_MAX] = {
    {
        .cmd = EXMPL_C_ECHO,
        .doit = exmpl_echo,
        .policy = exmpl_genl_policy,
    }
};

/* Step2: 定义family */
// family definition
static struct genl_family my_genl_family = {
    .id = 0,
    .hdrsize = 0,  //表示没有用户自定义的额外header
    .name = FAMILY_NAME,
    .version = 1,
    .ops = exmpl_genl_ops,
    .n_ops = ARRAY_SIZE(exmpl_genl_ops),
    .maxattr = EXMPL_A_MAX,
};

static int exmpl_echo(struct sk_buff *skb, struct genl_info *info)
{
    struct nlattr *na;
    struct sk_buff *reply_skb;
    void *msg_head;
    int ret;

    printk("%s in.\n", __func__);

    //内核已经解析好了每个attr
    na = info->attrs[EXMPL_A_MSG];
    if (!na) {
        printk("Error: attr EXMPL_A_MSG is null\n");
        return -EINVAL;
    }
    printk("Recv message: %s\n", (char *)nla_data(na));

    //将收到的消息发回去
    reply_skb = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
    //填写genl消息头
    msg_head = genlmsg_put(reply_skb, info->snd_portid, info->snd_seq, &my_genl_family, 0, EXMPL_C_ECHO);
    //向skb尾部填写attr
    nla_put_string(reply_skb, EXMPL_A_MSG, nla_data(na));
    //Finalize the message: 更新nlmsghdr中的nlmsg_len字段
    genlmsg_end(reply_skb, msg_head);
    //Send the message back
    ret = genlmsg_reply(reply_skb, info);
    if (ret != 0) {
        printk("genlmsg_reply return fail: %d\n", ret);
        return -ret;
    }

    return 0;
} 

static int __init ge_netlink_kernel_init(void)
{
    int ret;
    printk(KERN_INFO "ge_netlink_kernel: init module\n");

    /* Step3: 注册famliy */
    ret = genl_register_family(&my_genl_family);
    if (ret != 0) {
        printk("genl_register_family fail, ret:%d\n", ret);
        return ret;
    }

    return 0;

}

static void __exit ge_netlink_kernel_exit(void)
{
    printk(KERN_INFO "ge_netlink_kernel: exit module\n");
    genl_unregister_family(&my_genl_family);

}

module_init(ge_netlink_kernel_init);
module_exit(ge_netlink_kernel_exit);

MODULE_LICENSE("GPL");
