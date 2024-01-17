#ifndef LOG_H
#define LOG_H

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/fmt/bundled/format.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/async.h"
#include "help.h"
#include "config.h"

#define LOG_TRACE(...)       Logger::Instance()->GetSysLogger().get()->trace(__VA_ARGS__)
#define LOG_DEBUG(...)       Logger::Instance()->GetSysLogger().get()->debug(__VA_ARGS__)
#define LOG_INFO(...)        Logger::Instance()->GetSysLogger().get()->info(__VA_ARGS__)
#define LOG_WARN(...)        Logger::Instance()->GetSysLogger().get()->warn(__VA_ARGS__)
#define LOG_ERROR(...)       Logger::Instance()->GetSysLogger().get()->error(__VA_ARGS__)
#define LOG_CRITICAL(...)    Logger::Instance()->GetSysLogger().get()->critical(__VA_ARGS__)

//统计数据
#define LOG_STAT(...)        Logger::Instance()->GetPktLogger().get()->warn(__VA_ARGS__)

//控制台输出日志
#define LOG_CONSOLE(...)     spdlog::info(__VA_ARGS__);

const uint32_t system_log_file_size = 1024 * 1024; //1M
const uint32_t system_log_file_num  = 1; //2个 0 1
const uint32_t pkt_log_file_size = 1024 * 1024 * 10; //10M
const uint32_t pkt_log_file_num  = 5; //6个
const std::string log_dir_path = GetQcapPath() + "/log/";



class Logger
{
public:
    static Logger* Instance();
    Logger();
    ~Logger();
    

    std::shared_ptr<spdlog::logger> GetSysLogger();
    std::shared_ptr<spdlog::logger> GetPktLogger();
    void InitConfig();
    void SetLogLevel();
    void SetRealTimeFlush();
    void SetLevel(std::shared_ptr<spdlog::logger> logger,std::string level);

private:
    std::shared_ptr<spdlog::logger> m_sysLogger;
    std::shared_ptr<spdlog::logger> m_pktLogger;
    uint32_t m_sysLogFileSize;
    uint16_t m_sysLogFileNum;
    uint32_t m_pktLogFileSize;
    uint16_t m_pktLogFileNum;
    std::string  m_sysLogPath;
    std::string  m_pktLogPath;
    std::string  m_sysLevel;
    std::string  m_pktLevel;
};


#endif //LOG_H