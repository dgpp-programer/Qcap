#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdint.h>
#include <string>
#include <iostream>
#include <arpa/inet.h>

//协议编号
#define  Q_HOPOPT      0       //IPv6逐跳选项
#define  Q_ICMP        1       //Internet控制消息
#define  Q_IGMP        2       //Internet组管理
#define  Q_GGP     3       //网关对网关
#define  Q_IP      4       //IP中的IP（封装）
#define  Q_ST      5       //流
#define  Q_TCP     6       //传输控制
#define  Q_CBT     7       //CBT
#define  Q_EGP     8       //外部网关协议
#define  Q_IGP     9       //任何专用内部网关(Cisco 将其用于 IGRP)
#define  Q_BBN_RCC_MON     10      //BBNRCC监视
#define  Q_NVP_II      11      //网络语音协议
#define  Q_PUP     12      //PUP
#define  Q_ARGUS       13      //ARGUS
#define  Q_EMCON       14      //EMCON
#define  Q_XNET        15      //跨网调试器
#define  Q_CHAOS       16      //Chaos
#define  Q_UDP     17      //用户数据报
#define  Q_MUX     18      //多路复用
#define  Q_DCN_MEAS        19      //DCN测量子系统
#define  Q_HMP     20      //主机监视
#define  Q_PRM     21      //数据包无线测量
#define  Q_XNS_IDP     22      //XEROX NS IDP
#define  Q_TRUNK_1     23      //第1主干
#define  Q_TRUNK_2     24      //第2主干
#define  Q_LEAF_1      25      //第1叶
#define  Q_LEAF_2      26      //第2叶
#define  Q_RDP     27      //可靠数据协议
#define  Q_IRTP        28      //Internet可靠事务
#define  Q_ISO_TP4     29      //ISO传输协议第4类
#define  Q_NETBLT      30      //批量数据传输协议
#define  Q_MFE_NSP     31      //MFE网络服务协议
#define  Q_MERIT_INP       32      //MERIT节点间协议
#define  Q_SEP     33      //顺序交换协议
#define  Q_3PC     34      //第三方连接协议
#define  Q_IDPR        35      //域间策略路由协议
#define  Q_XTP     36      //XTP
#define  Q_DDP     37      //数据报传送协议
#define  Q_IDPR_CMTP       38      //IDPR控制消息传输协议
#define  Q_TP__        39      //TP++传输协议
#define  Q_IL      40      //IL传输协议
#define  Q_IPv6        41      //Ipv6
#define  Q_SDRP        42      //源要求路由协议
#define  Q_IPv6_Route      43      //IPv6的路由标头
#define  Q_IPv6_Frag       44      //IPv6的片断标头
#define  Q_IDRP        45      //域间路由协议
#define  Q_RSVP        46      //保留协议
#define  Q_GRE     47      //通用路由封装
#define  Q_MHRP        48      //移动主机路由协议
#define  Q_BNA     49      //BNA
#define  Q_ESP     50      //IPv6的封装安全负载
#define  Q_AH      51      //IPv6的身份验证标头
#define  Q_I_NLSP      52      //集成网络层安全性TUBA
#define  Q_SWIPE       53      //采用加密的IP
#define  Q_NARP        54      //NBMA地址解析协议
#define  Q_MOBILE      55      //IP移动性
#define  Q_TLSP        56      //传输层安全协议(使用 Kryptonet 密钥管理)
#define  Q_SKIP        57      //SKIP
#define  Q_IPv6_ICMP       58      //用于IPv6的ICMP
#define  Q_IPv6_NoNxt      59      //用于IPv6的无下一个标头
#define  Q_IPv6_Opts       60      //IPv6的目标选项
#define  Q_CFTP        62      //CFTP
#define  Q_SAT_EXPAK       64      //SATNET与后台EXPAK
#define  Q_KRYPTOLAN       65      //Kryptolan
#define  Q_RVD     66      //MIT远程虚拟磁盘协议
#define  Q_IPPC        67      //Internet Pluribus数据包核心
#define  Q_SAT_MON     69      //SATNET监视
#define  Q_VISA        70      //VISA协议
#define  Q_IPCV        71      //Internet数据包核心工具
#define  Q_CPNX        72      //计算机协议网络管理
#define  Q_CPHB        73      //计算机协议检测信号
#define  Q_WSN     74      //王安电脑网络
#define  Q_PVP     75      //数据包视频协议
#define  Q_BR_SAT_MON      76      //后台SATNET监视
#define  Q_SUN_ND      77      //SUN ND PROTOCOL_Temporary
#define  Q_WB_MON      78      //WIDEBAND监视
#define  Q_WB_EXPAK        79      //WIDEBAND EXPAK
#define  Q_ISO_IP      80      //ISO Internet协议
#define  Q_VMTP        81      //VMTP
#define  Q_SECURE_VMTP     82      //SECURE_VMTP
#define  Q_VINES       83      //VINES
#define  Q_TTP     84      //TTP
#define  Q_NSFNET_IGP      85      //NSFNET_IGP
#define  Q_DGP     86      //异类网关协议
#define  Q_TCF     87      //TCF
#define  Q_EIGRP       88      //EIGRP
#define  Q_OSPFIGP     89      //OSPFIGP
#define  Q_Sprite_RPC      90      //Sprite RPC协议
#define  Q_LARP        91      //轨迹地址解析协议
#define  Q_MTP     92      //多播传输协议
#define  Q_AX_25       93      //AX.25帧
#define  Q_IPIP        94      //IP中的IP封装协议
#define  Q_MICP        95      //移动互联控制协议
#define  Q_SCC_SP      96      //信号通讯安全协议
#define  Q_ETHERIP     97      //IP中的以太网封装
#define  Q_ENCAP       98      //封装标头
#define  Q_GMTP        100     //GMTP
#define  Q_IFMP        101     //Ipsilon流量管理协议
#define  Q_PNNI        102     //IP上的PNNI
#define  Q_PIM     103     //独立于协议的多播
#define  Q_ARIS        104     //ARIS
#define  Q_SCPS        105     //SCPS
#define  Q_QNX     106     //QNX
#define  Q_A_N     107     //活动网络
#define  Q_IPComp      108     //IP负载压缩协议
#define  Q_SNP     109     //Sitara网络协议
#define  Q_Compaq_Peer     110     //Compaq对等协议
#define  Q_IPX_in_IP       111     //IP中的IPX
#define  Q_VRRP        112     //虚拟路由器冗余协议
#define  Q_PGM     113     //PGM可靠传输协议
#define  Q_L2TP        115     //第二层隧道协议
#define  Q_DDX     116     //D_II数据交换(DDX)
#define  Q_IATP        117     //交互式代理传输协议
#define  Q_STP     118     //计划传输协议
#define  Q_SRP     119     //SpectraLink无线协议
#define  Q_UTI     120     //UTI
#define  Q_SMP     121     //简单邮件协议
#define  Q_SM      122     //SM
#define  Q_PTP     123     //性能透明协议
#define  Q_ISIS        124     //Over IPv4
#define  Q_FIRE        125     //
#define  Q_CRTP        126     //Combat无线传输协议
#define  Q_CRUDP       127     //Combat无线用户数据报
#define  Q_SSCOPMCE        128     //
#define  Q_IPLT        129     //
#define  Q_SPS     130     //安全数据包防护
#define  Q_PIPE        131     //IP中的专用IP封装
#define  Q_SCTP        132     //流控制传输协议
#define  Q_FC      133     //光纤通道

