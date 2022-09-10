#include "v_pipeline.h"
#include "../../lib_include.h"
#include "../v_model/v_model.h"

namespace PL
{
    std::map<std::string, v_pipeline*> v_pipeline::factory =
                std::map<std::string, v_pipeline*>();

    v_pipeline::v_pipeline(v_device* device, VkRenderPass& renderPass, std::string shaderName)
    : shader_name(shaderName), renderPass(renderPass), device(device)
    {
        //this->swapchain = swapchain;
        this->initialize();
    }

    v_pipeline::~v_pipeline()
    {
        vkDestroyPipeline(this->device->getLogicalDevice(),
                            graphicsPipeline, nullptr);

        vkDestroyCommandPool(this->device->getLogicalDevice(), 
                            commandPool, nullptr);
        delete this->config;
        
        /*
        for(int i = 0; i < static_cast<int>(SHADER_TO_USE.size()); i++)
        {   
            delete &allShaders[i];
        }
        delete[] allShaders;*/
    }

    void v_pipeline::initialize()
    {
        //this->createShaders();
        this->createPipeline();
        //this->updateDynamicStates();

        this->createCommandPools();
        this->allocateCommandBuffers(MAX_FRAMES_IN_FLIGHT);
    }

    void v_pipeline::recordPipeline(v_swapchain* swapchain, uint32_t bufferIndex, uint32_t imageIndex)
    {
        vkResetCommandBuffer(this->getCommandBuffer()[bufferIndex],  0);
            
        this->beginRecordingCommandBuffer(
                swapchain, bufferIndex, imageIndex);

        for(v_model* model : this->models)
        {
            model->bind(this->getCommandBuffer()[bufferIndex]);
            model->draw(this->getCommandBuffer()[bufferIndex]);
        }

        this->endRecordingCommandBuffer(
                swapchain, bufferIndex, imageIndex);         
    }
    void v_pipeline::recreateCommandBuffers(uint32_t size)
    {        
        if(this->commandBuffers.size() == size) return;
        this->freeCommandBuffers();
        this->allocateCommandBuffers(size);
    }
    void v_pipeline::freeCommandBuffers()
    {
        vkFreeCommandBuffers(this->device->getLogicalDevice(),
        this->commandPool, 
        static_cast<uint32_t>(this->commandBuffers.size()),
        this->commandBuffers.data());

        commandBuffers.clear();
    }

