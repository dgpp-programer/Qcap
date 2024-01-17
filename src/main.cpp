
#include <vector>
#include <string>
#include <signal.h>
#include <execinfo.h>
#include <cxxabi.h>
#include <dlfcn.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core_dpdk.h"
#include "decode.h"
#include "log.h"
#include "config.h"
#include "help.h"

#define BACKTRACE_SIZE 20

static std::vector<Decode*> g_decodes;
static std::vector<CoreDpdk*> g_coreDpdks; 
static std::vector<int> core = {1, 2, 3, 4};
static int coreIndex = 0;

void QcapExit()
{
    CapSummary capSummary;
    bool need_dump = false;
    for(auto& coreDpdk : g_coreDpdks)
    {
        if(coreDpdk->Running())
        {
            need_dump = true;
            coreDpdk->Stop();
            capSummary.capPktCount += coreDpdk->GetCapSummary().capPktCount;
            capSummary.dropPktCount += coreDpdk->GetCapSummary().dropPktCount;
        }
    }
    LOG_INFO("cap packet number: {}, drop packet number: {}", capSummary.capPktCount, capSummary.dropPktCount);
    for(auto& decode : g_decodes)
    {
        if(decode->Running())
        {
            decode->Stop();
        }
    }
    return;
}



void SigHandle(int sig)
{
    LOG_INFO("SIGHANDLE {}", sig);
    if(sig == SIGINT)
    {
        LOG_INFO("Recv Signal signal = {}, it`s ctrl + C",sig);
    }
    else if(sig == SIGSEGV || sig == SIGABRT)
    {
        void* buf[BACKTRACE_SIZE];
        int n = backtrace(buf, BACKTRACE_SIZE);
        char** symbols = backtrace_symbols(buf, n);
        if(NULL == symbols)
        {
            LOG_ERROR("backtrace symbols failed");
            return;
        }
        LOG_ERROR("================= Qcap crush dump info=================");
        for (int i = 0; i < n; i++) 
        {
            std::string s = demangle(symbols[i]);
            LOG_ERROR("{}", s);
        }
        free(symbols);
    }
    QcapExit();
}

void SignalRegister()
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, SigHandle);
    signal(SIGSEGV, SigHandle);
    signal(SIGABRT, SigHandle);
}

int main(int argc, char* argv[])
{
    LOG_INFO("Qcap start");
    SignalRegister();
    std::string tip("");
    if(!Config::GetConfig()->InitConfig(tip))
    {
        LOG_ERROR("{}", tip);
        return 0;
    }
    Logger::Instance()->SetLogLevel();
    Logger::Instance()->SetRealTimeFlush();
    std::vector<std::string> tips;
    Config::GetConfig()->dumpConfig(tips);
    for(auto& t : tips)
    {
        LOG_INFO("{}", t);
    }
    
    CoreDpdk* cap = new CoreDpdk();
    g_coreDpdks.push_back(cap);
    if(!cap->Init())
    {
        LOG_ERROR("Qcap Core Dpdk Init Failed!");
        return 0;
    }
    uint8_t decodeThreadNum = cap->GetRxQueNumber();
    for(int i = 0; i < decodeThreadNum; ++i)
    {
        Decode* decode = nullptr;
        uint64_t flowOutTime = Config::GetConfig()->getValue<uint64_t>("flowOutTime");
        DecodeMode mode = (DecodeMode)Config::GetConfig()->getValue<uint64_t>("decodeMode");
        decode = new Decode(flowOutTime, mode);
        decode->SetPktRing(cap->m_pktRings[i]);
        decode->Start(core[coreIndex++]);
        g_decodes.push_back(decode);
    }
    cap->Start();
    return 0;
}