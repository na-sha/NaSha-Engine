#pragma once

#include "Window.h"

//std
#include <iostream>
#include <vector>
#include <optional>

namespace Nasha{

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class Device{
#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif
    public:
        Device(Window &window);
        ~Device();

        Device(const Device &) = delete;
        Device &operator=(const Device &) = delete;

    private:
        /*--------- Main Functions ---------*/
        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSwapChain();

        /*-------- Helper Functions --------*/
        [[nodiscard]] std::vector<const char*> getRequiredExtensions() const;
        static void checkExtensionValidationSupport();
        bool checkValidationLayerSupport();
        static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        bool isDeviceSuitable(VkPhysicalDevice device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    private:
        VkInstance m_instance{};
        VkDebugUtilsMessengerEXT m_debugMessenger{};
        VkSurfaceKHR m_surface;
        Window &m_window;
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        VkDevice m_device;
        VkQueue m_graphicsQueue;
        VkQueue m_presentQueue;
        VkExtent2D m_windowExtent;
        VkSwapchainKHR m_swapChain;
        std::vector<VkImage> swapChainImages;

        const std::vector<const char*> m_validationLayers = { "VK_LAYER_KHRONOS_validation" };
        const std::vector<const char *> m_deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset" };
    };
}