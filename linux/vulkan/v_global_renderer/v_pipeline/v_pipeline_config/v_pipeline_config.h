#pragma once
#include "../../../va_uncopiable/va_uncopiable.h"
#include "../../v_swapchain/v_swapchain.h"
#include "../v_shader/va_shader.h"

namespace PL
{
    class v_pipeline_config : va_uncopiable
    {
    public:
        v_pipeline_config(VkRenderPass& renderPass, v_device* device, std::string name);
        ~v_pipeline_config();
        void createPipeline(VkPipeline* graphicsPipe);

        void setInputAssembly(VkPrimitiveTopology topology, bool isEBO);
        void setDepthClamp(bool isDepthClamp);
        void setPolygonFillMode(VkPolygonMode mode, float linewidth = 1.0f);
        void setCullMode(VkCullModeFlags cullMode, VkFrontFace face);
        void setDepthBias()
        {
            rasterizer.depthBiasEnable = VK_FALSE;
            rasterizer.depthBiasConstantFactor = 0.0f; // Optional
            rasterizer.depthBiasClamp = 0.0f; // Optional
            rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
        }
        void setMultisampling();
        void setColorBlend();
        void setUniformLayout();


        VkPipelineViewportStateCreateInfo& getViewportState() 
        { return this->viewportState;}
        VkPipelineInputAssemblyStateCreateInfo& getInputAssembly()
        {return this->inputAssembly;}
        VkPipelineRasterizationStateCreateInfo& getRasterizer()
        {return this->rasterizer;}
        VkPipelineMultisampleStateCreateInfo& getMultisampling()
        {return this->multisampling;}
        VkPipelineColorBlendStateCreateInfo& getColorBlending()
        {return this->colorBlending;}
        VkPipelineLayout& getPipelineLayout() 
        { return this->pipelineLayout;}
        VkPipelineVertexInputStateCreateInfo& getVertexInputInfo()
        { return this->vertexInputInfo;}
        VkPipelineDynamicStateCreateInfo& getDynamicState()
        { return this->dynamicState;}
        const VkPipelineShaderStageCreateInfo* getShaderStages()
        { return (this->shader->getShaderStages().data()); }
        uint32_t getShaderStageCount()
        { return static_cast<uint32_t>(this->shader->getShaderStages().size());}

    protected:
        //v_swapchain* swapchain;
        VkRenderPass& renderPass;
        v_device* device;       

        VkPipelineDynamicStateCreateInfo dynamicState{};
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        VkPipelineViewportStateCreateInfo viewportState{};
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        VkPipelineMultisampleStateCreateInfo multisampling{};
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        VkPipelineLayout pipelineLayout{};
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

        void initialize();
        void initializeShader();
        void initializeDynamicStates();
        void initializeVertexInput();

        va_shader* shader;
        std::string shader_name;


        const std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

    };




}