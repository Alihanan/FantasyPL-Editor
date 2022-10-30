#pragma once

#include <map>
#include "LoggerSubsystem.h"
#include <any>

namespace PL
{
    inline static const std::string SETTINGS_PATH = "settings_pldk.json";

    enum PLSetting
    {
        SETG_LOGGER_TYPE, SETG_LOGGER_VERBOSITY
    };

    std::map<PLSetting, std::any> DEFAULT_SETTINGS =
    {
        {SETG_LOGGER_TYPE, LOGGER_TYPE_STDOUT},
        {SETG_LOGGER_VERBOSITY, LOGGER_VERB_ALL}
    };


    class GlobalSettingsSubsystem
    {
    public:
        GlobalSettingsSubsystem();
        ~GlobalSettingsSubsystem()
        {
            this->allSettings.clear();
        }

        std::any GET(PLSetting setting) {return this->allSettings[setting];}

    protected:
        void initializeDefaultSettings();
        void readFromFile();
        std::map<PLSetting, std::any> allSettings {};
        

    };
}