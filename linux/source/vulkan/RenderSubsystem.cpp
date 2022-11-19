#include "../../include/vulkan/RenderSubsystem.h"
#include "../../include/io/LoggerSubsystem.h"

namespace PL
{
    /*
    LoggerSubsystem RenderSubsystem::GLOBAL_LOGGER = Logger(
        RenderSubsystem::LOGGER_TYPE, 
        RenderSubsystem::LOGGER_VERBOSITY);

    */
    const std::string RenderSubsystem::_DEP_ID = IDependent::type(RenderSubsystem());    

    void RenderSubsystem::Initialize()
    {
        //this->active_window = this->wi
    }
    RenderSubsystem::~RenderSubsystem()
    {

    }
    bool RenderSubsystem::MainTick()
    {
        
        if(this->active_window->MainShouldClose()) return false;
        vWindow::MainAllWindowGLFWTick();

        //uint32_t winCounter = 0;



        // for(vWindow* win : this->windows)
        // {
        //     bool shouldClose = win->MainShouldClose();
        //     winCounter += (!shouldClose);
        //     if(shouldClose)
        //     {
        //         continue;
        //     }
        // }
        //if(winCounter == 0) break;
        this->active_window->CheckMinimized();
        
        this->renderer->MainRenderTick(this->active_window);
        return true;
    }
}