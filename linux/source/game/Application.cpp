#include "../../include/game/Application.h"
#include "../../include/general/DependFactory.h"

#include <ctime>

#include <iostream>

namespace PL
{
    Application::~Application()
    {
        delete this->sceneGraphSS;
        delete this->renderSS;
        delete this->loggerSS;
        delete this->inputSS;
        delete this->gsettingsSS;
    }

    void Application::Initialize()
    {
        this->gsettingsSS = new GlobalSettingsSubsystem();
        this->inputSS = new InputSubsystem();

        LoggerType type = std::any_cast<LoggerType>(this->gsettingsSS->GET(SETG_LOGGER_TYPE));
        LoggerVerbosity verbos = std::any_cast<LoggerVerbosity>(this->gsettingsSS->GET(SETG_LOGGER_VERBOSITY));
        
        this->loggerSS = new LoggerSubsystem(type, verbos);
        //this->renderSS = new RenderSubsystem(); // Warning, should be 
        this->renderSS = static_cast<PL::RenderSubsystem*>(
            PL::DependFactory::I()->createOrGetInstance(
                PL::RenderSubsystem::_DEP_ID
            )[0]
        );


        this->sceneGraphSS = new SceneGraphSubsystem();
    }
    void Application::MainLoop()
    {
        return;
        time_t previous = time(nullptr);
        double lag = 0.0;
        static uint32_t i = 0;
        while(true)
        {
            time_t current = time(nullptr);
            double elapsed = difftime(current, previous);
            previous = current;
            lag += elapsed;
            
            //this->inputSS->ProcessInput();
            this->sceneGraphSS->Update();
            /*
            while (lag >= MS_PER_UPDATE)
            {
                this->inputSS->ProcessInput();
                this->sceneGraphSS->Update();
                lag -= MS_PER_UPDATE;
            }*/

            bool isClosed = this->renderSS->MainTick(this->sceneGraphSS);
            if(!isClosed)
            {
                break;
            }
            i++;
            if(i == 1000) break;
        }
    }

}