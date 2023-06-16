#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <string>
#include <stdexcept>

namespace Nasha {
    class Window {
    public:
        Window(int w, int h, std::string name);
        ~Window();

        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;

        bool shouldClose();
        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
        VkExtent2D getExtent();
        [[nodiscard]] inline bool wasWindowResized() const { return frameBufferResized; }
        inline void resetWindowResizedFlag(){ frameBufferResized = false; }
        [[nodiscard]] GLFWwindow *getGLFWwindow() const { return window; }

    private:
        void initWindow();
        static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);

    public:

    private:
        int width;
        int height;
        bool frameBufferResized = false;
        std::string windowName;
        GLFWwindow* window{nullptr};
    };
}  // namespace Nasha

