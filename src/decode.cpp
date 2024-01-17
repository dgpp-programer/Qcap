#include "decode.h"


Decode::Decode(uint32_t flowOutTime, DecodeMode dm /*=SINGLE_PACKET*/)
    : m_decodeMode(dm),
    m_flowCache(0)
{
    if(flowOutTime >= 1000)
    {
        m_flowOutTime = flowOutTime;
    }
    else
    {
        m_flowOutTime = DEFAULT_FLOW_OUT_TIME;
    }
    m_running = false;
    m_flowTable.clear();
}

Decode::~Decode()
{
}

void Decode::Start(int coreIndex)
{
    m_running = true;
    Create(Run, this, coreIndex);
    std::string threadName = "decode-" + std::to_string(coreIndex);
    SetThreadName(threadName);
    return;
}

void Decode::Stop()
{
    m_running.store(false);
    return;
}

void* Decode::Run(void* args)
{
    ((Decode*)args)->DecodeProcess();
    return nullptr;
}

void Decode::DecodeProcess()
{

    struct rte_mbuf * mbufs[BURST_SIZE];
    int dequeCount = 0;
    ProtoListSet protoListSet;
    while(m_running || !rte_ring_empty(m_pktRing))
    {
        usleep(100);
        protoListSet.clear();
        dequeCount = rte_ring_mc_dequeue_burst(m_pktRing, (void**)mbufs, BURST_SIZE, NULL);
        if(dequeCount == 0)
        {
            continue;
        }
        RecognizeProto(mbufs, dequeCount, protoListSet);
        if(SINGLE_PACKET == m_decodeMode) {
            ParseSinglePkt(protoListSet);
        }
        else if(FLOW == m_decodeMode) {
            ParseFlow(protoListSet);
            ClearExpiredFlow();
        }
        ReleasePkt(mbufs, dequeCount);
    }
    DumpAllFlow();
    LOG_INFO("Exit DecodeProcess");
    return;
}

void Decode::RecognizeProto(struct rte_mbuf** mbufs, int count, ProtoListSet& ProtoListSet)
{
    ProtoList list;
    ProtoNode node;
    for(int i = 0; i < count; ++i)
    {
        list.clear();
        do
        {
            if(mbufs[i] == nullptr || mbufs[i]->data_off < sizeof(EtherHeader))
            {
                LOG_DEBUG("mbufs[i] is 0x{}, data len {}", mbufs[i] == nullptr ? 0 : *(uint64_t*)mbufs[i], mbufs[i]->data_off);
                break;
            }
            uint32_t offset = 0;
            EtherHeader *ehdr = (EtherHeader*)((uint8_t*)(mbufs[i]->buf_addr) + mbufs[i]->data_off);
            node.Set(Q_ETHERIP, (uint8_t*)ehdr);
            list.emplace_back(node);
            offset += sizeof(EtherHeader);
            //目前只处理ipv4
            if(RTE_ETHER_TYPE_IPV4 != ntohs(ehdr->netProtoType))
            {
                break;
            }
            Ipv4Header *iphdr = (Ipv4Header*)(((uint8_t*)(mbufs[i]->buf_addr) + mbufs[i]->data_off + offset));
            node.Set(Q_IP, (uint8_t*)iphdr);
            list.emplace_back(node);
            offset += sizeof(Ipv4Header);

            if(iphdr->protocol == IPPROTO_TCP)
            {
                TcpHeader *tcphdr = (TcpHeader*)(((uint8_t*)(mbufs[i]->buf_addr) + mbufs[i]->data_off + offset));
                node.Set(Q_TCP, (uint8_t*)tcphdr);
                list.emplace_back(node);
                offset += sizeof(TcpHeader);
            }
            else if(iphdr->protocol == IPPROTO_UDP)
            {
                UdpHeader *udphdr = (UdpHeader*)(((uint8_t*)(mbufs[i]->buf_addr) + mbufs[i]->data_off + offset));
                node.Set(Q_UDP, (uint8_t*)udphdr);
                list.emplace_back(node);
            }
        } while (false);
        if(list.size())
        {
            ProtoListSet.emplace_back(list);
        }
    }
    return;
}

