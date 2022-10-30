#pragma once
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <map>

namespace PL
{
    typedef enum _LoggerType
    {
        _LOGGER_TYPE_STDOUT = 1L << 0,
        _LOGGER_TYPE_FILE = 1L << 1
    } LoggerType;

    inline LoggerType operator|(LoggerType a, LoggerType b)
    {
        return static_cast<LoggerType>(static_cast<long>(a) | static_cast<long>(b));
    }    

    static const long LOGGER_TYPE_SIZE = 2;
    static const LoggerType LOGGER_TYPE_STDOUT = _LOGGER_TYPE_STDOUT;
    static const LoggerType LOGGER_TYPE_FILE = _LOGGER_TYPE_FILE;

    enum LoggerVerbosity
    {
        LOGGER_VERB_ALL,
        LOGGER_VERB_ERROR_ONLY,
        LOGGER_VERB_NONE
    };

    enum LoggerSeverity
    {
        LOG_MSG_INFO,
        LOG_MSG_ERROR
    };

    inline static std::map<LoggerSeverity, const char*> _LOGGER_SEV_NAMES = 
    {
        {LOG_MSG_INFO, "INFO"}, 
        {LOG_MSG_ERROR, "ERROR"}
    };

    /**
     * @brief Logger works simply, but not safely:
     * 1) Initialize GLOBAL_LOGGER in one of the .cpp files 
     * 2) When needed to log:
     *      2.1) GLOBAL_LOGGER << verbosity
     *      2.2) GLOBAL_LOGGER << string      x N
     *      2.3) GLOBAL_LOGGER << std::endl
     * 3) log is submitted
     */
    class LoggerSubsystem
    {
    public:
        static LoggerSubsystem GLOBAL_LOGGER;

        LoggerSubsystem(LoggerType type, LoggerVerbosity verb);
        LoggerSubsystem(long type, LoggerVerbosity verb);
        ~LoggerSubsystem();       
        
    protected:
        std::mutex global_mutex;

        void log(std::string msg) const;
        void log(const char* msg) const;
        void severity(LoggerSeverity sev) {             
            this->currentSeverity = sev;
            
        }   
        void initialize();

        long type;
        LoggerVerbosity verbosity;
        LoggerSeverity currentSeverity = LOG_MSG_INFO;
        std::vector<std::ostream*> stream;


    public:
        void setupType(LoggerType type);

        LoggerSubsystem& operator<< (LoggerSeverity sev) {
            this->global_mutex.lock();
            this->severity(sev);
            std::string str = 
                "[" + std::string(_LOGGER_SEV_NAMES[sev]) + "] ";
            this->log(str);

            return *this;
        }
        LoggerSubsystem& operator<< (std::string const & s) {
            this->log(s);
            return *this;
        }
        LoggerSubsystem& operator<< (int input) {
            return this->operator<<(static_cast<long>(input));
        }
        LoggerSubsystem& operator<< (unsigned int input) {
            return this->operator<<(static_cast<size_t>(input));
        }

        LoggerSubsystem& operator<< (long input);
        LoggerSubsystem& operator<< (size_t input);

        LoggerSubsystem& operator<< (std::ostream&(*pManip)(std::ostream&)) {
            this->log("\n");
            this->severity(LOG_MSG_INFO);
            this->global_mutex.unlock();
            return *this;
        }
    };

}