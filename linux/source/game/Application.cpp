#include "../../include/game/Application.h"
#include "../../include/general/DependFactory.h"

#include <ctime>

namespace PL
{
    Application::~Application()
    {

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
        time_t previous = time(nullptr);
        double lag = 0.0;
        while(true)
        {
            time_t current = time(nullptr);
            double elapsed = difftime(current, previous);
            previous = current;
            lag += elapsed;

            this->inputSS->ProcessInput();

            while (lag >= MS_PER_UPDATE)
            {
                this->sceneGraphSS->Update();
                lag -= MS_PER_UPDATE;
            }

            auto* allModels = this->sceneGraphSS->GetAllRenderObjects();
            this->renderSS->registerRenderModels(allModels);
            delete allModels;
            bool isClosed = this->renderSS->MainTick();
            if(!isClosed)
            {
                break;
            }
        }
    }

    void Application::processKey()
    {

    }


}