# Qcap

## 1、介绍
基于dpdk实现的抓包、解码服务器

## 2、架构模型
抓包线程和任务线程都可以扩展。项目抓包利用了dpdk提高的RSS机制分流，使用硬件对ip层进行分流到对应的任务队列，每个任务
队列都有一个对应的任务线程，即线程独享任务队列，同时也独享流表，以此避免竞争影响性能。
但由于环境有限，该项目仅测试了单抓包线程的场景；

## 3、环境安装
### 3.1、设置环境变量
```
export RTE_SDK=/home/dpdk/dpdk-stable-21.11.2 && export PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig && export LD_LIBRARY_PATH=/usr/local/lib64
```
### 3.2、安装dpdk
```
pip3 install meson ninja
yum install -y numactl numactl-devel
git clone git://dpdk.org/dpdk-stable
cd dpdk-stable
git checkout 20.11
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