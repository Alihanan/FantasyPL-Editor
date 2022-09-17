#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"
#include "../io/logger.h"
#include "vWindow.h"
#include "vDevice.h"


namespace PL
{
    class vApplication : public IUncopiable, public IDependent
    {
    public:
        // IDependent
        inline const static std::string _DEP_ID = "vApplication";
        inline const static std::vector<std::string> _DEP_NEEDED_DEPS = {
            vWindow::_DEP_ID, vDevice::_DEP_ID
        };
        std::vector<std::string> GetNeededDependencies()
        {
            return this->_DEP_NEEDED_DEPS;
        }

        void ReceiveWindows(std::vector<IDependent*> context)
        {
            this->windows.clear();
            for(auto idep : context)
            {
                auto win = static_cast<vWindow*>(idep);
                this->windows.push_back(win);
            }
        }

        void ReceiveContext(std::vector<std::vector<IDependent*>> context)
        {          
            this->ReceiveWindows(context[0]);
            this->device = static_cast<vDevice*>(context[0][0]);
            this->Initialize();
        }
        void UpdateContext(std::vector<std::vector<IDependent*>> context)
        {
            this->ReceiveWindows(context[0]);
        }
        
        bool IsSingleton()
        {
            return true;
        }

        std::string GetDependencyID()
        {
            return this->_DEP_ID;
        }

        static Logger GLOBAL_LOGGER;
        vApplication() {}

        ~vApplication();
        void MainLoop();

    protected:
        std::vector<vWindow*> windows;
        vDevice* device;
        void Initialize();

        inline static const LoggerType LOGGER_TYPE = (LOGGER_TYPE_STDOUT);
        inline static const LoggerVerbosity LOGGER_VERBOSITY = LOGGER_VERB_ALL;
    };
}