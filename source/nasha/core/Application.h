#pragma once
#include "Core.inl"

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS //deprecated ig now uses radians as default
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <core/GameObject.h>
#include <core/Window.h>
#include <renderer/vulkan/vk_device.h>
#include <renderer/Renderer.h>
#include <core/Control.h>
#include <core/Camera.h>
#include <systems/SimpleRenderSystem.h>
#include <systems/PointLightSystem.h>
#include <renderer/vulkan/vk_buffer.h>
#include <renderer/vulkan/vk_descriptors.h>

#include <memory>
#include <vector>

namespace nasha{
    class API Application{
            public:
            Application();
            virtual ~Application();

            Application(const Application &) = delete;
            Application &operator=(const Application &) = delete;

            void run();

            private:
            void loadGameObjects();

            public:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            private:
            Window g_window{WIDTH, HEIGHT, "nasha-dev"};
            VkSetup g_vkSetup{g_window};
            Renderer g_renderer{g_window, g_vkSetup};
            std::unique_ptr<DescriptorPool> g_globalPool{};
            GameObject::Map g_gameObjects{};
    };

    Application* createApplication();
}