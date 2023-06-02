#pragma once

#include "Window.h"

//std
#include <iostream>
#include <vector>

namespace Nasha{
    class Device{
#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif
    public:
        Device();
        ~Device();

        Device(const Device &) = delete;
        Device &operator=(const Device &) = delete;

    private:
        /*--------- Main Functions ---------*/
        void createInstance();
        void setupDebugMessenger();
        void pickPhysicalDevice();

        /*-------- Helper Functions --------*/
        [[nodiscard]] std::vector<const char*> getRequiredExtensions() const;
        static void checkExtensionValidationSupport();
        bool checkValidationLayerSupport();
        static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        bool isDeviceSuitable(VkPhysicalDevice device);

    private:
        VkInstance m_instance{};
        VkDebugUtilsMessengerEXT m_debugMessenger{};
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

        const std::vector<const char*> m_validationLayers = { "VK_LAYER_KHRONOS_validation" };
    };
}