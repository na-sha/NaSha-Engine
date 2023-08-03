#include <renderer/Renderer.h>

namespace nasha{
    Renderer::Renderer(Window& window, VkSetup& device) : m_window{window}, m_device{device} {
        recreateSwapChain();
        createCommandBuffers();
    }
    Renderer::~Renderer(){
        freeCommandBuffers();
    }

    void Renderer::recreateSwapChain() {
        VkExtent2D extent = m_window.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = m_window.getExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(m_device.device());

        if (m_swapChain == nullptr) {
            m_swapChain = std::make_unique<SwapChain>(m_device, extent);
        }else{
            std::shared_ptr<SwapChain> oldSwapChain = std::move(m_swapChain);
            m_swapChain = std::make_unique<SwapChain>(m_device, extent, oldSwapChain);
            if (!oldSwapChain ->compareSwapChainFormat(*m_swapChain)){
                throw std::runtime_error("Swap chain image(or depth) format has changed!");
            }
        }
    }

    void Renderer::createCommandBuffers() {
        m_commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_device.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

        if (vkAllocateCommandBuffers(m_device.device(),
                                     &allocInfo,
                                     m_commandBuffers.data()) !=VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void Renderer:: freeCommandBuffers(){
        vkFreeCommandBuffers(m_device.device(),
                             m_device.getCommandPool(),
                             static_cast<uint32_t>(m_commandBuffers.size()),
                             m_commandBuffers.data());
        m_commandBuffers.clear();
    }

    VkCommandBuffer Renderer::beginFrame(){
        assert(!m_isFrameStarted && "Can't call beginFrame while already in progress");

        auto result = m_swapChain->acquireNextImage(&m_currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return nullptr;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        m_isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
        return commandBuffer;
    }

    void Renderer::endFrame(){
        assert(m_isFrameStarted && "Can't call endFrame while frame is not in progress");
        auto commandBuffer = getCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }

        auto result = m_swapChain->submitCommandBuffers(&commandBuffer, &m_currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
            m_window.wasWindowResized()) {
            m_window.resetWindowResizedFlag();
            recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        m_isFrameStarted = false;
        m_currentFrameIndex = (m_currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer){
        assert(m_isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_swapChain->getRenderPass();
        renderPassInfo.framebuffer = m_swapChain->getFrameBuffer((int)m_currentImageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_swapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_swapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(m_swapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, m_swapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) const{
        assert(m_isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);
    }
}