void Decode::ParseSinglePkt(ProtoListSet& ProtoListSet)
{
    char srcmac[50];
    int offset = 0;
    std::string record;
    record.reserve(512);
    for(ProtoList& protoList : ProtoListSet)
    {
        record.clear();
        for(ProtoNode& node : protoList)
        {
            if(node.m_protoType == 0xffff)
            {
                continue;
            }
            switch(node.m_protoType)
            {
                case Q_ETHERIP:
                { 
                    EtherHeader *eth = (EtherHeader*)node.m_nodeData;
                    record.append("srcMac:").append(FormatMacAddr((uint8_t*)(eth->srcMac))).append(" ");
                    record.append("dstMac:").append(FormatMacAddr((uint8_t*)(eth->dstMac))).append("\n");
                    break;
                }
                case Q_IP:      
                {
                    Ipv4Header *ipv4 = (Ipv4Header*)node.m_nodeData;
                    record.append("srcIp: ").append(FormatIpv4Addr(ipv4->srcIp)).append(" ");
                    record.append("dstIp: ").append(FormatIpv4Addr(ipv4->dstIp)).append("\n");
                    break;
                }
                case Q_TCP:     
                {
                    TcpHeader *tcp = (TcpHeader *)node.m_nodeData;
                    record.append("srcPort: ").append(std::to_string(ntohs(tcp->srcPort))).append(" ");
                    record.append("dstPort: ").append(std::to_string(ntohs(tcp->dstPort))).append(" ");
                    record.append("[TCP]\n");
                    break;
                }
                case Q_UDP:     
                {
                    UdpHeader *udp = (UdpHeader *)node.m_nodeData;
                    record.append("srcPort: ").append(std::to_string(ntohs(udp->srcPort))).append(" ");
                    record.append("dstPort: ").append(std::to_string(ntohs(udp->dstPort))).append(" ");
                    record.append("[UDP]\n");
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
        LOG_STAT(record);
    }
    return ;
}

void Decode::ParseFlow(ProtoListSet& ProtoListSet)
{
    //ProtoListSet已经全部识别完协议
    uint16_t pktNum = ProtoListSet.size();
    //遍历每个List，从协议里面获取五元组，从而获取FlowKey
    for(auto &protoList : ProtoListSet)
    {
        FlowKey key;
        FlowContext ctx;
        RecognizeFlow(protoList, key, ctx);
        InsertFlowTable(key, ctx);
    }
    return;
}

void Decode::RecognizeFlow(ProtoList& protoList, FlowKey& key, FlowContext& ctx)
{
    for(auto &node : protoList)
    {
        switch(node.m_protoType)
        {
            case Q_IP:
            {
                Ipv4Header *ipv4 = (Ipv4Header*)node.m_nodeData;
                key.m_ip1 = ipv4->srcIp;
                key.m_ip2 = ipv4->dstIp;
                ctx.m_ip1 = ipv4->srcIp;
                ctx.m_ip2 = ipv4->dstIp;
                ctx.m_curBytes = ntohs(ipv4->ipPackLen) + ETHER_HEADER_LEN;
                break;
            }
            case Q_TCP:
            {
                TcpHeader *tcp = (TcpHeader*)node.m_nodeData;
                key.m_port1 = tcp->srcPort;
                key.m_port2 = tcp->dstPort;
                ctx.m_isSyn = tcp->flag & TCP_FLAG_SYN;
                ctx.m_isFIN = tcp->flag & TCP_FLAG_FIN;
                break;
            }
            case Q_UDP:
            {
                UdpHeader *udp = (UdpHeader*)node.m_nodeData;
                key.m_port1 = udp->srcPort;
                key.m_port2 = udp->dstPort;
            }
            default:
                break;    
        }
        key.m_proto = node.m_protoType;
    }
    ctx.m_proto = key.m_proto;
    return;
}

void Decode::InsertFlowTable(FlowKey& key, FlowContext& ctx)
{
    if(ctx.m_isSyn && m_flowTable.count(key) && !m_flowTable[key]->m_lastIsSYN)
    {
        DumpFlow(key);
        EraseFlow(key);
    }
    //会话不存在，创建新会话
    if(!m_flowTable.count(key))
    {
        FlowContext* tCtx = new FlowContext();
        tCtx->m_flowBeginTime = GetCurTimeMs();
        tCtx->m_ip1 = key.m_ip1;
        tCtx->m_ip2 = key.m_ip2;
        tCtx->m_port1 = key.m_port1;
        tCtx->m_port2 = key.m_port2;
        tCtx->m_proto = key.m_proto;
        InsertFlow(key, tCtx);
    }
    assert(m_flowTable.count(key));
    m_flowTable[key]->m_lastIsSYN = ctx.m_isSyn ? true : false;
    m_flowTable[key]->m_SYNCount += ctx.m_isSyn ? 1 : 0;
    FlowContext* statCtx = m_flowTable[key];
    assert(statCtx->m_ip1 == ctx.m_ip1 || statCtx->m_ip2 == ctx.m_ip1);
    //1->2
    if(statCtx->m_ip1 == ctx.m_ip1)
    {
        ++statCtx->m_ip1To2Pkts;
        statCtx->m_ip1To2TotalBytes += ctx.m_curBytes;
    }
    else
    {
        ++statCtx->m_ip2To1Pkts;
        statCtx->m_ip2To1TotalBytes += ctx.m_curBytes;
    }

    auto fctx = m_flowTable[key];
    if(fctx->m_lastIsFIN && 2 == fctx->m_FINCount)
    {
        DumpFlow(key);
        EraseFlow(key);
    }
    fctx->m_FINCount += ctx.m_isFIN ? 1 : 0;
    fctx->m_lastIsFIN = ctx.m_isFIN;
    return;
}

void Decode::InsertFlow(FlowKey& key, FlowContext *ctx)
{
    assert(!m_flowTable.count(key));
    assert(!m_flowCache.Count(key));
    m_flowTable.insert(std::make_pair(key, ctx));
    uint64_t expire = ctx->m_flowBeginTime + m_flowOutTime;
    m_flowCache.Insert(key, expire);
    LOG_DEBUG("insert flow key = {}", key.GetHash());
}

void Decode::DumpFlow(const FlowKey& key, uint64_t now /* = 0*/)
{
    assert(m_flowTable.count(key));
    if(0 == now)
    {
        m_flowTable[key]->m_flowEndTime = GetCurTimeMs();
    }
    else
    {
        m_flowTable[key]->m_flowEndTime = now;
    }
    std::string record;
    record.reserve(512);
    FlowContext* ctx = m_flowTable[key];
    record.append("thread id ").append(std::to_string(m_threadId))
        .append("[").append(stampToTimeMs(ctx->m_flowBeginTime))
        .append(",").append(stampToTimeMs(ctx->m_flowEndTime)).append("] ")
        .append( "ip1: ").append(FormatIpv4Addr(ctx->m_ip1))
        .append(" ip2: ").append(FormatIpv4Addr(ctx->m_ip2))
        .append(" port1: ").append(std::to_string(ntohs(ctx->m_port1)))
        .append(" port2: ").append(std::to_string(ntohs(ctx->m_port2)))
        .append(" proto: ").append(GetProtoStr(ctx->m_proto))
        .append(" ip1->ip2 pkts:").append(std::to_string(ctx->m_ip1To2Pkts))
        .append(" ip2->ip1 pkts:").append(std::to_string(ctx->m_ip2To1Pkts))
        .append(" total pkts:").append(std::to_string(ctx->m_ip1To2Pkts + ctx->m_ip2To1Pkts))
        .append(" ip1->ip2 bytes:").append(std::to_string(ctx->m_ip1To2TotalBytes))
        .append(" ip2->ip1 bytes:").append(std::to_string(ctx->m_ip2To1TotalBytes))
        .append(" total bytes:").append(std::to_string(ctx->m_ip1To2TotalBytes + ctx->m_ip2To1TotalBytes));
    LOG_STAT(record);
    delete ctx;
    return;
}

void Decode::DumpAllFlow()
{
    uint64_t now = GetCurTimeMs();
    for(auto& flow : m_flowTable)
    {
        DumpFlow(flow.first,now);
    }
    m_flowTable.clear();
}

void Decode::EraseFlow(FlowKey& key)
{
    m_flowTable.erase(key);
    m_flowCache.Delete(key);
}

void Decode::ClearExpiredFlow()
{
    uint64_t now = GetCurTimeMs();
    FlowKey key;
    while(m_flowCache.GetExpired(key, now))
    {
        assert(m_flowTable.find(key) != m_flowTable.end());
        DumpFlow(key, now);
        EraseFlow(key);
    }
    return;
}

void Decode::SetPktRing(struct rte_ring* pktRing)
{
    m_pktRing = pktRing;
    return;
}

void Decode::ReleasePkt(struct rte_mbuf** mbuf, uint16_t count)
{
    for(int i = 0; i < count; ++i)
    {
        rte_pktmbuf_free(mbuf[i]);
    }
    return;
}
