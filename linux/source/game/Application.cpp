#include "../../include/game/Application.h"
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
        this->renderSS = new RenderSubsystem();
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
                this->Update();
                lag -= MS_PER_UPDATE;
            }

            this->renderSS->MainTick();
        }
    }

    void Update()
    {

    }

    void Application::processKey()
    {

    }


}