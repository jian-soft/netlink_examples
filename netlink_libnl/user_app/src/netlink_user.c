#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <netlink/netlink.h>
#include <netlink/msg.h>


#define MY_NETLINK 31
#define MY_NETLINK_TYPE_SET 0

static int my_input(struct nl_msg *msg, void *arg)
{
    struct nlmsghdr *nlh = nlmsg_hdr(msg);
    char *data = nlmsg_data(nlh);
    int datalen = nlmsg_datalen(nlh);

    printf("input cb: datalen:%d, data:%s\n", datalen, data);

    return 0;
}



int main(int argc, char* argv[])
{
    struct nl_sock *sk;
    int ret;
    
    sk = nl_socket_alloc();
    nl_socket_modify_cb(sk, NL_CB_MSG_IN, NL_CB_CUSTOM, my_input, NULL);

    ret = nl_connect(sk, MY_NETLINK);
    if (ret < 0) {
        //printf("Error: nl_connect return:%d\n", ret);
        nl_perror(ret, "nl_connect");
        return -1;
    }
    
    char msg[] = "Hello libnl!";
    
    ret = nl_send_simple(sk, MY_NETLINK_TYPE_SET, 0, msg, sizeof(msg));
    printf("Info: nl_send_simple return:%d\n", ret);

    nl_recvmsgs_default(sk);

    nl_socket_free(sk);

    return 0;
}