inline static std::string GetProtoStr(uint16_t proto)
{
    switch(proto)
    {
        case Q_IP:
        {
            return "IPV4";
        }
        case Q_TCP:
        {
            return "TCP";
        }
        case Q_UDP:
        {
            return "UDP";
        }
    }
    return "UNKNON";
} 

#define ETHER_HEADER_LEN 14
#define MAC_LEN	6		        /* Octets in one ethernet addr	 */
struct EtherHeader
{
  uint8_t  dstMac[MAC_LEN];	    /* destination mac address	    */
  uint8_t  srcMac[MAC_LEN];	    /* source ether mac address	    */
  uint16_t netProtoType;		/* network layer protocol type  */
} __attribute__ ((__packed__));


#define IPV4_ADDR_LEN 4
typedef uint8_t ipv4_addr[IPV4_ADDR_LEN];
struct Ipv4Header{
	uint8_t	    vhl;		    /* ip version and header length */
#define GET_IP_VERSION(ipv4header)      ((((ipv4header)->vhl) & 0xf0) >> 4)
#define GET_IP_HEADERLEN(ipv4header)    (((( ipv4header)->vhl) & 0x0f) * 4)
	uint8_t	    tos;		    /* type of service              */
	uint16_t	ipPackLen;		/* total length(except ethernet size 14 bytes)             */
	uint16_t	ipid;		    /* identification               */
	uint16_t	fragment;		/* fragment offset field(三位标志位，13位段偏移)            */
#define	IP_DF       0x4000	    /* don't fragment flag          */
#define	IP_MF       0x2000	    /* more fragments flag          */
#define	IP_OFFMASK  0x1fff	    /* mask for fragmenting bits（分片数据相对于原始数据的偏移   */
	uint8_t	    ttl;		    /* time to live                 */
	uint8_t	    protocol;	    /* protocol                     */
	uint16_t	checkSum;		/* checksum                     */
	uint32_t	srcIp;
    uint32_t    dstIp;	        /* source and dest address  */
}__attribute__ ((__packed__));


