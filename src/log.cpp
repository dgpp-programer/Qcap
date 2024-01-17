#include "log.h"


Logger* Logger::Instance()
{
    static Logger* logger = new Logger;
    return logger;
}
Logger::Logger()
{
    m_sysLogFileSize    = system_log_file_size;
    m_sysLogFileNum     = system_log_file_num;
    m_pktLogFileSize    = pkt_log_file_size;
    m_pktLogFileNum     = pkt_log_file_num;
    m_sysLogPath        = log_dir_path + "system.log";
    m_pktLogPath        = log_dir_path + "packet.log";
    m_sysLevel          = "debug";//日志级别目前最低为debug
    m_pktLevel          = "debug";
    try {
        m_sysLogger = spdlog::create_async<spdlog::sinks::rotating_file_sink_mt>(
            "systemLog", m_sysLogPath, m_sysLogFileSize, m_sysLogFileNum);
        //SetLevel(m_sysLogger, m_sysLevel);
        m_sysLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%l]: %v");

        m_pktLogger = spdlog::create_async<spdlog::sinks::rotating_file_sink_mt>(
            "pktLog", m_pktLogPath, m_pktLogFileSize, m_pktLogFileNum);
        //SetLevel(m_pktLogger, m_pktLevel);
        m_pktLogger->set_pattern("%v");
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cout << "[Logger] Initialization failed: " << ex.what() << std::endl;
    }
}
Logger::~Logger()
{
    spdlog::shutdown();
}


std::shared_ptr<spdlog::logger> Logger::GetSysLogger()
{
    return m_sysLogger;
}
std::shared_ptr<spdlog::logger> Logger::GetPktLogger()
{
    return m_pktLogger;
}

void Logger::SetRealTimeFlush()
{
    if(Config::GetConfig()->getValue<bool>("realTimeFlushLog"))
    {
        spdlog::flush_every(std::chrono::seconds(1));
    }
    return;
}

void Logger::SetLogLevel()
{
    m_sysLevel = Config::GetConfig()->getValue<std::string>("logLevel");
    m_pktLevel = Config::GetConfig()->getValue<std::string>("logLevel");
    SetLevel(m_sysLogger, m_sysLevel);
    SetLevel(m_pktLogger, m_pktLevel);
    return;
}

void Logger::SetLevel(std::shared_ptr<spdlog::logger> logger,std::string level)
{
    if (level == "trace") {
        logger->set_level(spdlog::level::trace);
        logger->flush_on(spdlog::level::trace);
    } else if (level == "debug") {
        logger->set_level(spdlog::level::debug);
        logger->flush_on(spdlog::level::debug);
    } else if (level == "info") {
        logger->set_level(spdlog::level::info);
        logger->flush_on(spdlog::level::info);
    } else if (level == "warn") {
        logger->set_level(spdlog::level::warn);
        logger->flush_on(spdlog::level::warn);
    } else if (level == "error") {
        logger->set_level(spdlog::level::err);
        logger->flush_on(spdlog::level::err);
    } else if (level == "critical") {
        logger->set_level(spdlog::level::critical);
        logger->flush_on(spdlog::level::critical);
    }
}
