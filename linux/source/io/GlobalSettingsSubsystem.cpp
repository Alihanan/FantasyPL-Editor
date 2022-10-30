#include "../../include/io/GlobalSettingsSubsystem.h"

namespace PL
{
    GlobalSettingsSubsystem::GlobalSettingsSubsystem()
    {
        this->initializeDefaultSettings();
    }

    void GlobalSettingsSubsystem::initializeDefaultSettings()
    {
        for(auto& [key, value] : DEFAULT_SETTINGS)
        {
            this->allSettings[key] = value;
        }
    }

}