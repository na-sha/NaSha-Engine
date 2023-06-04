#pragma once

#include "Device.h"

namespace Nasha{
    class Buffers{
    public:
        explicit Buffers(Device& device);
        ~Buffers();

        Buffers(const Buffers &) = delete;
        Buffers &operator=(const Buffers &) = delete;

    private:
        void createFramebuffers();

    public:
    private:
        Device& m_device;
        std::vector<VkFramebuffer> m_swapChainFramebuffers;
    };
}