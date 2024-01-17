#ifndef CONFIG_H
#define CONFIG_H

#include <unordered_map>
#include <stdexcept>
#include <any>
#include <variant>
#include <string>
#include <vector>

#define CONFIG_PATH "../config.conf"
#define DEFAULT_FLOW_OUT_TIME 10 * 1000LL //60s

enum DecodeMode 
{
    UNKNOW = 0,
    SINGLE_PACKET = 1,  // 单包模式
    FLOW = 2,           // 会话模式
};

enum ConfigType
{
    CFG_UINT64 = 1,
    CFG_BOOL = 2,
    CFG_STRING = 3,
};
class Config;

static Config* g_config = nullptr;

template <typename T>
static std::string toString(const T& value) 
{
    return std::to_string(value);
}

class Config 
{
public:
    Config() = default;

    ~Config() = default;

    static Config* GetConfig()
    {
        if(!g_config)
        {
            g_config = new Config;
        }
        return g_config;
    }
 
    bool InitConfig(std::string& tip)
    {
        std::string filename = CONFIG_PATH;
        std::unordered_map<std::string, std::string> configData;
        if(!ParseConfigFile(filename, configData, tip))
        {
            return false; 
        }
        FillConfig(configData, tip);
        return true;
    }

    void dumpConfig(std::vector<std::string>& config)
    {
        for(auto& cfg : m_configMap)
        {
            std::string value("");
            if (cfg.second.type() == typeid(std::string)) 
            {
                value = std::any_cast<std::string>(cfg.second);
            } 
            else if (cfg.second.type() == typeid(bool)) 
            {
                value = toString(std::any_cast<bool>(cfg.second));
            } 
            else if (cfg.second.type() == typeid(uint64_t)) 
            {
                value = toString(std::any_cast<uint64_t>(cfg.second));
            }
            std::string c = cfg.first + " = " + value;
            config.emplace_back(c);
        }
        return;
    }    

    bool ParseConfigFile(const std::string& filename,
        std::unordered_map<std::string, std::string>& configData,
        std::string& tip)
    {
        std::ifstream configFile(filename);
        if (!configFile.is_open()) 
        {
            tip = "Failed to open config file: " + filename;
            return false;
        }

        std::string line;
        while (std::getline(configFile, line)) 
        {
            if (line.empty() || line[0] == '#')
                continue;

            size_t equalPos = line.find('=');
            if (equalPos == std::string::npos) 
            {
                tip = "Invalid line in config file: " + line;
                return false;
            }
            std::string key = line.substr(0, equalPos);
            std::string value = line.substr(equalPos + 1);
            key.erase(key.begin(), std::find_if(key.begin(), key.end(), [](int ch) {
                return !std::isspace(ch);
                }));
            key.erase(std::find_if(key.rbegin(), key.rend(), [](int ch) {
                return !std::isspace(ch);
                }).base(), key.end());

            value.erase(value.begin(), std::find_if(value.begin(), value.end(), [](int ch) {
                return !std::isspace(ch);
                }));
            value.erase(std::find_if(value.rbegin(), value.rend(), [](int ch) {
                return !std::isspace(ch);
                }).base(), value.end());
            configData[key] = value;
        }
        configFile.close();
        return true;
    }

    bool FillConfig(std::unordered_map<std::string, std::string>& configData,
        std::string& tip)
    {
        for(auto &config : configData)
        {
            if(!m_configType.count(config.first))
            {
                tip = "config " + config.first + "not exist";
                return false;
            }
            ConfigType t = m_configType[config.first];
            switch (t)
            {
            case CFG_UINT64:
            {
                uint64_t value = atol(config.second.c_str());
                setValue<uint64_t>(config.first, value);
                break;
            }
            case CFG_BOOL:
            {
                bool value = true; 
                if(config.second == "false" || config.second == "0")
                {
                    value = false;
                }
                setValue<bool>(config.first, value);
                break;
            }
            case CFG_STRING:
            {
                std::string value = config.second;
                setValue<std::string>(config.first, value);
                break;
            }
            default:
                break;
            }
        }
        return true;
    }

    template<typename T>
    void setValue(const std::string& key, const T& value)
    {
        m_configMap[key] = value;
        return;
    }

    template<typename T>
    T getValue(const std::string& key) const 
    {
        auto it = m_configMap.find(key);
        if (it != m_configMap.end())
        {
            return std::any_cast<T>(it->second);
        }
        assert(false);
    }

private:
    std::unordered_map<std::string, std::any> m_configMap = 
    {
        std::make_pair("decodeMode", SINGLE_PACKET),
        std::make_pair("flowOutTime", DEFAULT_FLOW_OUT_TIME),
        std::make_pair("logLevel", "info"),
        std::make_pair("realTimeFlushLog", true),
        std::make_pair("isPromiscuous", true)
    };
    std::unordered_map<std::string, ConfigType> m_configType = 
    {
        std::make_pair("decodeMode", CFG_UINT64),
        std::make_pair("flowOutTime", CFG_UINT64),
        std::make_pair("logLevel", CFG_STRING),
        std::make_pair("realTimeFlushLog", CFG_BOOL),
        std::make_pair("isPromiscuous", CFG_BOOL)
    };
};

#endif //CONFIG_H