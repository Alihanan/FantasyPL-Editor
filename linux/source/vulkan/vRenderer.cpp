#include "../../include/vulkan/vRenderer.h"

namespace PL
{
    const std::string vRenderer::_DEP_ID = IDependent::type(vRenderer());

    vRenderer::~vRenderer()
    {
        if(!this->isInitialized) return;   
        this->freeCommandBuffers();
        vkDestroyCommandPool(this->device->GetReadyDevice()->logicalDevice, 
                            commandPool, nullptr);
    }
    
    void vRenderer::Initialize()
    {
        this->createCommandPools();
    }

    void vRenderer::RenderModel(GameModel* model)
    {
        this->manager->RecordModelAndPipeline(model, this->commandBuffers[currentFrameIndex]);
    }

    void vRenderer::BeginFrame(vWindow* activeWindow)
    {
        // 1. Acquire + 2. Check resize
        bool result = this->swapchain->AcquireImage(&this->curr_imageIndex);
        if (!result) {
            BeginFrame(activeWindow);
            return;
        } 

        // 3. Record pipelines
        this->recreateCommandBuffers(swapchain->GetImageCount());
        this->BeginRecordCommandBuffer(this->curr_imageIndex);
    }
    void vRenderer::EndFrame(vWindow* activeWindow)
    {
        this->EndRecordCommandBuffer(this->curr_imageIndex);

        // 4. Submit + 5. Present
        bool ret = this->swapchain->SubmitCommandBuffers(&this->commandBuffers[currentFrameIndex], &this->curr_imageIndex);     
        currentFrameIndex = (currentFrameIndex + 1) % vSwapchain::MAX_FRAMES_IN_FLIGHT; 
    }


    void vRenderer::ResizeSwapchain()
    {
        
        this->swapchain->Recreate();
    }
    void vRenderer::CheckResizeSwapchain()
    {
        this->swapchain->CheckWindowResize();
    }


    void vRenderer::createCommandPools()
    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = this->device->GetReadyDevice()->graphicsFamily.value();

        if (vkCreateCommandPool(this->device->GetReadyDevice()->logicalDevice,
                &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }
    }
    void vRenderer::BeginRecordCommandBuffer(uint32_t imageIndex)
    {
        vkResetCommandBuffer(this->commandBuffers[currentFrameIndex],  0);

        VkCommandBuffer commandBuffer = this->commandBuffers[currentFrameIndex];

        // begin command buffer
        // > reusage of CB defined as parameters
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        // bind FB to the image
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapchain->GetRenderPass();
        renderPassInfo.framebuffer = swapchain->GetActiveFramebuffer(imageIndex);
        // define size
        // should be same as imageview
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapchain->GetExtent();

        // something like glClearColor
        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        //start render pass
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }
    void vRenderer::EndRecordCommandBuffer(uint32_t imageIndex)
    {
        VkCommandBuffer commandBuffer = this->commandBuffers[currentFrameIndex];
        // end our render pass call
        // sth like VAO
        vkCmdEndRenderPass(commandBuffer);

        // possible other render passes(post-processing)

        // end command buffer -> chain of commands
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
    void vRenderer::allocateCommandBuffers(uint32_t size)
    {
        commandBuffers.resize(size);

        // primary   -> submit, cannot be used from others
        // secondary -> can't submit, only use from others
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

        if (vkAllocateCommandBuffers(device->GetReadyDevice()->logicalDevice,
                &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }
    void vRenderer::recreateCommandBuffers(uint32_t size)
    {
        if(this->commandBuffers.size() == size) return;
        this->freeCommandBuffers();
        this->allocateCommandBuffers(size);
    }
    void vRenderer::freeCommandBuffers()
    {
        if(this->commandBuffers.size() == 0) return;

        vkFreeCommandBuffers(this->device->GetReadyDevice()->logicalDevice,
            this->commandPool, 
            static_cast<uint32_t>(this->commandBuffers.size()),
            this->commandBuffers.data()
            );

        commandBuffers.clear();
    }

    
}