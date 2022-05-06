#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>

#define FAMILY_NAME "my_genl"

/* attributes */
enum {
    EXMPL_A_UNSPEC,
    EXMPL_A_MSG,
    _EXMPL_A_MAX,
};
#define EXMPL_A_MAX (_EXMPL_A_MAX - 1)
// define attribute policy
static struct nla_policy exmpl_genl_policy[EXMPL_A_MAX + 1] = {
    [EXMPL_A_MSG] = {.type = NLA_STRING},
};
// commands
enum {
    EXMPL_C_UNSPEC,
    EXMPL_C_ECHO,
    _EXMPL_C_MAX,
};
#define EXMPL_C_MAX (_EXMPL_C_MAX - 1)

//接收回调定义
int recv_callback(struct nl_msg* recv_msg, void* arg)
{
    struct nlmsghdr *nlh = nlmsg_hdr(recv_msg);
    struct nlattr *tb_msg[EXMPL_A_MAX + 1];

    if (nlh->nlmsg_type == NLMSG_ERROR) {
        printf("Received NLMSG_ERROR message!\n");
        return NL_STOP;
    }

    struct genlmsghdr *gnlh = (struct genlmsghdr*)nlmsg_data(nlh);

    nla_parse(tb_msg, EXMPL_A_MAX,
              genlmsg_attrdata(gnlh, 0),
              genlmsg_attrlen(gnlh, 0),
              exmpl_genl_policy);

    // check if a msg attribute was actually received
    if (tb_msg[EXMPL_A_MSG]) {
        // parse it as string
        char * payload_msg = nla_get_string(tb_msg[EXMPL_A_MSG]);
        printf("Kernel replied: %s\n", payload_msg);
    } else {
        printf("Attribute EXMPL_A_MSG is missing\n");
    }

    return NL_OK;
}

int main(int argc, char* argv[])
{
    //创建并连接genl socket
    struct nl_sock *sk = nl_socket_alloc();
    genl_connect(sk);

    int family_id;
    family_id = genl_ctrl_resolve(sk, FAMILY_NAME);
    if (family_id < 0) {
        printf("generic netlink family '" FAMILY_NAME "' NOT REGISTERED\n");
        nl_socket_free(sk);
        exit(-1);
    } else {
        printf("Family-ID of generic netlink family '" FAMILY_NAME "' is: %d\n", family_id);
    }

    //设置接收回调 
    nl_socket_modify_cb(sk, NL_CB_MSG_IN, NL_CB_CUSTOM, recv_callback, NULL);

    //发送消息
    struct nl_msg *msg = nlmsg_alloc();
    genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, family_id,
                0, NLM_F_REQUEST, EXMPL_C_ECHO, 1);
    NLA_PUT_STRING(msg, EXMPL_A_MSG, "genl message from user to kernel");
    int res = nl_send_auto(sk, msg);
    nlmsg_free(msg);
    if (res < 0) {
        printf("nl_send_auto fail, ret:%d\n", res);
    } else {
        printf("nl_send_auto OK, ret: %d\n", res);
    }

    //接收消息
    nl_recvmsgs_default(sk);

nla_put_failure: //referenced by NLA_PUT_STRING 
    nl_socket_free(sk);

    return 0;
}



