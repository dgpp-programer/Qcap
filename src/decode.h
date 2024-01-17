#ifndef DECODE
#define DEOCDE

#include <rte_ether.h>
#include <rte_ip.h>
#include <rte_tcp.h>
#include <rte_udp.h>
#include <rte_ring.h>
#include <rte_mbuf.h>
#include <atomic>

#include "log.h"
#include "help.h"
#include "thread.h"
#include "protocol.h"
#include "flow.h"
#include "flowCache.h"
#include "config.h"

#define BURST_SIZE 128


//一层协议为一个node
struct ProtoNode
{
    ProtoNode()
    {
        m_protoType = 0xffff;
        m_nodeData = nullptr;
    }
    void Set(uint16_t protoType, uint8_t* nodeData)
    {
        m_protoType = protoType;
        m_nodeData = nodeData;
    }

    void Clear()
    {
        m_protoType = 0xffff;
        m_nodeData = nullptr;
    }

    uint16_t m_protoType;
    uint8_t* m_nodeData;
};

typedef std::vector<ProtoNode> ProtoList;

/* 
    ProtoListSet 一组数据包的所有协议集合
eg:
| ETH | IP   | TCP | HTTP |...
| ETH | IPV6 | UDP | ...
| ETH | IPV4 | UDP | ...
*/
typedef std::vector<ProtoList> ProtoListSet;


class Decode :public Thread
{
public:
    Decode(uint32_t flowOutTime, DecodeMode dm = SINGLE_PACKET);
    ~Decode();
    void Start(int coreIndex);
    void Stop();
    bool Running() { return m_running.load();}
    static void* Run(void* args);
    void DecodeProcess();
    void SetPktRing(struct rte_ring* pktRing);

private:
    void RecognizeProto(struct rte_mbuf** mbuf, int count, ProtoListSet& ProtoListSet);
    void ParseSinglePkt(ProtoListSet& ProtoListSet);
    void ParseFlow(ProtoListSet& ProtoListSet);
    void RecognizeFlow(ProtoList& protoList, FlowKey& key, FlowContext& ctx);
    void InsertFlowTable(FlowKey& key,FlowContext& ctx);
    void InsertFlow(FlowKey& key,FlowContext *ctx);
    void DumpFlow(const FlowKey& key, uint64_t now = 0);
    void DumpAllFlow();
    void EraseFlow(FlowKey& key);
    void ClearExpiredFlow();
    void ReleasePkt(struct rte_mbuf** mbuf, uint16_t count);

private:
    DecodeMode m_decodeMode;
    uint32_t m_flowOutTime;
    std::atomic<bool> m_running;
    struct rte_ring* m_pktRing;
    FlowTable m_flowTable;
    FlowCache m_flowCache;
};

#endif //DECODE