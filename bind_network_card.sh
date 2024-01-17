#!/bin/bash
# 检查是否传递了网卡名参数
network_card=$1
if [ -z "$network_card" ]; then
    echo "Missing parameter. Please provide the network card name. eg dpdk-igb_uio-init.sh ens256"
    exit 1
fi

echo -e "\e[34m*** dpdk igb_uio mode preoperational environment initialization ***\e[0m"
export RTE_SDK=/home/dpdk/dpdk-stable-21.11.2
export PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig
export LD_LIBRARY_PATH=/usr/local/lib64

echo -e "\e[32m   RTE_SDK = ${RTE_SDK} \e[0m"
echo -e "\e[32m   PKG_CONFIG_PATH = ${PKG_CONFIG_PATH} \e[0m"
echo -e "\e[32m   LD_LIBRARY_PATH = ${LD_LIBRARY_PATH} \e[0m"



#down掉网卡
ifconfig $network_card down

#插入igb模块
modprobe uio
insmod ./igb_uio/igb_uio.ko
depmod
modprobe igb_uio

output=$(/home/dpdk/dpdk-stable-21.11.2/usertools/dpdk-devbind.py --status)

# 解析输出内容，获取包含"if="的行，并提取满足条件的行首的第一串字符
regex="^([0-9a-fA-F:.]+) .+if=${network_card}"
result=""
while IFS= read -r line; do
    if [[ $line =~ $regex ]]; then
        result="${BASH_REMATCH[1]}"
        break
    fi
done <<< "$output"


/home/dpdk/dpdk-stable-21.11.2/usertools/dpdk-devbind.py --bind=igb_uio $result
/home/dpdk/dpdk-stable-21.11.2/usertools/dpdk-devbind.py --status

# ens224: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP group default qlen 1000
#     link/ether 00:0c:29:c8:8b:f5 brd ff:ff:ff:ff:ff:ff
#     inet 192.168.182.129/24 brd 192.168.182.255 scope global noprefixroute dynamic ens224
#        valid_lft 5183695sec preferred_lft 5183695sec
#     inet6 fe80::7604:5b24:26ee:c96a/64 scope link noprefixroute
#        valid_lft forever preferred_lft forever
# ens256: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP group default qlen 1000
#     link/ether 00:0c:29:c8:8b:eb brd ff:ff:ff:ff:ff:ff
#     inet 192.168.182.140/24 brd 192.168.182.255 scope global noprefixroute dynamic ens256
#        valid_lft 5183695sec preferred_lft 5183695sec
#     inet6 fe80::9d31:a388:41cc:9470/64 scope link noprefixroute
#        valid_lft forever preferred_lft forever

# 0000:02:00.0 '82545EM Gigabit Ethernet Controller (Copper) 100f' if=ens32 drv=e1000 unused=igb_uio *Active*
# 0000:02:02.0 '82545EM Gigabit Ethernet Controller (Copper) 100f' if=ens34 drv=e1000 unused=igb_uio *Active*
# 0000:02:03.0 '82545EM Gigabit Ethernet Controller (Copper) 100f' if=ens35 drv=e1000 unused=igb_uio *Active*
# 0000:02:04.0 '82545EM Gigabit Ethernet Controller (Copper) 100f' if=ens36 drv=e1000 unused=igb_uio *Active*