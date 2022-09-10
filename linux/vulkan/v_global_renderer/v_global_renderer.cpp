#include "v_global_renderer.h"

namespace PL
{
    void v_global_renderer::createV_global_renderer()
    {
        if(v_global_renderer::global_renderer == nullptr)
            v_global_renderer::global_renderer = new v_global_renderer();
    }
    void v_global_renderer::pushModel(std::string srcFile, std::string shaderName)
    {
        if(v_global_renderer::global_renderer == nullptr)
            throw std::runtime_error("trying to access global renderer before creation");

        v_global_renderer::global_renderer->addModel(srcFile, shaderName);
    }
    void v_global_renderer::renderAll(uint32_t w, uint32_t h, uint32_t combufIndex)
    {
        if(v_global_renderer::global_renderer == nullptr)
            throw std::runtime_error("trying to access global renderer before creation");
        v_global_renderer::global_renderer->render(w, h, combufIndex);
    }
    void v_global_renderer::cleanUp()
    {
        if(v_global_renderer::global_renderer != nullptr)
            delete v_global_renderer::global_renderer;
    }

    v_global_renderer* v_global_renderer::global_renderer = nullptr;
    /**
     *  END OF STATIC PART
     */ 

    v_global_renderer::v_global_renderer() 
    {
        v_swapchain::createV_swapchain();
        this->initializeSynchronization();
    }
    v_global_renderer::~v_global_renderer()
    {
        
        v_swapchain::cleanUp();
    }
    void v_global_renderer::addModel(std::string srcFile, std::string shaderName)
    {

    }
    void v_global_renderer::render(uint32_t w, uint32_t h, uint32_t combufIndex)
    {
        VkDevice& device = v_device::LDevice(DEVICE_MAIN);

        // #0 Block rendering if prev frame not shown
        vkWaitForFences(device, 1, &inFlightFences[combufIndex], VK_TRUE, UINT64_MAX);
            
        // #1 Get image from swapchain
        uint32_t imageIndex;
        VkResult result = v_swapchain::acquireNextImage(&imageIndex, 
                    imageAvailableSemaphores[combufIndex]);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            //this->window->handleFramebufferResized();
            //this->recreateFrameOnResize();
            
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }
        

        
        VkResult result = v_swapchain::submitCommBuffers(this->commandBuffers, combufIndex);


    }

    void v_global_renderer::initializeSynchronization()
    {
        VkDevice& device = v_device::LDevice(DEVICE_MAIN);

        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        // NO PARAMS NEEDED FOR EMPTY SEMAPHORES      
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        // -> start as free(signaled) -> no waiting if no previous
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    void v_global_renderer::swapchainResized(uint32_t size)
    {

    }
    void v_global_renderer::allocateCommandBuffers(uint32_t size)
    {

    }
    void v_global_renderer::freeCommandBuffers()
    {
        
    }
/*
    std::map<std::string, v_pipeline*> pipe_factory;

        static v_pipeline* createPipeline(v_device* device, VkRenderPass pass, std::string shaderName)
        {
            if(v_pipeline::factory.find(shaderName) != v_pipeline::factory.end())
            {
                return v_pipeline::factory[shaderName];
            }

            v_pipeline::factory[shaderName] = new v_pipeline(device, pass, shaderName);
            return v_pipeline::factory[shaderName];
        }

        static void recordAllPipelines(
                v_swapchain* swap, uint32_t bufferIndex, uint32_t imageIndex)
        {
            for(const auto& [key, value] : v_pipeline::factory)
            {
                value->recordPipeline(swap, bufferIndex, imageIndex);
            }
        }
        static VkCommandBuffer* getAllBuffers(uint32_t bufferIndex)
        {
            std::vector<VkCommandBuffer> bufs;
            for(const auto& [key, value] : v_pipeline::factory)
            {
                std::cout << value->getCommandBuffer()[bufferIndex] << std::endl;
                bufs.push_back(value->getCommandBuffer()[bufferIndex]);
            }
            return bufs.data();
        }
        static uint32_t getAllBufferCount(uint32_t bufferIndex)
        {
            return static_cast<uint32_t>(v_pipeline::factory.size());
        }

        static void recreateAllCommandBuffers(uint32_t size)
        {
            for(const auto& [key, value] : v_pipeline::factory)
            {
                value->recreateCommandBuffers(size);
            }
        }
*/
}