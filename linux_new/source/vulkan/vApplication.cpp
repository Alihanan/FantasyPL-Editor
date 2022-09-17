#include "../../include/vulkan/vApplication.h"

namespace PL
{
    Logger vApplication::GLOBAL_LOGGER = Logger(
        vApplication::LOGGER_TYPE, 
        vApplication::LOGGER_VERBOSITY);


    

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
            vWindow::MainAllWindowGLFWTick();

            uint32_t winCounter = 0;

            for(vWindow* win : this->windows)
            {
                winCounter += (!win->MainShouldClose());
                if(win->MainShouldClose())
                {
                    continue;
                }
            }

            if(winCounter == 0) break;
        }
    }
}