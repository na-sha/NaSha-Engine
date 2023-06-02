#include "Device.h"

#include <set>
#include <unordered_set>

namespace Nasha{

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData) {

        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator,
                                          VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                               "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance,
                        pCreateInfo,
                        pAllocator,
                        pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                       VkDebugUtilsMessengerEXT debugMessenger,
                                       const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    Device::Device(Window &window)
        :m_window{window}{
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
    }

    Device::~Device() {
        vkDestroySwapchainKHR(m_device ,m_swapChain , nullptr);
        vkDestroyDevice(m_device, nullptr);
        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(m_instance,
                                          m_debugMessenger,
                                          nullptr);
        }
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        vkDestroyInstance(m_instance, nullptr);
    };

    /*--------- Main Functions ---------*/
    void Device::createInstance() {
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }
        VkApplicationInfo applicationInfo{};
        /*
        typedef struct VkApplicationInfo {
            VkStructureType    sType;
            const void*        pNext;
            const char*        pApplicationName;
            uint32_t           applicationVersion;
            const char*        pEngineName;
            uint32_t           engineVersion;
            uint32_t           apiVersion;
        } VkApplicationInfo;
         */
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "Nasha";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "No Engine";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = VK_API_VERSION_1_2;

        VkInstanceCreateInfo createInfo{};
        /*
        typedef struct VkInstanceCreateInfo {
            VkStructureType             sType;
            const void*                 pNext;
            VkInstanceCreateFlags       flags;
            const VkApplicationInfo*    pApplicationInfo;
            uint32_t                    enabledLayerCount;
            const char* const*          ppEnabledLayerNames;
            uint32_t                    enabledExtensionCount;
            const char* const*          ppEnabledExtensionNames;
        } VkInstanceCreateInfo;
        */
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        createInfo.pApplicationInfo = &applicationInfo;
        std::vector<const char*> extensions = getRequiredExtensions();
        extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
            createInfo.ppEnabledLayerNames = m_validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo,
                             nullptr,
                             &m_instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
        checkExtensionValidationSupport();
    }

    void Device::setupDebugMessenger() {
        if (!enableValidationLayers) return;
        /*
        typedef struct VkDebugUtilsMessengerCreateInfoEXT {
            VkStructureType                         sType;
            const void*                             pNext;
            VkDebugUtilsMessengerCreateFlagsEXT     flags;
            VkDebugUtilsMessageSeverityFlagsEXT     messageSeverity;
            VkDebugUtilsMessageTypeFlagsEXT         messageType;
            PFN_vkDebugUtilsMessengerCallbackEXT    pfnUserCallback;
            void*                                   pUserData;
        } VkDebugUtilsMessengerCreateInfoEXT;
        */
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(m_instance,
                                         &createInfo,
                                         nullptr,
                                         &m_debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    void Device::createSurface(){
        m_window.createWindowSurface(m_instance, &m_surface);
    }

    void Device::pickPhysicalDevice(){
        uint32_t deviceCount = 0;
        /*
        VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDevices(
            VkInstance                                  instance,
            uint32_t*                                   pPhysicalDeviceCount,
            VkPhysicalDevice*                           pPhysicalDevices);
        */
        vkEnumeratePhysicalDevices(m_instance,
                                   &deviceCount,
                                   nullptr);
        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_instance,
                                   &deviceCount,
                                   devices.data());

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                m_physicalDevice = device;
                break;
            }
        }

        if (m_physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    void Device::createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

        /*
        typedef struct VkDeviceQueueCreateInfo {
            VkStructureType             sType;
            const void*                 pNext;
            VkDeviceQueueCreateFlags    flags;
            uint32_t                    queueFamilyIndex;
            uint32_t                    queueCount;
            const float*                pQueuePriorities;
        } VkDeviceQueueCreateInfo;
        */
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        /*
        typedef struct VkDeviceCreateInfo {
            VkStructureType                    sType;
            const void*                        pNext;
            VkDeviceCreateFlags                flags;
            uint32_t                           queueCreateInfoCount;
            const VkDeviceQueueCreateInfo*     pQueueCreateInfos;
            uint32_t                           enabledLayerCount;
            const char* const*                 ppEnabledLayerNames;
            uint32_t                           enabledExtensionCount;
            const char* const*                 ppEnabledExtensionNames;
            const VkPhysicalDeviceFeatures*    pEnabledFeatures;
        } VkDeviceCreateInfo;
        */
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = 1;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
            createInfo.ppEnabledLayerNames = m_validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(m_physicalDevice,
                           &createInfo,
                           nullptr,
                           &m_device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }
        vkGetDeviceQueue(m_device,
                         indices.graphicsFamily.value(),
                         0,
                         &m_graphicsQueue);
        vkGetDeviceQueue(m_device,
                         indices.graphicsFamily.value(),
                         0,
                         &m_presentQueue);
    }

    void Device::createSwapChain() {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice);

        /*
        typedef struct VkSurfaceFormatKHR {
            VkFormat           format;
            VkColorSpaceKHR    colorSpace;
        } VkSurfaceFormatKHR;
        */
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        /*
        typedef enum VkPresentModeKHR {
            VK_PRESENT_MODE_IMMEDIATE_KHR = 0,
            VK_PRESENT_MODE_MAILBOX_KHR = 1,
            VK_PRESENT_MODE_FIFO_KHR = 2,
            VK_PRESENT_MODE_FIFO_RELAXED_KHR = 3,
            VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR = 1000111000,
            VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR = 1000111001,
            VK_PRESENT_MODE_MAX_ENUM_KHR = 0x7FFFFFFF
        } VkPresentModeKHR;
        */
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        /*
        typedef struct VkExtent2D {
            uint32_t    width;
            uint32_t    height;
        } VkExtent2D;
        */
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        /*
        typedef struct VkSwapchainCreateInfoKHR {
            VkStructureType                  sType;
            const void*                      pNext;
            VkSwapchainCreateFlagsKHR        flags;
            VkSurfaceKHR                     surface;
            uint32_t                         minImageCount;
            VkFormat                         imageFormat;
            VkColorSpaceKHR                  imageColorSpace;
            VkExtent2D                       imageExtent;
            uint32_t                         imageArrayLayers;
            VkImageUsageFlags                imageUsage;
            VkSharingMode                    imageSharingMode;
            uint32_t                         queueFamilyIndexCount;
            const uint32_t*                  pQueueFamilyIndices;
            VkSurfaceTransformFlagBitsKHR    preTransform;
            VkCompositeAlphaFlagBitsKHR      compositeAlpha;
            VkPresentModeKHR                 presentMode;
            VkBool32                         clipped;
            VkSwapchainKHR                   oldSwapchain;
        } VkSwapchainCreateInfoKHR;
        */
        VkSwapchainCreateInfoKHR createSwapChainInfo{};
        createSwapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createSwapChainInfo.surface = m_surface;
        createSwapChainInfo.minImageCount = imageCount;
        createSwapChainInfo.imageFormat = surfaceFormat.format;
        createSwapChainInfo.imageColorSpace = surfaceFormat.colorSpace;
        createSwapChainInfo.imageExtent = extent;
        createSwapChainInfo.imageArrayLayers = 1;
        createSwapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily) {
            createSwapChainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createSwapChainInfo.queueFamilyIndexCount = 2;
            createSwapChainInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createSwapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createSwapChainInfo.queueFamilyIndexCount = 0; // Optional
            createSwapChainInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createSwapChainInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createSwapChainInfo.compositeAlpha =VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createSwapChainInfo.presentMode = presentMode;
        createSwapChainInfo.clipped =VK_TRUE;
        createSwapChainInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_device,
                                 &createSwapChainInfo,
                                 nullptr,
                                 &m_swapChain) != VK_SUCCESS){
            throw std::runtime_error("failed to create swap chain!");
        }
    }

    /*--------- Helper Functions ---------*/
    std::vector<const char*> Device::getRequiredExtensions() const {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        // _f is for formatted --- OK, is there a better naming convention?
        std::vector<const char*> glfwExtensions_f(glfwExtensions,
                                                  glfwExtensions+glfwExtensionCount);
        if (enableValidationLayers) {
            glfwExtensions_f.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

//        std::cout << "available GLFW instance extensions:\n";
//
//        for (const auto& extension : glfwExtensions_f) {
//            std::cout << '\t' << extension << '\n';
//        }
        return glfwExtensions_f;
    }

    void Device::checkExtensionValidationSupport(){
        uint32_t extensionCount = 0;
        /*
        VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(
            const char*                                 pLayerName,
            uint32_t*                                   pPropertyCount,
            VkExtensionProperties*                      pProperties);
        */
        vkEnumerateInstanceExtensionProperties(nullptr,
                                               &extensionCount,
                                               nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr,
                                               &extensionCount,
                                               extensions.data());
//        std::cout << "available extensions:\n";
//
//        for (const auto& extension : extensions) {
//            std::cout << '\t' << extension.extensionName << '\t' << extension.specVersion << '\n';
//        }
    }

    bool Device::checkValidationLayerSupport() {
        uint32_t layerCount;
        /*
        VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(
            uint32_t*                                   pPropertyCount,
            VkLayerProperties*                          pProperties);
         */
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

//        std::cout << "available properties:\n";
//
//        for (const auto& properties : availableLayers) {
//            std::cout << '\t' << properties.layerName <<  '\n';
//        }

        for (const char* layerName : m_validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    void Device::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional
    }

    bool Device::isDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }

    bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(m_deviceExtensions.begin(),
                                                 m_deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device,
                                                 &queueFamilyCount,
                                                 nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device,
                                                 &queueFamilyCount,
                                                 queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device,
                                                 i,
                                                 m_surface,
                                                 &presentSupport);
            if (presentSupport) {
                indices.presentFamily = i;
            }
            if (indices.isComplete()) {
                break;
            }
            i++;
        }

        return indices;
    }

    SwapChainSupportDetails Device::querySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,
                                                  m_surface,
                                                  &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device,
                                             m_surface,
                                             &formatCount,
                                             nullptr);
        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device,
                                                 m_surface,
                                                 &formatCount,
                                                 details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device,
                                                  m_surface,
                                                  &presentModeCount,
                                                  nullptr);
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device,
                                                      m_surface,
                                                      &presentModeCount,
                                                      details.presentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR Device::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR Device::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                std::cout << "Present mode: Mailbox" << std::endl;
                return availablePresentMode;
            }
        }
        std::cout << "Present mode: V_Sync(FIFO)" << std::endl;
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D Device::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            VkExtent2D actualExtent = m_windowExtent;
            actualExtent.width = std::max(
                    capabilities.minImageExtent.width,
                    std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(
                    capabilities.minImageExtent.height,
                    std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }
}
