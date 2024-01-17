#include "core_dpdk.h"
#include "log.h"
#include "config.h"


CoreDpdk::CoreDpdk()
{
    m_name          = "Qcap";
    m_rxQueNumber   = 1;
    m_txQueNumber   = 0;
    m_isPromiscuous = Config::GetConfig()->getValue<bool>("isPromiscuous");
    m_pktsRingFlags = RING_F_SP_ENQ | RING_F_MC_RTS_DEQ;
    m_pktRingSize = 1024;
    m_dpdkPortId = 0;
    m_running = true;
    m_pktRings.resize(m_rxQueNumber);
}

CoreDpdk::~CoreDpdk()
{
    LOG_INFO("Qcap exit CoreDpdk");
}

void CoreDpdk::UnInit()
{
    rte_eth_dev_stop(m_dpdkPortId);
    rte_eal_cleanup();
}

bool CoreDpdk::Init()
{
    int ret = 0;
    char *env = (char *)m_name.c_str();
    if ((ret = rte_eal_init(1, &env)) < 0)
    {
        LOG_ERROR("Error with eal init, return {}", ret);
        return false;
    }

    struct rte_mempool *mbufPool = rte_pktmbuf_pool_create("mbuf_pool", NUM_BUFS, 0, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
    if (NULL == mbufPool)
    {
        LOG_ERROR("mbuf_pool create failed");
        return false;
    }

    uint16_t nb_sys_ports = rte_eth_dev_count_avail();
    if (0 == nb_sys_ports)
    {
        LOG_ERROR("No Support eth found, Please bind eth");
        return false;
    }
    struct rte_eth_dev_info devinfo;
    rte_eth_dev_info_get(m_dpdkPortId, &devinfo);
    int maxRxQue = devinfo.max_rx_queues;
    int maxTxQue = devinfo.max_tx_queues;
    LOG_INFO("NIC suport max rx queues = {}, mac tx queues = {}", maxRxQue, maxTxQue, ret);
    if(m_rxQueNumber > maxRxQue)
    {
        m_rxQueNumber = maxRxQue;
    }
    if(m_txQueNumber > maxTxQue)
    {
        m_txQueNumber = maxTxQue;
    }
    if(1 == m_rxQueNumber)
    {
        g_devConfDefault.rxmode.mq_mode = ETH_MQ_RX_NONE;
        g_devConfDefault.rx_adv_conf.rss_conf.rss_hf = 0;
    }

    ret = rte_eth_dev_configure(m_dpdkPortId, m_rxQueNumber, m_txQueNumber, &g_devConfDefault);
    if (ret < 0)
    {
        LOG_ERROR("rte_eth_dev_configure failed, return {}", ret);
        return false;
    }

    m_socketId = rte_eth_dev_socket_id(m_dpdkPortId);
    for(int i = 0; i < m_rxQueNumber; ++i)
    {
        ret = rte_eth_rx_queue_setup(m_dpdkPortId, i, 128, m_socketId, NULL, mbufPool);
        if (ret < 0)
        {
            LOG_ERROR("rte_eth_rx_queue_setup failed, queue id = {}, return {}}", i, ret);
            return false;
        }
        m_pktRings[i] = rte_ring_create("rawPktRing", rte_align32pow2(m_pktRingSize), m_socketId, m_pktsRingFlags);
        if(!m_pktRings[i])
        {
            LOG_ERROR("rte_ring_create failed, queue id = {}, error {}", i, rte_errno);
            return false;
        }
    }

    ret = rte_eth_dev_start(m_dpdkPortId);
    if (ret < 0)
    {
        LOG_ERROR("rte_eth_dev_start failed, return {}", ret);
        return false;
    }

    if(m_isPromiscuous)
    {
        rte_eth_promiscuous_enable(m_dpdkPortId);
    }
    
    return true;
}

void CoreDpdk::Start()
{
    LOG_INFO("Start PollingCapture");
    PollingCapture();
    UnInit();
    return;
}

void CoreDpdk::Stop()
{
    m_running.store(false);
}

void CoreDpdk::PollingCapture()
{
    struct rte_mbuf *mbufs[BURST_SIZE];
    uint16_t nbRx;
    int nbRxEnque;
    while (m_running)
    {
        usleep(50);
        for(int i = 0; i < m_rxQueNumber; ++i)
        {
            nbRx = rte_eth_rx_burst(m_dpdkPortId, i, mbufs, BURST_SIZE);
            if(0 == nbRx)
            {
                continue;
            }
            LOG_CONSOLE("rx pkt number is {}", nbRx);
            m_capSummary.capPktCount += nbRx;
            nbRxEnque = rte_ring_sp_enqueue_burst(m_pktRings[i], (void**)mbufs, nbRx, NULL);
            if(nbRxEnque < nbRx)
            {
                m_capSummary.dropPktCount += (nbRx - nbRxEnque);
                for(int i = nbRxEnque; i < nbRx; ++i)
                {
                    rte_pktmbuf_free(mbufs[i]);
                }
            }
        }
    }
    LOG_INFO("Exit PollingCapture");
}
