#pragma once
#include "vk_types.inl"

#include <renderer/vulkan/vk_device.h>

#include <memory>

namespace nasha{
    class SwapChain{
    public:
        SwapChain(VkSetup &deviceRef, VkExtent2D extent);
        SwapChain(VkSetup &deviceRef, VkExtent2D extent, std::shared_ptr<SwapChain> previous);
        ~SwapChain();

        SwapChain(const SwapChain&) = delete;
        SwapChain &operator=(const SwapChain&) = delete;

        [[nodiscard]] uint32_t width() const { return m_swapChainExtent.width; }
        [[nodiscard]] uint32_t height() const { return m_swapChainExtent.height; }

        VkFramebuffer getFrameBuffer(int index) { return m_swapChainFramebuffers[index]; }
        VkRenderPass getRenderPass() { return m_renderPass; }
        VkImageView getImageView(int index) { return m_swapChainImageViews[index]; }

        size_t imageCount() { return m_swapChainImages.size(); }
        VkFormat getSwapChainImageFormat() { return m_swapChainImageFormat; }
        VkExtent2D getSwapChainExtent() { return m_swapChainExtent; }
        float extentAspectRatio() const {
            return static_cast<float>(m_swapChainExtent.width) / static_cast<float>(m_swapChainExtent.height);
        }

        VkFormat findDepthFormat();

        VkResult acquireNextImage(uint32_t *imageIndex);
        VkResult submitCommandBuffers(const VkCommandBuffer *buffers, const uint32_t *imageIndex);

        [[nodiscard]] bool compareSwapChainFormat(const SwapChain& swapChain) const {
            return swapChain.m_swapChainDepthFormat == m_swapChainDepthFormat &&
                   swapChain.m_swapChainImageFormat == m_swapChainImageFormat;
        }

    private:
        void init();
        void createSwapChain();
        void createImageViews();
        void createDepthResources();
        void createRenderPass();
        void createFramebuffers();
        void createSyncObjects();

        // Surface Format
        static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        // Presentation Mode
        static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        // Swap Extent
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    private:
        // Initialising SwapChain
        VkSetup& m_device;
        VkExtent2D m_windowExtent{};
        // Swapchain
        VkSwapchainKHR m_swapChain{};
        std::shared_ptr<SwapChain> m_oldSwapChain{};

        VkFormat m_swapChainImageFormat{};
        VkFormat m_swapChainDepthFormat{};
        VkExtent2D m_swapChainExtent{};

        // Render Pass
        VkRenderPass m_renderPass{};

        // FrameBuffers
        std::vector<VkFramebuffer> m_swapChainFramebuffers{};

        std::vector<VkImage> m_depthImages;
        std::vector<VkDeviceMemory> m_depthImageMemories;
        std::vector<VkImageView> m_depthImageViews;
        std::vector<VkImage> m_swapChainImages{}; // SwapChain images
        std::vector<VkImageView> m_swapChainImageViews{}; // Image Views

        std::vector<VkSemaphore> m_imageAvailableSemaphores;
        std::vector<VkSemaphore> m_renderFinishedSemaphores;
        std::vector<VkFence> m_inFlightFences;
        std::vector<VkFence> m_imagesInFlight;
        size_t m_currentFrame = 0;
    };
}