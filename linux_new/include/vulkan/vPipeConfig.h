#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"
#include "vDevice.h"
#include "vSwapchain.h"
#include "vShaderManager.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>


namespace PL
{
    class vPipeline;

    class vPipeConfig : public IUncopiable, public IDependent
    {
    public:
        struct vPipeConfigData
        {
            VkPipelineDynamicStateCreateInfo dynamicState{};
            VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
            VkPipelineViewportStateCreateInfo viewportState{};
            VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
            VkPipelineRasterizationStateCreateInfo rasterizer{};
            VkPipelineMultisampleStateCreateInfo multisampling{};
            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            VkPipelineColorBlendStateCreateInfo colorBlending{};
            VkPipelineTessellationStateCreateInfo tesselation{};
            // main info
            VkGraphicsPipelineCreateInfo pipelineInfo{};

            // vertex input + uniform layout
            vShader* shader;
        };
        // IDependent
        const static std::string _DEP_ID;
        // inline const static std::vector<std::string> _DEP_NEEDED_DEPS = {
        //     vDevice::_DEP_ID, vSwapchain::_DEP_ID, vShaderManager::_DEP_ID
        // };
        std::vector<std::string> GetNeededDependencies()
        {
            return {vDevice::_DEP_ID, vSwapchain::_DEP_ID, vShaderManager::_DEP_ID};
        }
        void ReceiveContext(std::vector<std::vector<IDependent*>> context)
        {          
            this->device = static_cast<vDevice*>(context[0][0]);
            this->swapchain = static_cast<vSwapchain*>(context[1][0]);
            this->shaderManager = static_cast<vShaderManager*>(context[2][0]);
            this->Initialize();
        }

        void UpdateContext(std::vector<std::vector<IDependent*>> context)
        {
            auto ret = std::string(typeid(this).name());
        }

        std::string GetDependencyID()
        {
            return this->_DEP_ID;
        }

        // Others
        vPipeConfig() {}
        ~vPipeConfig();

        void SetInputAssembly(VkPrimitiveTopology topology, bool isEBO);
        void SetDepthClamp(bool isDepthClamp);
        void SetPolygonFillMode(VkPolygonMode mode, float linewidth = 1.0f);
        void SetCullMode(VkCullModeFlags cullMode, VkFrontFace face);
        void SetDepthBias()
        {
            data.rasterizer.depthBiasEnable = VK_FALSE;
            data.rasterizer.depthBiasConstantFactor = 0.0f; // Optional
            data.rasterizer.depthBiasClamp = 0.0f; // Optional
            data.rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
        }
        void SetMultisampling();
        void SetColorBlend();
        void SetShaderName(std::string name);
        void SetDynamicStates();
        void SetRenderPass();

        vPipeline* GeneratePipeline();
        
    protected:  
        void Initialize();

        void SetShaderConfig();

        vPipeConfigData data {};
        vDevice* device;
        vSwapchain* swapchain;
        vShaderManager* shaderManager;

        const std::vector<VkDynamicState> dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
            };
    };


}