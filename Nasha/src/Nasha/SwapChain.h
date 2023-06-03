#pragma once

#include "Device.h"

//Remember to add back to cMake

namespace Nasha{
    class SwapChain {
    public:
        SwapChain(Device &deviceRef, VkExtent2D windowExtent);
        ~SwapChain();

        SwapChain(const SwapChain &) = delete;
        SwapChain &operator=(const SwapChain &) = delete;

    private:
        /*--------- Main Functions ---------*/
        void init();
        void createSwapChain();

        /*-------- Helper Functions --------*/
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    public:
    private:
        Device &m_device;
        VkExtent2D m_windowExtent;
    };
}