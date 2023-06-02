#pragma once

#include "Device.h"

//Remember to add back to cMake

namespace Nasha{
    class SwapChain {
    public:
        SwapChain();
        ~SwapChain();

        SwapChain(const SwapChain &) = delete;
        SwapChain &operator=(const SwapChain &) = delete;

    private:
        /*--------- Main Functions ---------*/

        /*-------- Helper Functions --------*/
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    public:
    private:
    };
}