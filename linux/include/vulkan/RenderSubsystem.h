#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"
#include "../game/ModelDefines.h"

#include "vWindow.h"
#include "vDevice.h"
#include "vRenderer.h"

namespace PL
{   
    class RenderSubsystem : public IUncopiable, public IDependent
    {
    public:
        // IDependent
        const static std::string _DEP_ID; 
        // inline const static std::vector<std::string> _DEP_NEEDED_DEPS = {
        //     vWindow::_DEP_ID, vDevice::_DEP_ID, vRenderer::_DEP_ID
        // };
        std::vector<std::string> GetNeededDependencies()
        {
            return {vWindow::_DEP_ID, vDevice::_DEP_ID, vRenderer::_DEP_ID};
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
            active_window = this->windows[0];
            
            this->device = static_cast<vDevice*>(context[1][0]);
            this->renderer = static_cast<vRenderer*>(context[2][0]);
            this->Initialize();
        }
        void UpdateContext(std::vector<std::vector<IDependent*>> context)
        {
            this->ReceiveWindows(context[0]);
        }
        
        std::string GetDependencyID()
        {
            return this->_DEP_ID;
        }

        //static LoggerSubsystem GLOBAL_LOGGER;
        RenderSubsystem() {}
        ~RenderSubsystem();

        void registerRenderModels(std::vector<GameModel*>* toRender) {
            if(toRender == nullptr) return;
            this->renderer->UpdatePipelineModels(toRender);
            
        }

        bool MainTick();

    protected:
        std::vector<vWindow*> windows;
        vWindow* active_window;
        vDevice* device;
        vRenderer* renderer;

        void Initialize();

        //inline static const LoggerType LOGGER_TYPE = (LOGGER_TYPE_STDOUT);
        //inline static const LoggerVerbosity LOGGER_VERBOSITY = LOGGER_VERB_ALL;
    };
}