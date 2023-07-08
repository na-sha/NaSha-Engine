//these files are not compiled but just all the code in one place

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <unordered_set>
#include <optional>
#include <set>
#include <cstdint> // Necessary for uint32_t
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp
#include <fstream>

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

    struct Vertex {
        glm::vec2 pos;
        glm::vec3 color;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            return attributeDescriptions;
        }
    };

    class Application{
    public:
#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif
        Application();
        ~Application();

        Application(const Application &) = delete;
        Application &operator=(const Application &) = delete;

        void run();

    private:
        void initWindow();
        void initVulkan();
        void mainLoop();
        void cleanup();
        void createInstance();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSurface();
        void createSwapChain();
        void createImageViews();
        void createGraphicsPipeline();
        void createRenderPass();
        void createFramebuffers();
        void createCommandPool();
        void createVertexBuffer();
        void createCommandBuffer();
        void drawFrame();
        void createSyncObjects();
        void recreateSwapChain();
        void cleanupSwapChain();

        [[nodiscard]] std::vector<const char *> getRequiredExtensions() const;
        void hasGLFWRequiredInstanceExtensions();
        bool checkValidationLayerSupport();
        void setupDebugMessenger();
        static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        bool isDeviceSuitable(VkPhysicalDevice device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
        static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        static std::vector<char> readFile(const std::string& filename);
        VkShaderModule createShaderModule(const std::vector<char>& code);
        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    public:
        static constexpr unsigned int WIDTH = 800;
        static constexpr unsigned int HEIGHT = 600;

        const int MAX_FRAMES_IN_FLIGHT = 2;
    private:
        GLFWwindow* g_window{nullptr};
        VkInstance g_instance{};
        VkDebugUtilsMessengerEXT debugMessenger{};
        VkPhysicalDevice g_physicalDevice = VK_NULL_HANDLE;
        VkDevice g_device{};
        VkQueue g_graphicsQueue{};
        VkSurfaceKHR g_surface{};
        VkQueue g_presentQueue{};
        VkSwapchainKHR g_swapChain{};
        std::vector<VkImage> g_swapChainImages{};
        VkFormat g_swapChainImageFormat{};
        VkExtent2D g_swapChainExtent{};
        std::vector<VkImageView> g_swapChainImageViews{};
        VkRenderPass g_renderPass{};
        VkPipelineLayout g_pipelineLayout{};
        VkPipeline g_graphicsPipeline{};
        std::vector<VkFramebuffer> g_swapChainFramebuffers{};
        VkBuffer g_vertexBuffer{};
        VkDeviceMemory g_vertexBufferMemory{};

        VkCommandPool g_commandPool{};
        std::vector<VkCommandBuffer> g_commandBuffers;

        std::vector<VkSemaphore> g_imageAvailableSemaphores;
        std::vector<VkSemaphore> g_renderFinishedSemaphores;
        std::vector<VkFence> g_inFlightFences;
        bool framebufferResized = false;

        uint32_t currentFrame = 0;
        const std::vector<const char *> validationLayers = { "VK_LAYER_KHRONOS_validation" };
        const std::vector<const char *> deviceExtensions = { "VK_KHR_portability_subset", VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        const std::vector<Vertex> vertices = {
                {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
                {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };
    };
}
