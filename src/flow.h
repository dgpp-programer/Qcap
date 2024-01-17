#ifndef FLOW_H
#define FLOW_H

#include <stdint.h>
#include "protocol.h"
#include "help.h"

#define OUTTIME_FLOW_TIME 60

struct FlowKey
{
    FlowKey()
    {
        m_ip1       = 0;
        m_ip2       = 0;
        m_port1     = 0;
        m_port2     = 0;
        m_proto     = 0;
    }
    FlowKey(uint32_t ip1, uint32_t ip2, uint16_t port1, uint16_t port2, uint8_t proto):
        m_ip1(ip1), m_ip2(ip2), m_port1(port1), m_port2(port2), m_proto(proto)
    {
        if(m_ip1 < m_ip2)
        {
            uint32_t temp = m_ip1;
            m_ip1 = m_ip2;
            m_ip2 = temp;
        }
        if(m_port1 < m_port2)
        {
            uint16_t temp = m_port1;
            m_port1 = m_port2;
            m_port2 = temp;
        }
    }

    uint32_t GetHash() const
    {
        return  m_ip1 ^ m_ip2 ^
                m_port1 ^ m_port2 ^
                m_proto;
    }

    uint32_t m_ip1;
    uint32_t m_ip2;
    uint16_t m_port1;
    uint16_t m_port2;
    uint16_t m_proto;
};

struct FlowContext
{
    FlowContext() :
        m_ip1(0),
        m_ip2(0),
        m_port1(0),
        m_port2(0),
        m_proto(0),
        m_curBytes(0),
        m_ip1To2TotalBytes(0),
        m_ip2To1TotalBytes(0),
        m_ip1To2Pkts(0),
        m_ip2To1Pkts(0),
        m_flowBeginTime(0),
        m_flowEndTime(0),
        m_isSyn(false),
        m_SYNCount(0),
        m_lastIsSYN(false),
        m_isFIN(false),
        m_FINCount(0),
        m_lastIsFIN(false)
    {
    }
    uint32_t m_ip1;
    uint32_t m_ip2;
    uint32_t m_port1;
    uint32_t m_port2;
    uint16_t m_proto;
    uint32_t m_curBytes;
    uint32_t m_ip1To2TotalBytes;
    uint32_t m_ip2To1TotalBytes;
    uint32_t m_ip1To2Pkts;
    uint32_t m_ip2To1Pkts;
    uint64_t m_flowBeginTime;
    uint64_t m_flowEndTime;
    bool    m_isSyn;
    uint8_t m_SYNCount;
    bool    m_lastIsSYN;
    bool    m_isFIN;
    uint8_t m_FINCount;
    bool    m_lastIsFIN;
};

struct FlowKeyFunc
{
    uint32_t operator()(const FlowKey &key) const 
    {
        return key.GetHash();
    }
};


struct FlowKeyEqualFunc
{
    bool operator () (const FlowKey &key1, const FlowKey &key2) const
    {
        return  key1.GetHash() == key2.GetHash();
    }
};

typedef std::unordered_map<FlowKey, FlowContext*, FlowKeyFunc, FlowKeyEqualFunc> FlowTable;
#endif //FLOW_H