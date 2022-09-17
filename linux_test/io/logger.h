#pragma once
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

namespace PL
{
    typedef enum _LoggerType
    {
        _LOGGER_TYPE_STDOUT = 1L << 0,
        _LOGGER_TYPE_FILE = 1L << 1
    } LoggerType;

    inline long operator|(LoggerType a, LoggerType b)
    {
        return static_cast<long>(static_cast<long>(a) | static_cast<long>(b));
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

    static const char* _LOGGER_SEV_NAMES[] = 
    {
        "INFO", "ERROR"
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
    class Logger
    {
    public:
        static Logger GLOBAL_LOGGER;

        Logger(LoggerType type, LoggerVerbosity verb);
        Logger(long type, LoggerVerbosity verb);
        ~Logger();       
        
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

        Logger& operator<< (LoggerSeverity sev) {
            this->global_mutex.lock();
            this->severity(sev);
            std::string str = 
                "[" + std::string(_LOGGER_SEV_NAMES[static_cast<long>(sev)]) + "] ";
            this->log(str);

            return *this;
        }
        Logger& operator<< (std::string const & s) {
            this->log(s);
            return *this;
        }
        Logger& operator<< (int input) {
            return this->operator<<(static_cast<long>(input));
        }
        Logger& operator<< (unsigned int input) {
            return this->operator<<(static_cast<size_t>(input));
        }

        Logger& operator<< (long input);
        Logger& operator<< (size_t input);

        Logger& operator<< (std::ostream&(*pManip)(std::ostream&)) {
            this->log("\n");
            this->severity(LOG_MSG_INFO);
            this->global_mutex.unlock();
            return *this;
        }
    };

}