#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h> //getpid()

#include <sys/socket.h>
#include <linux/netlink.h>

#define MY_NETLINK 31
#define MAX_PAYLOAD 1024

int sock_fd;
struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh;
struct iovec iov;
struct msghdr msg;

//There are 2 ways to send msg to kernel
//1-sendmsg
static void send_via_sendmsg()
{
    //set io vector
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;

    //set msghdr
    msg.msg_name = (void *)&dest_addr;  //address
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;  //iov's count

    printf("sendmsg to kernel\n");
    sendmsg(sock_fd, &msg, 0);
    printf("Waiting for message from kernel\n");

    /* Read message from kernel */
    recvmsg(sock_fd, &msg, 0);
    printf("recvmsg payload: %s\n", (char *)NLMSG_DATA(nlh));
}

//2-sendto
static void send_via_sendto()
{
    printf("sendto to kernel\n");
    sendto(sock_fd, nlh, nlh->nlmsg_len, 0,
           (struct sockaddr*)&dest_addr, sizeof(dest_addr));

    printf("Waiting for message from kernel\n");

    /* Read message from kernel */
    recvfrom(sock_fd, nlh, NLMSG_LENGTH(MAX_PAYLOAD), 0, NULL, NULL);
    printf("recvmsg payload: %s\n", (char *)NLMSG_DATA(nlh));

}


int main(int argc, char* argv[])
{
    sock_fd = socket(PF_NETLINK, SOCK_RAW, MY_NETLINK);
    if (sock_fd < 0)
        return -1;

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); /* self pid */

    bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));

    //set dest address
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; /* For Linux Kernel */
    dest_addr.nl_groups = 0; /* unicast */

    //set nlmsg
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;
    strcpy(NLMSG_DATA(nlh), "Hello");


    send_via_sendmsg();

    send_via_sendto();


    close(sock_fd);
    free(nlh);

    return 0;
}



