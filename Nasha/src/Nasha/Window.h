#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <string>
#include <stdexcept>

namespace Nasha {
    class Window {
    private:
        void initWindow();
        static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);
    public:
        Window(int w, int h, std::string name);
        ~Window();

        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;

        bool shouldClose();
        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
        VkExtent2D getExtent();
        inline bool wasWindowResized(){ return frameBufferResized; }
        inline void resetWindowResizedFlag(){ frameBufferResized = false; }

    private:
        int width;
        int height;
        bool frameBufferResized = false;
        std::string windowName;
        GLFWwindow* window{nullptr};
    };
}  // namespace Nasha

