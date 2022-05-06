# netlink examples

netlink是用户空间app和内核空间通信的一种机制。
generic netlink是在netlink基础上扩展出来的一种更通用的机制。
本代码仓列举了netlink的使用示例。其中用户空间app有基于Linux原生API，也有基于libnl API。

测试环境：x86-64 vmware + Ubuntu16

注：我的Ubuntu上没有libnl-3.so这个文件，有的是libnl-3.so.200。
在链接libnl库时会报错，我是手动建了一个libnl-3.so软链接后解决。
```
cd /lib/x86_64-linux-gnu
sudo ln -s libnl-3.so.200.22.0 libnl-3.so
sudo ln -s libnl-genl-3.so.200.22.0 libnl-genl-3.so
```

## netlink_raw -- 基于Linux原生API

```
//内核模块编译与加载:
cd netlink_raw/kernel_module
make
sudo insmod netlink_kernel.ko

//用户空间app编译与运行：
cd netlink_raw/user_app
make
./bin/netlink_user
```

## netlink_libnl -- 基于libnl API

```
//内核模块编译与加载:
cd netlink_libnl/kernel_module
make
sudo insmod netlink_kernel.ko

//用户空间app编译与运行：
cd netlink_libnl/user_app
make
./bin/netlink_user
```
这里netlink_libnl/kernel_module的代码与netlink_raw/kernel_module里的代码完全一样，
只是应用层基于libnl API重写。

include/libnl3/netlink这个目录是libnl3的头文件目录。
我本地Ubuntu机器上没找到，我直接基于libnl3源码编译复制了一份。

## ge_netlink_libnl -- 基于libnl generic netlink API

```
//内核模块编译与加载:
cd ge_netlink_libnl/kernel_module
make
sudo insmod ge_netlink_kernel.ko

//用户空间app编译与运行：
cd ge_netlink_libnl/user_app
make
./bin/ge_netlink_user
```
