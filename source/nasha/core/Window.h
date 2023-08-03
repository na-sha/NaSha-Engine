#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>

namespace nasha{
    class Window{
    public:
        Window(int width, int height, const char* windowName);
        ~Window();

        Window(const Window&) = delete;
        Window &operator=(const Window&) = delete;

        // Used during the main game loop to check whether window is open or not
        bool shouldClose();
        // Accessed for the creation of surface during setup phase
        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

        [[nodiscard]] VkExtent2D getExtent() const;
        [[nodiscard]] bool wasWindowResized() const { return frameBufferResized; }
        void resetWindowResizedFlag() { frameBufferResized = false; }
        [[nodiscard]] GLFWwindow* getGLFWWindow() const { return m_window; }
    private:
        void initWindow();
        static void frameBufferResizeCallback(GLFWwindow *refWindow, int width, int height);

    public:
    private:
        int m_width{}, m_height{};
        const char* m_windowName{};
        GLFWwindow* m_window{};
        bool frameBufferResized{false};
    };
}