#ifndef HELP_H
#define HELP_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <fstream>
#include <iostream>
#include <experimental/filesystem>
#include <sys/stat.h>
#include <unistd.h>
#include <chrono>
#include <iomanip>
#include <sys/sysinfo.h>
#include <pthread.h>
#include <stdlib.h>
#include <cstring>
#include <cxxabi.h>



/**
 * @brief 		检测文件是否存在
 * @param[in] 	filename 文件路径
 * @return		不存在返回false，存在返回true
 */
inline bool IsFileExist(const std::string& filename)
{
  return std::experimental::filesystem::exists(filename);
}

/**
 * @brief 		获取文件内容大小
 * @param[in] 	filename 文件路径
 * @return		失败返回-1，成功返回文件内容大小
 */
inline int GetFileSize(const std::string& filename)
{
	struct stat  statbuf;
	int ret = stat(filename.c_str(),&statbuf);
	if(ret != 0) return -1;//获取失败。
	return statbuf.st_size;//返回文件大小。
}

/**
 * @brief 		获取文件内容
 * @param[in] 	path 文件路径 ptr 事先分配的内存
 * @note        
 * @return		失败返回-1，成功返回文件内容大小
 */
inline int ReadFile(const std::string& path, char* ptr, size_t bufSize)
{
    if(!IsFileExist(path))
    {
        return -1;
    }
    size_t fileSize = GetFileSize(path);
    if(fileSize >= bufSize)
    {
        return -1;
    }
    if(!ptr)
    {
        return -1;
    }
    std::ifstream inFile(path, std::ios::in);
    if (!inFile.is_open())
	{
		std::cout << "Open file failed!" << std::endl;
		return -1;
	}
    inFile.read(ptr,fileSize);
    inFile.close();
    return fileSize;
}

/**
 * @brief 		写数据到文件
 * @param[in] 	path 文件路径 ptr 事先分配的内存
 * @note        
 * @return		失败返回-1，成功返回xieru文件内容大小
 */
inline int WriteFile(const std::string& path,const char* ptr, size_t bufSize)
{
    if(!ptr)
    {
        return -1;
    }
    std::ofstream outFile(path, std::ios::out & std::ios::app);
    if (!outFile.is_open())
	{
		std::cout << "Open file failed!" << std::endl;
		return -1;
	}
    outFile.write(ptr, bufSize);
    outFile.close();
    return 0;
}

/**
 * @brief 		秒级时间戳转时间字符串
 * @param[0] 	秒级时间戳
 * @note        
 * @return		时间字符串
 */
inline std::string stampToTimeS(time_t time)
{
    char myStr[25] = { 0 };
    struct tm *mytm;
    mytm = gmtime(&time);
    std::string formatTime = "%Y-%m-%d:%H:%M:%S";
    mytm->tm_hour +=8;//转为北京时间要加8
    strftime(myStr, sizeof(myStr), formatTime.c_str(), mytm);
    std::string stdtime(myStr,sizeof(myStr));
    return stdtime;
}
/**
 * @brief 		毫秒级时间戳转时间字符串，保留两位毫秒
 * @param[0] 	毫秒级时间戳
 * @note        
 * @return		时间字符串
 */
inline std::string stampToTimeMs(long long timestamp) {
    std::chrono::milliseconds ms(timestamp);
    std::chrono::system_clock::time_point tp(ms);
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm* local_time = std::localtime(&t);
    auto ms_part = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()) % 1000;
    std::stringstream ss;
    ss << std::put_time(local_time, "%Y-%m-%d %H:%M:%S:") << std::setw(3) << std::setfill('0') << ms_part.count();
    std::string timeString = ss.str();
    return timeString;
}

/**
 * @brief 		获取Qcap路径
 * @return		Qcap项目路径 /home/Qcap
 */
inline std::string GetQcapPath()
{
    char path[30];
    getcwd(path, 30);
    std::string pathStr(path); 
    size_t pos = pathStr.find_last_of("/");
    pathStr = pathStr.substr(0,pos);
    return pathStr;// /home/Qcap
}

