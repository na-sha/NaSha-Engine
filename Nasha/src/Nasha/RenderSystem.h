#pragma once

#include "Pipeline.h"
#include "Device.h"
#include "GameObject.h"

#include <memory>
#include <vector>

namespace Nasha {
    class RenderSystem {
    public:
        RenderSystem(Device& device, VkRenderPass renderPass);
        ~RenderSystem();

        RenderSystem(const RenderSystem &) = delete;
        RenderSystem &operator=(const RenderSystem &) = delete;

        void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects);

    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

    public:

    private:
        Device& m_device;
        std::unique_ptr<Pipeline> m_pipeline;
        VkPipelineLayout m_pipelineLayout{};
    };
}

