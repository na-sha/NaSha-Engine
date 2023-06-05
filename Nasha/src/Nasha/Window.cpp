#include "Window.h"

namespace Nasha {
    Window::Window(int w, int h, std::string name)
            : width{w}, height{h}, windowName{std::move(name)} {
        initWindow();
    }

    Window::~Window() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Window::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
    }

    bool Window::shouldClose() {
        return glfwWindowShouldClose(window);
    }

    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
        if(glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS){
            throw std::runtime_error("Failed to Create a window Surface");
        }
    }

    VkExtent2D Window::getExtent() {
        return {static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)};
    }

    void Window::frameBufferResizeCallback(GLFWwindow *window, int width, int height) {
        auto bufferWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        bufferWindow -> frameBufferResized = true;
        bufferWindow -> width = width;
        bufferWindow -> height = height;
    }
}  // namespace Nasha
