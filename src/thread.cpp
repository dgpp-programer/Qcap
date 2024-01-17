
#include "thread.h"
#include "log.h"

Thread::Thread()
{
    m_createSuc =  false;
    m_threadId = 0;
    m_threadName = "UNKNOW";
}

Thread::~Thread()
{

}

int Thread::Create(threadFunc func,void* arg, int coreIndex)
{
    pthread_t threadId;
    int ret = pthread_create(&threadId, NULL, func, arg);
    if(ret != 0)
    {
        LOG_ERROR("pthread_create fialed! ret = {}",ret);
        return ret;
    }
    m_threadId = threadId;
    m_createSuc = true;
    if(coreIndex == -1)
    {
        return 0;
    }
    if(!bindThreadToCore(threadId, coreIndex))
    {
        LOG_WARN("bind thread to core failed, therad id = {}", threadId);
    }
    return 0;
}

uint16_t Thread::GetThreadId()
{
    return m_createSuc ? m_threadId : 0;
}

std::string Thread::GetThreadName()
{
    return m_createSuc ? m_threadName : "";
}

int Thread::SetThreadName(const std::string& threadName)
{
    if(!m_createSuc)
    {
        LOG_ERROR("pthread_setname_np failed! thread not created");
        return -1;
    }
    m_threadName = threadName;
    int ret = pthread_setname_np(m_threadId, m_threadName.c_str());
    if(ret != 0)
    {
        LOG_ERROR("pthread_setname_np failed! ret = {}", ret);
        return ret;
    }
    return 1;
}

void Thread::Join()
{
    if(!m_createSuc) 
    {
        return ;
    }
    void *threadResult;
    int rt = pthread_join(m_threadId, &threadResult);
    if(0 != rt) 
    {
        LOG_ERROR("pthread_join thread fail, return = {}, info = {}", rt, (char*)threadResult);
    }
    return;
}

void Thread::Detach()
{
    if(!m_createSuc) 
    {
        return ;
    }
    int rt = pthread_detach(m_threadId);
    if(0 != rt) 
    {
        LOG_ERROR("pthread_detach thread fail, ret = {}", rt);
    }
    return;
}