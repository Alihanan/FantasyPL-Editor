#include "../../include/vulkan/vWindow.h"
#include "vApplication.h"

namespace PL
{
    vWindow::~vWindow()
    {
        vkDestroySurfaceKHR(this->currentInstance->GetInstance(), this->khrSurface, nullptr);
        glfwDestroyWindow(this->glfwWindow);
        vApplication::GLOBAL_LOGGER << LOG_MSG_INFO << "Window [" << 
                    this->windowName << "] destroyed" << std::endl;
    }

    void vWindow::Initialize()
    {
        // GLFW window create
        this->glfwWindow = glfwCreateWindow(this->windowExtent.width, this->windowExtent.height, 
                                this->windowName.c_str(), nullptr, nullptr);
        // Resize callback
        glfwSetWindowUserPointer(this->glfwWindow, this);
        glfwSetFramebufferSizeCallback(this->glfwWindow, vWindow::CallbackFramebufferResize);

        // KHR Surface
        VkResult res = glfwCreateWindowSurface(this->currentInstance->GetInstance(), 
                    this->glfwWindow, nullptr, &this->khrSurface);
        if (res != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }

        Logger::GLOBAL_LOGGER << LOG_MSG_INFO << "Window [" << 
                    this->windowName << "] destroyed" << std::endl;
    }

    void vWindow::MainAllWindowGLFWTick()
    {
        glfwPollEvents();
    }
    bool vWindow::MainShouldClose()
    {
        bool ret = glfwWindowShouldClose(this->glfwWindow);
        return ret;
    }

    void vWindow::CallbackFramebufferResize(GLFWwindow* window, int width, int height)
    {
        auto win = reinterpret_cast<vWindow*>(glfwGetWindowUserPointer(window));
        win->SetWindowExtent({static_cast<uint32_t>(width), static_cast<uint32_t>(height)});
    }
}