/**
 * @brief 		获取当前时间戳(s)
 * @return		秒时间戳
 */
inline uint32_t GetCurTimeS()
{
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

/**
 * @brief 		获取当前时间戳(ms)
 * @return		毫秒时间戳
 */
inline uint64_t GetCurTimeMs()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

/**
 * @brief 		获取当前时间戳(micro)
 * @return		微秒时间戳
 */
inline uint64_t GetCurTimeMicro()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

/**
 * @brief 		获取当前时间戳(ns)
 * @return		纳秒时间戳
 */
inline uint64_t GetCurTimeNs()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

/**
 * @brief 		获取当前标准时间格式字符串
 * @return		时间字符串 %Y-%m-%d %H:%M:%S
 */
inline std::string GetCurTimeString() 
{
    std::string timeFmt = "%Y-%m-%d_%H-%M-%S";
    std::time_t c_time_t = GetCurTimeS();
    char mbstr[100];
    if(std::strftime(mbstr, sizeof(mbstr), timeFmt.c_str(), std::localtime(&c_time_t))) {
        std::string timeFmt(mbstr);
        return timeFmt;
    }
    return "";
} 

/**
 * @brief 		通过数据包获取key,目前仅支持ipv4
 * @return		返回由目的ip和源ip运算得到的key进行分流
 */
#define MAC_HEADER_LEN 14
#define IPV4_HEADER_LEN_EXCEPT_IP (20 - 8)
inline uint32_t GetKeyByPkt(uint8_t* pkt, uint16_t threadNum) 
{
    uint32_t ip1 = *(uint32_t*)(pkt + MAC_HEADER_LEN + IPV4_HEADER_LEN_EXCEPT_IP);
    uint32_t ip2 = *(uint32_t*)(pkt + MAC_HEADER_LEN + IPV4_HEADER_LEN_EXCEPT_IP + 4);
    return (ip1 + ip2) % threadNum;
}

/**
 * @brief 		mac地址生成hash，需要由外部保证数组长度
 */
inline void HashMac(uint32_t& hash, const uint8_t* mac)
{
    hash = (hash << 8) - (hash << 1) - hash + mac[5];
    hash = (hash << 8) - (hash << 1) - hash + mac[4];
    hash = (hash << 8) - (hash << 1) - hash + mac[3];
    hash = (hash << 8) - (hash << 1) - hash + mac[2];
    hash = (hash << 8) - (hash << 1) - hash + mac[1];
    hash = (hash << 8) - (hash << 1) - hash + mac[0];
}

/**
 * @brief 	主机序ip地址生成hash，需要由外部保证数组长度
 */
inline void HashHostIpv4(uint32_t& hash, const uint8_t* ipv4)
{
    hash = (hash << 8) - (hash << 1) -hash + ipv4[3];
    hash = (hash << 8) - (hash << 1) -hash + ipv4[2];
    hash = (hash << 8) - (hash << 1) -hash + ipv4[1];
    hash = (hash << 8) - (hash << 1) -hash + ipv4[0];
}

/**
 * @brief 	网络序ip地址生成hash，需要由外部保证数组长度
 */
inline void HashNetIpv4(uint32_t& hash, const uint8_t* ipv4)
{
    hash = (hash << 8) - (hash << 1) -hash + ipv4[0];
    hash = (hash << 8) - (hash << 1) -hash + ipv4[1];
    hash = (hash << 8) - (hash << 1) -hash + ipv4[2];
    hash = (hash << 8) - (hash << 1) -hash + ipv4[3];
}

/**
 * @brief 	主机序端口port生成hash，需要由外部保证数组长度
 */
inline void HashHostPort(uint32_t hash, const uint8_t* port)
{
    hash = (hash << 8) - (hash << 1) -hash + port[1];
    hash = (hash << 8) - (hash << 1) -hash + port[0];
}

/**
 * @brief 	网络序端口port生成hash，需要由外部保证数组长度
 */
inline void HashNetPort(uint32_t hash, const uint8_t* port)
{
    hash = (hash << 8) - (hash << 1) -hash + port[0];
    hash = (hash << 8) - (hash << 1) -hash + port[1];
}

/**
 * @brief 	传输层协议号生成hash，需要由外部保证数组长度
 */
inline void HashProto(uint32_t hash, const uint8_t proto)
{
    hash = (hash << 8) - (hash << 1) -hash + proto;
}

/**
 * @brief 		网络字节序mac地址格式化为字符串
 */
inline std::string FormatNetMacAddr(uint8_t* mac)
{
    char macStr[25];
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", 
        (mac)[5], (mac)[4], (mac)[3], (mac)[2], (mac)[1], (mac)[0]);
    return macStr;
}

