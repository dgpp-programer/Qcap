#ifndef THREAD_H
#define THREAD_H
#include <pthread.h>
#include <vector>
#include <string>


typedef void *(threadFunc) (void *);

class Thread
{

public:
    Thread();
    ~Thread();
    int Create(threadFunc func, void* arg, int coreIndex = -1);
    uint16_t GetThreadId();
    int SetThreadName(const std::string& threadName);
    std::string GetThreadName();
    void Join();
    void Detach();


public:
    bool m_createSuc;
    pthread_t m_threadId;
    std::string m_threadName;
};



#endif //THREAD_H