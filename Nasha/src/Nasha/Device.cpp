#include "Device.h"

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

    Device::Device() {
        createInstance();
        setupDebugMessenger();
        pickPhysicalDevice();
    }

    Device::~Device() {
        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(m_instance,
                                          m_debugMessenger,
                                          nullptr);
        }
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

    bool isDeviceSuitable(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
               deviceFeatures.geometryShader;
    }
}
