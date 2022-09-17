#pragma once

#include "../io/logger.h"
#include "vWindow.h"


namespace PL
{
    class vApplication : public IUncopiable, public IDependent
    {
    public:
        // IDependent
        inline const static std::string _DEP_ID = "vApplication";
        inline const static std::vector<std::string> _DEP_NEEDED_DEPS = {
            vWindow::_DEP_ID
        };
        virtual std::vector<std::string> GetNeededDependencies()
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

        virtual void ReceiveContext(std::vector<std::vector<IDependent*>> context)
        {          
            this->ReceiveWindows(context[0]);
            this->Initialize();
        }
        virtual void UpdateContext(std::vector<std::vector<IDependent*>> context)
        {
            this->ReceiveWindows(context[0]);
        }
        
        virtual bool IsSingleton()
        {
            return true;
        }

        virtual std::string GetDependencyID()
        {
            return this->_DEP_ID;
        }

        static Logger GLOBAL_LOGGER;
        vApplication() {}

        ~vApplication();
        void MainLoop();

    protected:
        std::vector<vWindow*> windows;
        void Initialize();

        inline static const LoggerType LOGGER_TYPE = (LOGGER_TYPE_STDOUT);
        inline static const LoggerVerbosity LOGGER_VERBOSITY = LOGGER_VERB_ALL;
    };
}