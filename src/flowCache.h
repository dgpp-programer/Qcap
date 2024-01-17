#ifndef FLOW_CACHE_H
#define FLOW_CACHE_H

#include <unordered_map>
#include <list>

#include "flow.h"
#include "assert.h"



class FlowCache {
public:
    FlowCache(int capacity = 0) : m_capacity(capacity)
    {
        m_cacheMap.clear();
        m_cacheList.clear();
    }

    void Update(const FlowKey& key, uint64_t value)
    {
        assert(m_cacheMap.find(key) != m_cacheMap.end());
        auto iter = m_cacheMap.find(key);
        m_cacheList.erase(iter->second);
        m_cacheList.push_back(std::make_pair(key, value));
        ListIter newIter = std::prev(m_cacheList.end());
        iter->second = newIter;
        return ;
    }

    bool Insert(const FlowKey& key, const uint64_t value)
    {
        assert(m_cacheMap.find(key) == m_cacheMap.end());
        if(0 != m_capacity && m_cacheMap.size() > m_capacity)
        {
            return false;
        }
        m_cacheList.push_back(std::make_pair(key, value));
        ListIter iter = std::prev(m_cacheList.end());
        m_cacheMap.insert(std::make_pair(key, iter));
        return true;
    }

    bool Count(FlowKey& key)
    {
        return m_cacheMap.find(key) != m_cacheMap.end();
    }

    bool Expired(FlowKey& key, uint64_t now)
    {
        return m_cacheMap[key]->second >= now;
    }

    bool GetExpired(FlowKey& key, uint64_t now)
    {
        if(m_cacheMap.empty())
        {
            return false;
        }
        if(m_cacheList.front().second <= now)
        {
            key = m_cacheList.front().first;
            return true;
        }
        return false;
    }

    void Delete(FlowKey& key)
    {
        assert(m_cacheMap.find(key) != m_cacheMap.end());
        m_cacheList.erase(m_cacheMap[key]);
        m_cacheMap.erase(key);
    }

private:
    int m_capacity;
    typedef std::list<std::pair<FlowKey, uint64_t>>::iterator ListIter;
    std::unordered_map<FlowKey, ListIter, FlowKeyFunc, FlowKeyEqualFunc> m_cacheMap;
    std::list<std::pair<FlowKey, uint64_t>> m_cacheList;
};


#endif //FLOW_CACHE_H