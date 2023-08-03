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
#include <map>
#include <vector>

namespace nasha{
    class PointLightSystem {
    public:
        PointLightSystem(VkSetup &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem &) = delete;
        PointLightSystem &operator=(const PointLightSystem &) = delete;

        void update(FrameInfo& frameInfo, GlobalUbo& ubo);
        void render(FrameInfo &frameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        VkSetup &lveDevice;

        std::unique_ptr<Pipeline> lvePipeline;
        VkPipelineLayout pipelineLayout{};
    };
}