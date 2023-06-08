#pragma once

#include "Window.h"
#include "Device.h"
#include "SwapChain.h"

#include <cassert>
#include <memory>
#include <vector>

namespace Nasha {

    class Renderer {
    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void drawFrame();
        void recreateSwapchain();

    public:
        Renderer(Window& window, Device& device);
        ~Renderer();

        Renderer(const Renderer &) = delete;
        Renderer &operator=(const Renderer &) = delete;

    private:
        Window& m_window;
        Device& m_device;
        std::unique_ptr<SwapChain> swapChain;
        std::vector<VkCommandBuffer> commandBuffers;

    public:
    };
}

