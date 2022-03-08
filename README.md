# netlink examples

测试环境：x86-64 vmware+ubuntu16

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


## netlink example -- 基于libnl

## general netlink example -- 基于Linux原生API

## general netlink example -- 基于libnl