/**
 * @brief 		mac地址格式化为字符串
 */
inline std::string FormatMacAddr(uint8_t* mac)
{
    char macStr[25];
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", 
        (mac)[0], (mac)[1], (mac)[2], (mac)[3], (mac)[4], (mac)[5]);
    return macStr;
}

/**
 * @brief 		网络序Ip地址格式化为字符串
 */
inline std::string FormatNetIpv4Addr(uint32_t ip)
{
    struct in_addr inAddr;
    inAddr.s_addr = ntohl(ip);
    std::string ipStr(inet_ntoa(inAddr));
    return ipStr;
}

/**
 * @brief 		主机序Ip地址格式化为字符串
 */
inline std::string FormatIpv4Addr(uint32_t ip)
{
    struct in_addr inAddr;
    inAddr.s_addr = ip;
    std::string ipStr(inet_ntoa(inAddr));
    return ipStr;
}

/**
 * @brief 		去除字符串中的空格
 */
inline char* trim(char* str)
{
    if (str == NULL || *str == '\0')
    {
        return str;
    }

    int len = strlen(str);
    char *p = str + len - 1;
    while (p >= str  && isspace(*p))
    {
        *p = '\0';
        --p;
    }

    return str;
}

/**
 * @brief 		获取系统中实际可以使用的核心
 */
static int GetSystemCoreNumber()
{
    return get_nprocs();
}

/**
 * @brief 		线程绑定核心
 */
static bool bindThreadToCore(pthread_t threadId, int coreIndex) 
{    
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(coreIndex, &cpuset);
 
    int ret = pthread_setaffinity_np(threadId, sizeof(cpu_set_t), &cpuset);
    if (ret != 0)
        return false;
    ret = pthread_getaffinity_np(threadId, sizeof(cpu_set_t), &cpuset);
    if (ret != 0)
        return false;
 
    return true;
}

/**
 * @brief 		堆栈符号转为可读字符串
 */
static std::string demangle(const char* symbol)
{
    std::string symbolStr(symbol);
    size_t startPos;
    size_t endPos;
    std::string addr;
    std::string result;
    std::array<char, 128> buffer;
    startPos = symbolStr.find_first_of('(');
    endPos = symbolStr.find_first_of('+');
    std::string temp;
    if (startPos != std::string::npos && endPos != std::string::npos && endPos > startPos)
    {
        temp = symbolStr.substr(startPos + 1, endPos - startPos - 1);
    }
    if(0 == temp.length())
    {
        return std::string(symbol);
    }
    result = abi::__cxa_demangle(temp.c_str(), NULL, NULL, NULL);
    result.append(" ");
    startPos = symbolStr.find('[');
    endPos = symbolStr.find(']');
    if (startPos != std::string::npos && endPos != std::string::npos && endPos > startPos) 
    {
        addr = symbolStr.substr(startPos + 1, endPos - startPos - 1);
    }
    std::string cmd = "addr2line -e Qcap " + addr;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (pipe) 
    {
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) 
        {
            result += buffer.data();
        }
        pclose(pipe);
    }
    result.erase(result.size() - 1);
    return result;
}

#endif //HELP_H