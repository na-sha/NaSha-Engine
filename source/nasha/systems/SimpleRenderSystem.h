#pragma once

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS //deprecated -- now radians by default
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <core/Camera.h>
#include <core/GameObject.h>
#include <renderer/vulkan/vk_device.h>
#include <renderer/vulkan/vk_pipeline.h>
#include <core/FrameInfo.h>

#include <memory>
#include <vector>

namespace nasha{
    class SimpleRenderSystem{
    public:
        SimpleRenderSystem(VkSetup& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem &) = delete;
        SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

        void renderGameObjects(FrameInfo& frameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

    public:

    private:
        VkSetup& m_device;
        std::unique_ptr<Pipeline> m_pipeline{};
        VkPipelineLayout m_pipelineLayout{};
    };
}