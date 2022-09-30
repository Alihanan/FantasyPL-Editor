#include "../../include/vulkan/vWindow.h"
#include "../../include/vulkan/vApplication.h"

namespace PL
{
    const std::string vWindow::_DEP_ID = IDependent::type(new vWindow());

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

        vApplication::GLOBAL_LOGGER << LOG_MSG_INFO << "Window [" << 
                    this->windowName << "] created" << std::endl;
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

    VkExtent2D vWindow::GetPixelExtent()
    {
        int width, height;
        glfwGetFramebufferSize(this->glfwWindow, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
        return actualExtent;
    }

    void vWindow::CallbackFramebufferResize(GLFWwindow* window, int width, int height)
    {
        auto win = reinterpret_cast<vWindow*>(glfwGetWindowUserPointer(window));
        win->SetWindowExtent({static_cast<uint32_t>(width), static_cast<uint32_t>(height)});
        std::cout << "resize: " << width << " " << height << std::endl;
    }

    void vWindow::CheckMinimized()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(this->glfwWindow, &width, &height);
        while (width == 0 || height == 0) {
            vApplication::GLOBAL_LOGGER << LOG_MSG_INFO << "Window minimized!" << std::endl;
            glfwGetFramebufferSize(this->glfwWindow, &width, &height);
            glfwWaitEvents();
        }
    }
}