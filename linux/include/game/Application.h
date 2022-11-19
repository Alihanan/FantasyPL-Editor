#pragma once

#include "GOTransform.h"
#include "../general/IUncopiable.h"
#include "../general/IDependent.h"
#include "../vulkan/RenderSubsystem.h"
#include "../io/InputSubsystem.h"
#include "../io/LoggerSubsystem.h"
#include "../io/GlobalSettingsSubsystem.h"
#include "SceneGraphSubsystem.h"

namespace PL
{
    class Application : public IUncopiable
    {
    public:
        Application() {}
        ~Application();

        void Initialize();
        void MainLoop();
        void Update();
        void processKey();

    protected:
        GlobalSettingsSubsystem* gsettingsSS;
        RenderSubsystem* renderSS;
        InputSubsystem* inputSS;
        LoggerSubsystem* loggerSS;
        SceneGraphSubsystem* sceneGraphSS;

        inline static const double MS_PER_UPDATE = 0.01;
    };
}