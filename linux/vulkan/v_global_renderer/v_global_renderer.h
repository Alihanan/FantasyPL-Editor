#pragma once

#include "../va_uncopiable/va_uncopiable.h"
#include "../../lib_include.h"
//#include "../v_pipeline/v_pipeline.h"
#include "v_swapchain/v_swapchain.h"

#include <map>
#include <string>

namespace PL
{
    class v_global_renderer : va_uncopiable
    {
    public:
        
        static void createV_global_renderer();
        static void pushModel(std::string srcFile, std::string shaderName);
        static void renderAll(uint32_t w, uint32_t h, uint32_t combufIndex);
        static void cleanUp();

    protected:    
        static v_global_renderer* global_renderer;
        v_global_renderer();
        ~v_global_renderer();

        //std::map<std::string, v_pipeline*> pipe_factory;
        
        void addModel(std::string srcFile, std::string shaderName);
        void render(uint32_t w, uint32_t h, uint32_t combufIndex);
        void swapchainResized(uint32_t size);
        //v_swapchain* swapchain;
        //void initializeSwapchain();

        void allocateCommandBuffers(uint32_t size); 
        void freeCommandBuffers();

        std::vector<VkCommandBuffer> commandBuffers;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        void initializeSynchronization();
    };
}