    void v_pipeline::allocateCommandBuffers(uint32_t size)
    {
        commandBuffers.resize(size);


        // primary   -> submit, cannot be used from others
        // secondary -> can't submit, only use from others
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

        if (vkAllocateCommandBuffers(device->getLogicalDevice(),
                &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }
    
    void v_pipeline::beginRecordingCommandBuffer(v_swapchain* swapchain, uint32_t bufferIndex, uint32_t imageIndex)
    {
        VkCommandBuffer commandBuffer = this->commandBuffers[bufferIndex];
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
        renderPassInfo.renderPass = swapchain->getRenderPass();
        renderPassInfo.framebuffer = swapchain->getFramebuffers()[imageIndex];
        // define size
        // should be same as imageview
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapchain->getSwapchainExtent();

        // something like glClearColor
        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        //start render pass
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // fixed state pipeline
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
        // dynamic state pipeline
        //this->updateDynamicStates(
        //        swapchain->getSwapchainExtent().width,
        //        swapchain->getSwapchainExtent().height); // fetch new values from swapchain
        vkCmdSetViewport(commandBuffer, 0, 1, &this->viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &this->scissor);

    }
    void v_pipeline::endRecordingCommandBuffer(v_swapchain* swapchain, uint32_t bufferIndex, uint32_t imageIndex)
    {
        VkCommandBuffer commandBuffer = this->commandBuffers[bufferIndex];
        // end our render pass call
        // sth like VAO
        vkCmdEndRenderPass(commandBuffer);

        // possible other render passes(post-processing)

        // end command buffer -> chain of commands
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }


    void v_pipeline::createCommandPools()
    {
        QueueFamilyIndices queueFamilyIndices = 
            this->device->getAvailableQueueFamily(this->device->getPhysicalDevice());

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(this->device->getLogicalDevice(),
                &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }

    }
    void v_pipeline::setViewport(float x, float y, float w, float h)
    {
        viewport.x = x;
        viewport.y = y;
        viewport.width = w;
        viewport.height = h;     
    }

    void v_pipeline::setScissor(int32_t x_offset, int32_t y_offset, uint32_t w, uint32_t h)
    {       
        scissor.offset = {x_offset, y_offset};
        scissor.extent = {w, h};      
    }
    void v_pipeline::setDepthScissor(float minD, float maxD)
    {
        viewport.minDepth = minD;
        viewport.maxDepth = maxD;
    }
    void v_pipeline::createPipeline()
    {
        this->config = new v_pipeline_config(this->renderPass, this->device, this->shader_name);
        this->config->createPipeline(&this->graphicsPipeline);
        //pipelineInfo.pStages = allShaders[1]->infos;
    }    

    void v_pipeline::updateDynamicStates(uint32_t w, uint32_t h)
    {      
        this->setViewport(this->viewport.x, this->viewport.y, w, h);
        this->setScissor(this->scissor.offset.x, this->scissor.offset.y, w, h);
        this->setDepthScissor(this->viewport.minDepth, this->viewport.maxDepth);
    }

    
    /*
    void v_pipeline::createShaders()
    {
        
        allShaders = new ShaderObj*[SHADER_TO_USE.size()];

        int i = 0;
        for(const char* name : SHADER_TO_USE)
        {
            std::string s_name(name);
            allShaders[i] = this->createShaderModule(s_name);    
            i++;
        }
    }
    */
    /*
    ShaderObj* v_pipeline::createShaderModule(std::string name)
    {
        ShaderData* shaders = this->readShaders(name);
        ShaderObj* ret = new ShaderObj;

        for(uint32_t i = 0; i < SHADER_TYPE_SIZE; i++)
        {
            ShaderData sd = shaders[i];

            if(sd.data->size() == 0)
            {
                ret->isIncluded[i] = false;
                continue;
            }
                


            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = sd.data->size();
            createInfo.pCode = reinterpret_cast<const uint32_t*>(sd.data->data());

            VkShaderModule shaderModule;
            if (vkCreateShaderModule(this->device->getLogicalDevice(),
                &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {

                throw std::runtime_error("failed to create shader module!");
            }

            VkPipelineShaderStageCreateInfo shaderStageInfo{};
            shaderStageInfo.stage = sd.type.bit;
            shaderStageInfo.pName = "main";
            shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageInfo.module = shaderModule;

            ret->infos[i] = shaderStageInfo;
            ret->isIncluded[i] = true;
        }

        for(uint32_t i = 0; i < SHADER_TYPE_SIZE; i++){
            delete shaders[i].data;
        }
        delete[] shaders;

        return ret;
    }

    ShaderData* v_pipeline::readShaders(std::string name)
    {
        bool isComplete = true;
        ShaderData* data = new ShaderData[SHADER_TYPE_SIZE];

        int i = 0;
        for(ShaderType type : SHADER_TYPES)
        {
            std::string fullpath = 
                SHADER_DEFAULT_PATH + name + type.format + SHADER_SPIRV_FORMAT;
            data[i].type = type;

            std::vector<char>* ret = readFile(fullpath);
            bool exists = (ret->size() != 0);            

            bool isNeeded = !(type.isNeeded && !exists);
            isComplete = isComplete & isNeeded;
            data[i].data = ret;

            i++;
        }

        if(!isComplete)
        {
            throw std::runtime_error("failed to open vert + frag of [" + name + "]!");
        }
        else{
            Logger::GLOBAL_LOGGER << LOG_MSG_INFO << "[VERTEX LOADING] ";
            Logger::GLOBAL_LOGGER << "[" << name + ".*" + SHADER_SPIRV_FORMAT 
                    << "] successfully read" << std::endl;
        }
        
        return data;
    }

    std::vector<char>* v_pipeline::readFile(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        std::vector<char>* buffer = new std::vector<char>();

        if (!file.is_open()) {
            return buffer;
        }
        size_t fileSize = (size_t) file.tellg();
        buffer->resize(fileSize);

        file.seekg(0);
        file.read(buffer->data(), fileSize);
        file.close();

        return buffer;
    }*/
}