#define IPV6_ADDR_LEN 16
typedef uint8_t ipv6_addr[IPV6_ADDR_LEN];
struct Ipv6Header
{
    uint32_t    verTosFlowLable;
#define GET_IPV6_VERSION(ipv6Header)    ((ntohl((ipv6Header)->verTosFlowLable) & 0xf0000000) >> 28)
#define GET_IPV6_TOS(ipv6Header)        ((ntohl((ipv6Header)->verTosFlowLable) & 0x0ff00000) >> 20)
#define GET_IPV6_FLOWLABLE(ipv6Header)  ((ntohl((ipv6Header)->verTosFlowLable) & 0x000fffff))
    uint16_t    payloadLen;        /*payload len(except header 40 bytes)*/
    uint8_t     nextHeader;     /*next header protocol type (tcp or udp ?)*/
    uint8_t     ttl;            /*the same as ipv4 ttl*/
    ipv6_addr   srcIp;
    ipv6_addr   dstIp;
}__attribute__ ((__packed__));;


struct TcpHeader
{
    uint16_t    srcPort;
    uint16_t    dstPort;
    uint32_t    seq;
    uint32_t    ack;
    uint8_t     hlr;            /*header len(4bit) and reserved(4bit)   */
#define GET_TCP_HEADERLEN(tcpheader)    (((((tcpheader)->hlr) & 0xf0) >> 4) * 4)
    uint8_t     flag;           /*CWR ECE URG ACK PSH RST SYN FIN       */
#define  TCP_FLAG_CWR         0x80
#define  TCP_FLAG_ECE         0x40
#define  TCP_FLAG_URG         0x20
#define  TCP_FLAG_ACK         0x10
#define  TCP_FLAG_PSH         0x08
#define  TCP_FLAG_RST         0x04
#define  TCP_FLAG_SYN         0x02
#define  TCP_FLAG_FIN         0x01
    uint16_t    windowSize;     /*receive windows size                  */
    uint16_t    checkSum;         /*internet checksum                     */
    uint16_t    urgpointer;     /*urgent data pointer                   */
}__attribute__ ((__packed__));;


struct UdpHeader
{
    uint16_t    srcPort;
    uint16_t    dstPort;
    uint16_t    dataLen;
    uint16_t    checkSum;
}__attribute__ ((__packed__));


#endif //PROTOCOL_H