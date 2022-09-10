#pragma once
#include "../../../va_uncopiable/va_uncopiable.h"
#include "../../../v_device/v_device.h"

namespace PL
{
    class v_renderpass : va_uncopiable
    {
    public:
        v_renderpass(VkFormat& format);
        ~v_renderpass();
        VkRenderPass& getRenderPass() { return this->renderPass;}

    protected:
        void initialize();
        VkFormat& format;
        VkAttachmentDescription colorAttachment{};
        VkAttachmentReference colorAttachmentRef{};
        VkRenderPass renderPass;
        VkRenderPassCreateInfo renderPassInfo{};
        VkSubpassDescription subpass{};

    };



}