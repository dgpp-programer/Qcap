# Qcap

## 1、介绍
基于dpdk实现的抓包、解码服务器

#### 项目来源：
这是在我工作方向为linux C++服务端开发的时候，学习了一下dpdk，在学习dpdk的时候就顺带着写了这个服务器，
在我看来dpdk中有很多值得学习的高性能组件的实现方式，当然也是一个不错的工作方向；
于是我就想着理论和实践并行的方式写下了这个服务器，通过dpdk来抓包，实现了单包解码和会话解码的功能；
#### 可以干啥：
或许你可以基于这个框架去完善解码的流程（本服务器对于解码部分写的比较简单，如果以后有合适的开源解码项目，我可能会将它移植进来），
用来做一个抓包小工具也是不错的，补充一些功能后当作个人的小项目或者毕业设计都是可以的；

## 2、架构模型
抓包线程和任务线程都可以扩展。项目抓包利用了dpdk提供的RSS机制分流，使用硬件对ip层进行分流到对应的任务队列，每个任务
队列都有一个对应的任务线程，线程独享任务队列，同时也独享流表，以此避免竞争影响性能。
但由于环境有限，该项目仅测试了单抓包线程的场景；
```
                            --> 数据包无锁队列1 --> 解码线程1 --> 解码 --> 输出到文件
抓包线程 -->(RSS机制ip层分流)
                            --> 数据包无锁队列2 --> 解码线程2 --> 解码 --> 输出到文件
```

## 3、环境安装
### 3.1、设置环境变量
```
export RTE_SDK=/home/dpdk/dpdk-stable-21.11.2 && export PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig && export LD_LIBRARY_PATH=/usr/local/lib64
```
### 3.2、安装dpdk
```
pip3 install meson ninja
yum install -y numactl numactl-devel
git clone https://github.com/DPDK/dpdk.git
cd dpdk
git fetch origin v20.11
git checkout v20.11
meson build
cd build
ninja
ninja install
```
### 3.3、检查是否安装成功：
```
pkg-config --modversion libdpdk
20.11.3
```
### 3.4、加载驱动(2选1)
#### 3.4.1、vfio
```
modprobe vfio-pci
```
#### 3.4.2、uio
```
modprobe uio
git clone http://dpdk.org/git/dpdk-kmods
make
rmmod igb_uio.ko
insmod igb_uio.ko
cp igb_uio.ko /lib/modules/XXX系统内核号
depmod
modprobe igb_uio
```

### 4、网卡绑定和解绑脚本
```
目前已经把绑定网卡和解绑网卡写成了一个脚本，绑定起来更简单
注意：绑定的网卡不能是ssh连接的网卡，否则会导致ssh断开
绑定：
cd Qcap/
sh bind_network_card.sh 网卡名
如：
sh bind_network_card.sh ens256

解绑所有网卡：
sh unbind_all_network_card.sh 
```
#### 5、运行Qcap
```
cd Qcap/
mkdir build && cd build/
cmake ../
make
./Qcap
```
### 6、运行结果
```
系统日志：Qcap/log/system.log
输出结果：Qcap/log/packet.log
```
