#include "v_pipeline_config.h"

namespace PL
{
    v_pipeline_config::v_pipeline_config(
        VkRenderPass& renderPass, 
        v_device* device, 
        std::string name) 
        : renderPass(renderPass), device(device), shader_name(name)
    {
        //this->swapchain = swapchain;
        //this->device = device;
        this->initializeShader();
        if(this->shader == nullptr){
            throw std::runtime_error("Non-loaded shader required! (" + 
                shader_name + ")");
        } 
        this->initialize();
    }

    v_pipeline_config::~v_pipeline_config()
    {
        vkDestroyPipelineLayout(this->device->getLogicalDevice(),
                                pipelineLayout, nullptr);
        delete this->shader;
    }

    void v_pipeline_config::initialize()
    {
        this->initializeDynamicStates();
        
        this->setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);
        this->setDepthClamp(false);
        this->setPolygonFillMode(VK_POLYGON_MODE_FILL, 1.0f);
        this->setCullMode(VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE);
        this->setDepthBias();
        this->setMultisampling(); // turned off
        this->setColorBlend();
        
    }

    void v_pipeline_config::initializeShader()
    {
        this->shader = va_shader::createShader(
            this->shader_name, 
            this->device->getLogicalDevice());
        if(this->shader == nullptr) return;
        this->initializeVertexInput(); 
        this->setUniformLayout();
    }

    void v_pipeline_config::createPipeline(
            VkPipeline* graphicsPipe)
    {
        if(this->renderPass == VK_NULL_HANDLE) throw std::runtime_error("RENDER PASS ERROR");
        

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        auto& stages = this->shader->getShaderStages();
        pipelineInfo.stageCount = 
            stages.size();
        pipelineInfo.pStages = 
            stages.data();  
        pipelineInfo.pVertexInputState = 
            &(this->getVertexInputInfo());
        pipelineInfo.pInputAssemblyState = 
            &(this->getInputAssembly());
        pipelineInfo.pViewportState = 
            &(this->getViewportState());
        pipelineInfo.pRasterizationState = 
            &(this->getRasterizer());
        pipelineInfo.pMultisampleState = 
            &(this->getMultisampling());
        pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = 
            &(this->getColorBlending());
        pipelineInfo.pDynamicState = 
            &(this->getDynamicState());

        pipelineInfo.layout = this->getPipelineLayout();

        pipelineInfo.renderPass = this->renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional


        if (vkCreateGraphicsPipelines(this->device->getLogicalDevice(), 
        VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, graphicsPipe) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }
    }


    void v_pipeline_config::initializeDynamicStates()
    {       
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(this->dynamicStates.size());
        dynamicState.pDynamicStates = this->dynamicStates.data();
        dynamicState.flags = 0;

        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        viewportState.pViewports = nullptr;
        viewportState.pScissors = nullptr;
        viewportState.flags = 0;
    }
    
    
    void v_pipeline_config::initializeVertexInput()
    {
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        //auto& bindingDescriptions = Vertex::getBindingDescriptions();
        //auto& attributeDescriptions = Vertex::getAttributeDescriptions();
        auto& bindingDescriptions = this->shader->getBindingDescriptions();
        auto& attributeDescriptions = this->shader->getAttributeDescriptions();

        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        
        /*
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional
        */
    }

    void v_pipeline_config::setInputAssembly(VkPrimitiveTopology topology, bool isEBO)
    {
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = topology;
        inputAssembly.primitiveRestartEnable = isEBO ? VK_TRUE : VK_FALSE;
    }

    void v_pipeline_config::setMultisampling()
    {
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional
    }

    void v_pipeline_config::setColorBlend()
    {
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

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

        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional
    }

    void v_pipeline_config::setDepthClamp(bool isDepthClamp)
    {
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = isDepthClamp ? VK_TRUE : VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;       
    }

    void v_pipeline_config::setPolygonFillMode(VkPolygonMode mode, float linewidth)
    {
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.polygonMode = mode;
        rasterizer.lineWidth = 1.0f;
    }
    
    void v_pipeline_config::setCullMode(VkCullModeFlags cullMode, VkFrontFace face)
    {
        rasterizer.cullMode = cullMode;
        rasterizer.frontFace = face;
    }

    

    void v_pipeline_config::setUniformLayout()
    {
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        if (vkCreatePipelineLayout(this->device->getLogicalDevice(),
                        &pipelineLayoutInfo, nullptr, &pipelineLayout) 
                        != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    


}