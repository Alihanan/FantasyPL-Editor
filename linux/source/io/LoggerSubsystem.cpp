#include "../../include/io/LoggerSubsystem.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <ctime>

using std::to_string;

namespace PL
{
    LoggerSubsystem::LoggerSubsystem(LoggerType type, LoggerVerbosity verb) : 
        LoggerSubsystem(static_cast<long>(type), verb)       
    {}

    LoggerSubsystem::LoggerSubsystem(long type, LoggerVerbosity verb)
    : type(type), verbosity(verb)
    {
        this->initialize();
    }


    void LoggerSubsystem::setupType(LoggerType ltype)
    {
        switch (ltype)
        {
        case LOGGER_TYPE_STDOUT:
        {
            this->stream.push_back(&std::cout);
            break;
        }
        case LOGGER_TYPE_FILE:
        {
            std::cout << "Creating...\n";
            std::ofstream* fstrm = new std::ofstream();


            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
            

            std::cout << ss.str() << std::endl;
            std::filesystem::create_directory("log/");
            fstrm->open("log/" + ss.str());

            this->stream.push_back(fstrm);
            break;
        }
        default:
            break;
        }
    }


    void LoggerSubsystem::initialize()
    {
        for(long i = 0; i < LOGGER_TYPE_SIZE; i++)
        {
            long lType = (1 << i);
            bool isType = lType & this->type;
            if(isType)
            {
                this->setupType(static_cast<LoggerType>(lType));
            }
        }
    }

    LoggerSubsystem::~LoggerSubsystem()
    {
        for(std::ostream* strm : this->stream)
        {
            if(dynamic_cast<std::ofstream*>(strm) != NULL){
                std::ofstream* strmp = dynamic_cast<std::ofstream*>(strm);
                strmp->close();
                delete strmp;
            }
        }
        
    }

    void LoggerSubsystem::log(std::string msg) const
    {
        if(this->verbosity == LOGGER_VERB_NONE)
        {
            return;
        }

        if(static_cast<int>(this->verbosity) > static_cast<int>(this->currentSeverity))
        {
            return;
        }

        for(std::ostream* strm : this->stream)
        {        
            *strm << msg;
        }       
    }
    void LoggerSubsystem::log(const char* msg) const
    {
        this->log(std::string(msg));
    }

    LoggerSubsystem& LoggerSubsystem::operator<< (long input) {
        this->log(to_string(input));
        return *this;
    }
    LoggerSubsystem& LoggerSubsystem::operator<< (size_t input) {
        this->log(to_string(input));
        return *this;
    }




}