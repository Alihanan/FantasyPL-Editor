#pragma once
#include "api_vulkanapp.h"
#include "v_instance/v_instance.h"
#include "v_window/v_window.h"
#include "v_device/v_device.h"
//#include "v_swapchain/v_swapchain.h"
//#include "v_pipeline/v_pipeline.h"
//#include "v_renderpass/v_renderpass.h"
//#include "v_model/v_model.h"

namespace PL
{
    class VulkanApp : API_vulkanapp
    {
    public:
        VulkanApp();
        ~VulkanApp();

        void initializeVulkan(std::string name);
        void createWindow(uint32_t width, uint32_t height);     
        void createModels(std::vector<const char*> files);  
        void mainLoop();


    protected:
        v_instance* instance;
        
        //v_window* window;
        void initializeWindow(std::string name, uint32_t W, uint32_t H);

        //v_device* device;
        //void initializeDevice();

        //v_swapchain* swapchain;
        //void initializeSwapchain();

        //v_pipeline* pipeline;
        //void initializePipeline();

        //std::vector<VkSemaphore> imageAvailableSemaphores;
        //std::vector<VkSemaphore> renderFinishedSemaphores;
        //std::vector<VkFence> inFlightFences;
        //void initializeSynchronization();

        uint32_t currentFrame = 0;
        //void recreateFrameOnResize();


        //std::vector<v_model*> models;
        void loadModel(std::string filename);
    };



}




