#include "Buffers.h"

namespace Nasha{

    Buffers::Buffers(Device& device)
        :m_device{device}{
        createFramebuffers();
    }

    Buffers::~Buffers() {
        for (auto framebuffer : m_swapChainFramebuffers) {
            vkDestroyFramebuffer(m_device.device(), framebuffer, nullptr);
        }
    }

    void Buffers::createFramebuffers() {
        std::vector<VkImageView> swapChainImageViews = m_device.getSwapChainImageViews();
        m_swapChainFramebuffers.resize(swapChainImageViews.size());
        for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            VkImageView attachments[] = {
                    swapChainImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_device.getRenderPass();
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = m_device.width();
            framebufferInfo.height = m_device.height();
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(m_device.device(),
                                    &framebufferInfo,
                                    nullptr,
                                    &m_swapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }
}