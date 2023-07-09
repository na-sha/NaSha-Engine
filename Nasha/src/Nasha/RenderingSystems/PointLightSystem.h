#pragma once

#include "../FrameInfo.h"
#include "../Pipeline.h"
#include "../Device.h"
#include "../GameObject.h"

#include <memory>
#include <vector>

namespace Nasha {
    class PointLightSystem {
    public:
        PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem &) = delete;
        PointLightSystem &operator=(const PointLightSystem &) = delete;

        void render(FrameInfo& frameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

    public:

    private:
        Device& m_device;
        std::unique_ptr<Pipeline> m_pipeline;
        VkPipelineLayout m_pipelineLayout{};
    };
}

