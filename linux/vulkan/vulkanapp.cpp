#include "vulkanapp.h"

namespace PL
{
    Logger Logger::GLOBAL_LOGGER = Logger(
        LOGGER_TYPE_STDOUT | LOGGER_TYPE_FILE, 
        LOGGER_VERBOSITY);

    VulkanApp::VulkanApp()
    {

    }

    VulkanApp::~VulkanApp()
    {
        //delete this->window;
        
        //VkDevice& device = v_device::LDevice(DEVICE_MAIN);
        /*
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, inFlightFences[i], nullptr);
        }*/
        /*
        for(auto model : this->models)
        {
            delete model;
        }*/

        //delete pipeline;
        //delete swapchain;
        //delete this->device;
        //delete window;
        
        v_global_renderer::cleanUp();
        v_device::cleanUp();
        v_window::cleanUp();  
        v_instance::cleanUp(); // after -> 190,988 bytes

        glfwTerminate(); // Ubuntu + notebook -> not releasing mem
    }

    void VulkanApp::initializeVulkan(std::string name)
    {
        char var_env[42] = "DISABLE_VK_LAYER_VALVE_steam_overlay_1=1";
        putenv(var_env);

        glfwInit(); // + 28543 bytes allocated

        v_instance::createV_instance(name);  
    }

    void VulkanApp::createWindow(uint32_t width, uint32_t height)
    {       
        v_window::createWindow(WINDOW_MAIN, v_instance::getAppName(), width, height);
        v_device::createDevice(DEVICE_MAIN);
        v_global_renderer::createV_global_renderer();
        //this->initializeWindow(this->instance->getAppName(), width, height);
        //this->initializeDevice();
        //this->initializeSwapchain();
        //this->initializePipeline();
        //this->initializeSynchronization();
    }
    void VulkanApp::createModels(std::vector<const char*> files)
    {
        //models.push_back(new v_model(this->device));

        //models.push_back(new v_model(this->device));
        this->loadModel(std::string(""));

        // TODO implement loading
        for(const char* name : files)
        {
            this->loadModel(std::string(name));
        }
    }
    
    void VulkanApp::loadModel(std::string filename)
    {
        // TODO
        //v_pipeline::createPipeline(this->device, this->swapchain->getRenderPass(), "simple");
        //new v_model(this->device, this->swapchain->getRenderPass(), "", "simple");
        //this->pipeline = new v_pipeline(this->device, this->swapchain->getRenderPass(),
        //                            "simple");
        v_global_renderer::pushModel("", "");
    }
    
    /*
    void VulkanApp::initializeInstance(std::string name)
    {
        this->instance = new v_instance(name);
    }
    void VulkanApp::initializeWindow(std::string name, uint32_t W, uint32_t H)
    {
        this->window = new v_window(W, H, name, this->instance);
    }
    void VulkanApp::initializeDevice()
    {
        this->device = new v_device(this->instance, this->window);
    }
    void VulkanApp::initializeSwapchain()
    {
        this->swapchain = new v_swapchain(this->device, this->window);
    }*/
    /*
    void VulkanApp::initializePipeline()
    {
        this->pipeline = new v_pipeline(this->swapchain, this->device);
    }*/

    /* 
    void VulkanApp::initializeSynchronization()
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
    }*/

    void VulkanApp::mainLoop()
    {
        
        while (!v_window::shouldClose(WINDOW_MAIN)) {
            v_window::mainLoopTick(WINDOW_MAIN);
            
            // Render loop
            VkDevice& device = v_device::LDevice(DEVICE_MAIN);  

            // check if window is active
            v_window::checkMinimized(WINDOW_MAIN);
            // render frame
            VkExtent2D extent = v_window::getCurrentExtent(WINDOW_MAIN);
            v_global_renderer::renderAll(extent.width, extent.height, currentFrame);
            /* 
            + global fence(CPU side) to block this main loop


            0) wait for the end of previous frame
            (semaphore on GPU side -> wait until previous frame shown)
            1) get image from swapchain
            2) record commandbuffer(FB of fetched image)
            3) submit command buffer
            (semaphor on GPU side -> wait until commands finished)
            4) show image(imageview) on screen
            */
        /*
            // #0 Block rendering if prev frame not shown
            vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
            

            // #1 Get image from swapchain
            uint32_t imageIndex;
            VkResult result = vkAcquireNextImageKHR(device, 
                this->swapchain->getSwapchain(), UINT64_MAX, 
                imageAvailableSemaphores[currentFrame], 
                VK_NULL_HANDLE, &imageIndex);

            if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                this->window->handleFramebufferResized();
                this->recreateFrameOnResize();
                continue;
            } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
                throw std::runtime_error("failed to acquire swap chain image!");
            }
            
            vkResetFences(device, 1, &inFlightFences[currentFrame]);

            // #2 Record command buffer
            // sth like glBind(vao=0)
            v_pipeline::recordAllPipelines(this->swapchain, currentFrame, imageIndex);          
            
            // #3 Submit command buffer
            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            // bind waiting semaphores
            VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};           
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            // bind finish semaphores
            VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;
            // only writing to image is blocking
            VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
            submitInfo.pWaitDstStageMask = waitStages;
            // set command buffer
            submitInfo.commandBufferCount = v_pipeline::getAllBufferCount(currentFrame);
            submitInfo.pCommandBuffers = v_pipeline::getAllBuffers(currentFrame);
                    //&(this->pipeline->getCommandBuffer()[currentFrame]);
            std::cout << submitInfo.pCommandBuffers[0] << std::endl;
            // submit            
            if (vkQueueSubmit(this->device->getGraphicsQueue(), 
                    1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
                throw std::runtime_error("failed to submit draw command buffer!");
            }
            
            // #4 show(present) image on screen
            VkPresentInfoKHR presentInfo{};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = signalSemaphores;// wait for CB to finish

            // send all swapchains and images per swapchain
            VkSwapchainKHR swapChains[] = {this->swapchain->getSwapchain()};
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapChains;
            presentInfo.pImageIndices = &imageIndex;
            
            // if more swapchains -> array of results can be used
            // but if only one used -> return value already is a result
            presentInfo.pResults = nullptr; // Optional

            // present(show) image
            result = vkQueuePresentKHR(this->device->getPresentQueue(), &presentInfo);
            if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR
                || this->window->isFramebufferResized()) {
                this->window->handleFramebufferResized();
                this->recreateFrameOnResize();
            } else if (result != VK_SUCCESS) {
                throw std::runtime_error("failed to present swap chain image!");
            }
            */
            currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;            
        }
        vkDeviceWaitIdle(v_device::LDevice(DEVICE_MAIN));
    }
    /*
    void VulkanApp::recreateFrameOnResize()
    {
        // if minimized, just pause
        int width = 0, height = 0;
        glfwGetFramebufferSize(this->window->getGLFWWindow(), &width, &height);
        while (width == 0 || height == 0) {
            std::cout << "Minimized!" << std::endl;
            glfwGetFramebufferSize(this->window->getGLFWWindow(), &width, &height);
            glfwWaitEvents();
        }

        // otherwise recreate swapchain      
        vkDeviceWaitIdle(this->device->getLogicalDevice()); 

        this->swapchain->recreateSwapchain();

        v_pipeline::recreateAllCommandBuffers(this->swapchain->getImageCount());

        
    }*/
        /*
        if(this->swapchain->getImageCount() != this->pipeline->getCommandBuffer().size())
        {
            this->pipeline->recreateCommandBuffers();
        }*/
}