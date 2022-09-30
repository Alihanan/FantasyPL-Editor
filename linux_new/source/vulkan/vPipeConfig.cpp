#include "../../include/vulkan/vPipeConfig.h"
#include "../../include/vulkan/vPipeline.h"

namespace PL
{
    const std::string vPipeConfig::_DEP_ID = IDependent::type(vPipeConfig());

    vPipeConfig::~vPipeConfig()
    {
        
    }
    
    void vPipeConfig::Initialize()
    {
        // TODO
        data.pipelineInfo.pDepthStencilState = nullptr; // Optional

        this->SetRenderPass();
        this->SetDynamicStates();
        this->SetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);
        this->SetDepthClamp(false);
        this->SetPolygonFillMode(VK_POLYGON_MODE_FILL, 1.0f);
        this->SetCullMode(VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE);
        this->SetDepthBias();
        this->SetMultisampling(); // turned off
        this->SetColorBlend();
    }
    void vPipeConfig::SetRenderPass()
    {
        data.pipelineInfo.renderPass = this->swapchain->GetRenderPass();
        data.pipelineInfo.subpass = 0;
        data.pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        data.pipelineInfo.basePipelineIndex = -1; // Optional
    }

    void vPipeConfig::SetDynamicStates()
    {
        data.dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        data.dynamicState.dynamicStateCount = static_cast<uint32_t>(this->dynamicStates.size());
        data.dynamicState.pDynamicStates = this->dynamicStates.data();
        data.dynamicState.flags = 0;

        data.viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        data.viewportState.viewportCount = 1;
        data.viewportState.scissorCount = 1;

        data.viewportState.pViewports = nullptr;
        data.viewportState.pScissors = nullptr;
        data.viewportState.flags = 0;

        data.pipelineInfo.pDynamicState = &(this->data.dynamicState);
        data.pipelineInfo.pViewportState = &(this->data.viewportState);
    }
    void vPipeConfig::SetInputAssembly(VkPrimitiveTopology topology, bool isEBO)
    {
        data.inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        data.inputAssembly.topology = topology;
        data.inputAssembly.primitiveRestartEnable = isEBO ? VK_TRUE : VK_FALSE;

        data.pipelineInfo.pInputAssemblyState = &(this->data.inputAssembly);
    }
    void vPipeConfig::SetDepthClamp(bool isDepthClamp)
    {
        data.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        data.rasterizer.depthClampEnable = isDepthClamp ? VK_TRUE : VK_FALSE;
        data.rasterizer.rasterizerDiscardEnable = VK_FALSE;     

        data.pipelineInfo.pRasterizationState = &(this->data.rasterizer);
    }
    void vPipeConfig::SetPolygonFillMode(VkPolygonMode mode, float linewidth)
    {
        data.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        data.rasterizer.polygonMode = mode;
        data.rasterizer.lineWidth = 1.0f;

        data.pipelineInfo.pRasterizationState = &(this->data.rasterizer);
    }
    void vPipeConfig::SetCullMode(VkCullModeFlags cullMode, VkFrontFace face)
    {
        data.rasterizer.cullMode = cullMode;
        data.rasterizer.frontFace = face;

        data.pipelineInfo.pRasterizationState = &(this->data.rasterizer);
    }
    void vPipeConfig::SetMultisampling()
    {
        data.multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        data.multisampling.sampleShadingEnable = VK_FALSE;
        data.multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        data.multisampling.minSampleShading = 1.0f; // Optional
        data.multisampling.pSampleMask = nullptr; // Optional
        data.multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        data.multisampling.alphaToOneEnable = VK_FALSE; // Optional

        data.pipelineInfo.pMultisampleState = &(this->data.multisampling);
    }
    void vPipeConfig::SetColorBlend()
    {
        data.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        data.colorBlendAttachment.blendEnable = VK_TRUE;
        data.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        data.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        data.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        data.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        data.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        data.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        /*
        if (blendEnable) {
            finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
            finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
        } else {
            finalColor = newColor;
        }

        finalColor = finalColor & colorWriteMask;

        
        finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor;
        finalColor.a = newAlpha.a;
        */

        data.colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        data.colorBlending.logicOpEnable = VK_FALSE;
        data.colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        data.colorBlending.attachmentCount = 1;
        data.colorBlending.pAttachments = &data.colorBlendAttachment;
        data.colorBlending.blendConstants[0] = 0.0f; // Optional
        data.colorBlending.blendConstants[1] = 0.0f; // Optional
        data.colorBlending.blendConstants[2] = 0.0f; // Optional
        data.colorBlending.blendConstants[3] = 0.0f; // Optional

        data.pipelineInfo.pColorBlendState = &(data.colorBlending);
    }
    
    void vPipeConfig::SetShaderName(std::string name)
    {
        data.shader = this->shaderManager->createShader(name);
        this->SetShaderConfig();
    }

    vPipeline* vPipeConfig::GeneratePipeline()
    {
        if (data.shader == nullptr)
            throw std::runtime_error("Trying to create pipeline without shader!");
        
        
        VkPipeline* graphicsPipe = new VkPipeline;
        if (vkCreateGraphicsPipelines(this->device->GetReadyDevice()->logicalDevice, 
        VK_NULL_HANDLE, 1, &(data.pipelineInfo), nullptr, graphicsPipe) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        /*
        auto& stages = data.shader->getShaderStages();
        for(size_t i = 0; i < stages.size(); i++)
        {
            vkDestroyShaderModule(this->device->GetReadyDevice()->logicalDevice, 
                                stages[i].module, nullptr);
        }
        */

        vPipeline* ret = new vPipeline(graphicsPipe, this->data.shader, device, this->swapchain);
        return ret;
    }

    void vPipeConfig::SetShaderConfig()
    {      
        // Vertex Input
        data.vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        auto& bindingDescriptions = data.shader->getBindingDescriptions();
        auto& attributeDescriptions = data.shader->getAttributeDescriptions();

        data.vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
        data.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        data.vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
        data.vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        data.pipelineInfo.pVertexInputState = &(this->data.vertexInputInfo);

        // Uniform layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
        if (vkCreatePipelineLayout(this->device->GetReadyDevice()->logicalDevice,
                &pipelineLayoutInfo, nullptr, &data.pipelineLayout) 
                != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
        data.pipelineInfo.layout = data.pipelineLayout;

        // Stages
        auto& stages = data.shader->getShaderStages();

        data.pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        data.pipelineInfo.stageCount = static_cast<uint32_t>(stages.size());
        data.pipelineInfo.pStages = stages.data();  

        // Tesellation
        bool isTess = false;
        data.tesselation.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        for(auto stg : stages) // if any tesellation state found
        {
            if(stg.stage == VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT || 
                stg.stage == VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT)
            {
                data.tesselation.patchControlPoints = 4; // set quad for now
                isTess = true;
            }
        }
        if(!isTess)
            data.tesselation.patchControlPoints = 1; // if none, turn it off  
        data.pipelineInfo.pTessellationState = &data.tesselation;
    }

}