#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"
#include "vInstance.h"

// 3D graphics
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h> 


namespace PL
{
    class vWindow : public IUncopiable, public IDependent
    {
    public:
        // IDependent
        const static std::string _DEP_ID;
        inline const static std::vector<std::string> _DEP_NEEDED_DEPS = {
            vInstance::_DEP_ID
        };

        vWindow()
        : windowExtent({INITIAL_WIDTH, INITIAL_HEIGHT})
        {}
        
        std::vector<std::string> GetNeededDependencies()
        {
            return this->_DEP_NEEDED_DEPS;
        }

        void ReceiveContext(std::vector<std::vector<IDependent*>> context)
        {
            this->currentInstance = dynamic_cast<vInstance*>(context[0][0]); 
            this->windowName = this->currentInstance->GetName();
            this->Initialize();
        }

        void UpdateContext(std::vector<std::vector<IDependent*>> context)
        {

        }

        std::string GetDependencyID()
        {
            return this->_DEP_ID;
        }

        // Other members
        ~vWindow();
        static void MainAllWindowGLFWTick();

        bool MainShouldClose();
        
        VkExtent2D GetPixelExtent();

        VkSurfaceKHR& GetKHRSurface() { return this->khrSurface; }

    protected:
        vInstance* currentInstance;
        void Initialize();
        void SetWindowExtent(VkExtent2D extent) { this->windowExtent = extent; }
        static void CallbackFramebufferResize(GLFWwindow* window, int width, int height);

        std::string windowName;
        GLFWwindow* glfwWindow;
        VkExtent2D windowExtent;
        VkExtent2D pixelExtent;
        VkSurfaceKHR khrSurface;
        
        inline static const uint32_t INITIAL_WIDTH = 640;
        inline static const uint32_t INITIAL_HEIGHT = 320;

    private:
    };
}