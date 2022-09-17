#pragma once
#include "../va_uncopiable/va_uncopiable.h"
#include "../v_instance/v_instance.h"
#include "../../lib_include.h"

// 3D graphics
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h> 

#include <map>

namespace PL
{
    class v_window : va_uncopiable
    {
    public:
        static void mainLoopTick(std::string uid);
        static void createWindow(std::string uid, 
            std::string name, uint32_t width, uint32_t height);
        static VkSurfaceKHR& getSurface(std::string uid);
        static bool shouldClose(std::string uid);
        static VkExtent2D getPixelExtent(std::string uid);
        static VkExtent2D getCurrentExtent(std::string uid);
        static void checkMinimized(std::string uid);
        static void cleanUp();


    protected:

        void setNewExtent(uint32_t w, uint32_t h);
        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
        VkExtent2D getExtent() { return this->extent; }
        VkExtent2D getPixelExtent();
        //bool isFramebufferResized() { return this->framebufferResized;}
        //void handleFramebufferResized() { this->framebufferResized = false; }
        //void signalFramebufferResized() { this->framebufferResized = true; }

        void checkMinimized();

        static std::map<std::string, v_window*> window_factory;
    protected:
        v_window(uint32_t w, uint32_t h, std::string name);
        ~v_window();

        bool shouldClose();
        void mainLoopTick();

        uint32_t getWidth() {return this->extent.width;}
        uint32_t getHeight() {return this->extent.height;}
        std::string getName() {return this->win_name;}

        VkSurfaceKHR& getSurface() { return this->surface;}
        GLFWwindow* getGLFWWindow() { return this->window; }
        


        GLFWwindow* window;
        //uint32_t width {640};
        //uint32_t height {320};
        VkExtent2D extent {640, 320};
        std::string win_name = "";

        // surface
        VkSurfaceKHR surface;

        void initWindow();
        void initializeSurface();

        //bool framebufferResized = false;
    };


}

