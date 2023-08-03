#pragma once

#include <GLFW/glfw3.h>

#include <core/Window.h>
#include <renderer/vulkan/vk_device.h>
#include <renderer/vulkan/vk_swapchain.h>

#include <memory>
#include <vector>

namespace nasha{
    class Renderer{
    public:
        Renderer(Window& window, VkSetup& device);
        ~Renderer();

        Renderer(const Renderer &) = delete;
        Renderer &operator=(const Renderer &) = delete;

        [[nodiscard]] VkRenderPass getSwapChainRenderPass() const { return m_swapChain->getRenderPass(); }
        [[nodiscard]] float getAspectRatio() const { return m_swapChain->extentAspectRatio(); }
        [[nodiscard]] bool isFrameInProgress() const { return m_isFrameStarted; }

        [[nodiscard]] VkCommandBuffer getCurrentCommandBuffer() const {
            assert(m_isFrameStarted && "Cannot get command buffer when frame not in progress");
            return m_commandBuffers[m_currentFrameIndex];
        }

        [[nodiscard]] int getFrameIndex() const {
            assert(m_isFrameStarted && "Cannot get frame index when frame not in progress");
            return m_currentFrameIndex;
        }

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer) const;

    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

    public:

    private:
        Window& m_window;
        VkSetup& m_device;
        std::unique_ptr<SwapChain> m_swapChain;
        std::vector<VkCommandBuffer> m_commandBuffers{};

        uint32_t m_currentImageIndex{};
        int m_currentFrameIndex{0};
        bool m_isFrameStarted{false};
    };
}