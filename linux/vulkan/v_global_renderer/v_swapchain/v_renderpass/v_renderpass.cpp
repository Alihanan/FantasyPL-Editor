#include "v_renderpass.h"

namespace PL
{
    v_renderpass::v_renderpass(VkFormat& format)
    : format(format)
    {
        this->initialize();
    }

    v_renderpass::~v_renderpass()
    {
        vkDestroyRenderPass(v_device::LDevice(DEVICE_MAIN), renderPass, nullptr);
    }

    void v_renderpass::initialize()
    {
        colorAttachment.format = this->format;
        // no multisampling
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

        // clear after draw
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        // save buffer after draw
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        // not use stencil
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        // format after renderpass should be same as Swapchain
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


        // multiple subpasses
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        // create render pass
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        // # dependencies
        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;


        if (vkCreateRenderPass(v_device::LDevice(DEVICE_MAIN),
                &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }

    }
}