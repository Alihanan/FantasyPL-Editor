#include "../../include/vulkan/vApplication.h"

namespace PL
{
    Logger vApplication::GLOBAL_LOGGER = Logger(
        vApplication::LOGGER_TYPE, 
        vApplication::LOGGER_VERBOSITY);


    const std::string vApplication::_DEP_ID = IDependent::type(vApplication());    

    void vApplication::Initialize()
    {
        
    }
    vApplication::~vApplication()
    {

    }
    void vApplication::MainLoop()
    {
        while(true)
        {
            if(this->active_window->MainShouldClose()) break;
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
        }
    }
}