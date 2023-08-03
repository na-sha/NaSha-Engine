#pragma once
#include "vk_types.inl"

#include <core/Window.h>

namespace nasha{
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails {
        // Basic surface capabilities (min/max number of images in swap chain, min/max width and height of images)
        VkSurfaceCapabilitiesKHR capabilities;
        // Surface formats (pixel format, color space)
        std::vector<VkSurfaceFormatKHR> formats;
        // Available presentation modes
        std::vector<VkPresentModeKHR> presentModes;
    };

    class VkSetup{
    public:
#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif
        explicit VkSetup(Window &window);
        ~VkSetup();

        VkCommandPool getCommandPool() { return m_commandPool; }
        VkDevice device() { return m_device; }
        VkSurfaceKHR surface() { return m_surface; }
        VkQueue graphicsQueue() { return m_graphicsQueue; }
        VkQueue presentQueue() { return m_presentQueue; }

        SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(m_physicalDevice); }
        VkSurfaceKHR getSurface() { return m_surface; }
        QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(m_physicalDevice); }

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates,
                                     VkImageTiling tiling,
                                     VkFormatFeatureFlags features);

        // BufferHelpers Helper Functions
        void createBuffer(
                VkDeviceSize size,
                VkBufferUsageFlags usage,
                VkMemoryPropertyFlags properties,
                VkBuffer &buffer,
                VkDeviceMemory &bufferMemory);
        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void copyBufferToImage(
                VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

        void createImageWithInfo(
                const VkImageCreateInfo &imageInfo,
                VkMemoryPropertyFlags properties,
                VkImage &image,
                VkDeviceMemory &imageMemory);
    private:
        // Instance creation
        void createInstance();
        [[nodiscard]] std::vector<const char*> getRequiredExtensions() const;

        // Debugging & Validation Layers
        bool checkValidationLayerSupport();
        static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void setupDebugMessenger();

        // Surface creation
        void createSurface();

        // Physical Devices
        void pickPhysicalDevice();
        bool isDeviceSuitable(VkPhysicalDevice device);

        // Queue Families
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

        // Logical Devices
        void createLogicalDevice();
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        // SwapChain support
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

        // Command Pool
        void createCommandPool();
    public:
        VkPhysicalDeviceProperties properties{};

    private:
        const std::vector<const char *> validationLayers = { "VK_LAYER_KHRONOS_validation" };
        const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset" };
        Window &m_window;
        //Instance
        VkInstance m_instance{};
        // debugging
        VkDebugUtilsMessengerEXT debugMessenger{};
        // Surface creation
        VkSurfaceKHR m_surface{};
        //Physical Devices
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        // Logical Devices
        VkDevice m_device{};
        // Queues
        VkQueue m_graphicsQueue{}; // Graphics queue
        VkQueue m_presentQueue{}; // Presentation Queue
        // Command Pool
        VkCommandPool m_commandPool{};
    };
}