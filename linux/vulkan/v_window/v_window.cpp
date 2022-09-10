#include "v_window.h"
#include "../../lib_include.h"


namespace PL
{
    std::map<std::string, v_window*> v_window::window_factory = 
                std::map<std::string, v_window*>();

    void v_window::mainLoopTick(std::string uid)
    {
        if(v_window::window_factory.find(uid) == v_window::window_factory.end())
        {
            throw std::runtime_error("Trying to access window before creation!");
        }
        return v_window::window_factory[uid]->mainLoopTick();
    }
    void v_window::createWindow(std::string uid, 
        std::string name, uint32_t width, uint32_t height)
    {
        if(v_window::window_factory.find(uid) == v_window::window_factory.end())
        {
            v_window::window_factory[uid] = new v_window(width, height, name);
        }
    }
    VkSurfaceKHR& v_window::getSurface(std::string uid)
    {
        if(v_window::window_factory.find(uid) == v_window::window_factory.end())
        {
            throw std::runtime_error("Trying to access window before creation!");
        }
        return v_window::window_factory[uid]->getSurface();
    }
    bool v_window::shouldClose(std::string uid)
    {
        if(v_window::window_factory.find(uid) == v_window::window_factory.end())
        {
            throw std::runtime_error("Trying to access window before creation!");
        }
        return v_window::window_factory[uid]->shouldClose();
    }
    VkExtent2D v_window::getPixelExtent(std::string uid)
    {
        if(v_window::window_factory.find(uid) == v_window::window_factory.end())
        {
            throw std::runtime_error("Trying to access window before creation!");
        }
        v_window* win = v_window::window_factory[uid];
        return win->getPixelExtent();
    }
    VkExtent2D v_window::getCurrentExtent(std::string uid)
    {
        if(v_window::window_factory.find(uid) == v_window::window_factory.end())
        {
            throw std::runtime_error("Trying to access window before creation!");
        }
        v_window* win = v_window::window_factory[uid];
        return win->getExtent();
    }
    void v_window::checkMinimized(std::string uid)
    {
        if(v_window::window_factory.find(uid) == v_window::window_factory.end())
        {
            throw std::runtime_error("Trying to access window before creation!");
        }
        v_window* win = v_window::window_factory[uid];
        win->checkMinimized();
    }

    void v_window::cleanUp()
    {
        for(const auto& [key, value] : v_window::window_factory)
        {
            delete value;
        }
    }
    /**
     *  END OF STATIC PART
     */

    v_window::v_window(uint32_t w, uint32_t h, std::string name)
    {
        //this->width = w;
        //this->height = h;
        this->extent = {w, h};
        this->win_name = name;
        this->initWindow();
        this->initializeSurface();
    }

    v_window::~v_window()
    {
        vkDestroySurfaceKHR(v_instance::getV_instance(), surface, nullptr);
        glfwDestroyWindow(this->window);
        glfwTerminate();

        Logger::GLOBAL_LOGGER << LOG_MSG_INFO << "Window [" << 
                    this->getName() << "] destroyed" << std::endl;
    }
    VkExtent2D v_window::getPixelExtent()
    {
        int width, height;
        glfwGetFramebufferSize(this->getGLFWWindow(), &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
        return actualExtent;
    }
    void v_window::checkMinimized()
    {
        VkExtent2D ext = this->getPixelExtent();
        while (ext.width == 0 || ext.height == 0) {
            Logger::GLOBAL_LOGGER << LOG_MSG_INFO << "Minimized!" << std::endl;
            ext = this->getPixelExtent();
            glfwWaitEvents();
        }
    }

    static void glfwErrorCallback(int code, const char* description) {
        Logger::GLOBAL_LOGGER << LOG_MSG_ERROR << "GLFW Error " << code << ": " << description << std::endl;
    }


    void v_window::setNewExtent(uint32_t w, uint32_t h)
    {
        this->extent = {w, h};
    }
    void v_window::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto win = reinterpret_cast<v_window*>(glfwGetWindowUserPointer(window));
        //win->signalFramebufferResized();
        win->setNewExtent(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    }

    void v_window::initWindow()
    {
        //glfwInit();
        glfwSetErrorCallback(glfwErrorCallback);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        //glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        

        this->window = glfwCreateWindow(this->extent.width, this->extent.height, 
                                this->win_name.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, v_window::framebufferResizeCallback);
        Logger::GLOBAL_LOGGER << LOG_MSG_INFO << "Window: " << 
                this->extent.width << "x" << this->extent.height 
                    << " created!" << std::endl;

    }

    void v_window::mainLoopTick()
    {
        glfwPollEvents();
    }

    bool v_window::shouldClose()
    {
        bool ret = glfwWindowShouldClose(this->window);
        return ret;
    }

    void v_window::initializeSurface()
    {
        VkResult res = glfwCreateWindowSurface(v_instance::getV_instance(), 
                    window, nullptr, &surface);

        if (res != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }
}