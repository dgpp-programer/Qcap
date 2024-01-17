#ifndef CORE_DPDK
#define CORE_DPDK
#include <unistd.h>

#include <iostream>
#include <vector>
#include <atomic>

#include <rte_ether.h>
#include <rte_ip.h>
#include <rte_udp.h>
#include <rte_ethdev.h>
#include <rte_malloc.h>



#define HWADDR_LENGTH 6
#define NUM_BUFS    (4096-1)
#define BURST_SIZE 128

struct CapSummary
{
    CapSummary()
    {
        dropPktCount = 0;
        capPktCount = 0;
    }
    uint32_t dropPktCount;
    uint32_t capPktCount;
};

static struct rte_eth_conf g_devConfDefault = 
{
    .rxmode = {
        .mq_mode = ETH_MQ_RX_RSS,
        .max_lro_pkt_size = RTE_ETHER_MAX_LEN,
        .split_hdr_size = 0,
        .offloads = DEV_RX_OFFLOAD_CHECKSUM
    },
    .txmode = {
		.mq_mode = RTE_ETH_MQ_TX_NONE,
	},
    .rx_adv_conf = {
		.rss_conf = {
			.rss_key = NULL,
			.rss_hf = RTE_ETH_RSS_IP,
		},//RS硬件hash配置
	}
};

class CoreDpdk
{
public:
    CoreDpdk();
    ~CoreDpdk();
    bool Init();
    void UnInit();
    void Start();
    void Stop();
    bool Running() { return m_running.load();}
    void PollingCapture();
    CapSummary GetCapSummary() { return m_capSummary;}
    uint8_t GetRxQueNumber() {return m_rxQueNumber;}
    

public:
    std::vector<rte_ring*> m_pktRings;
    int m_dpdkPortId;

private:
    std::string m_name;
    uint8_t m_rxQueNumber;
    uint8_t m_txQueNumber;
    bool m_isPromiscuous; //是否打开混杂模式

    uint32_t m_pktRingSize;
    int32_t m_pktsRingFlags;
    int32_t m_socketId;

    std::atomic<bool> m_running;
    CapSummary m_capSummary;
};

#endif //CORE_DPDK