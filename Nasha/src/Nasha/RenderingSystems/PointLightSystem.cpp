#include "PointLightSystem.h"

#include "vulkan/vulkan.h"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

#include <stdexcept>

namespace Nasha{
    PointLightSystem::PointLightSystem(Device& device,
                               VkRenderPass renderPass,
                               VkDescriptorSetLayout globalSetLayout)
            :m_device{device}{
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    };
    PointLightSystem::~PointLightSystem(){
        vkDestroyPipelineLayout(m_device.device(),
                                m_pipelineLayout,
                                nullptr);
    };

    void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
//        VkPushConstantRange pushConstantRange{};
//        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT |
//                                       VK_SHADER_STAGE_FRAGMENT_BIT;
//        pushConstantRange.offset = 0;
//        pushConstantRange.size = sizeof(SimplePushConstant);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if(vkCreatePipelineLayout(m_device.device(),
                                  &pipelineLayoutInfo,
                                  nullptr,
                                  &m_pipelineLayout) != VK_SUCCESS){
            throw std::runtime_error("Failed to create pipeline layout");
        }
    }

    void PointLightSystem::createPipeline(VkRenderPass renderPass) {
        assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
        PipelineConfigInfo pipelineConfig{};
        Pipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        m_pipeline = std::make_unique<Pipeline>(m_device,
                                                "../Nasha/src/Nasha/shaders/point_light.vert.spv",
                                                "../Nasha/src/Nasha/shaders/point_light.frag.spv",
                                                pipelineConfig);
    }

    void PointLightSystem::render(FrameInfo& frameInfo) {
        m_pipeline ->bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(
                frameInfo.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                m_pipelineLayout,
                0,
                1,
                &frameInfo.globalDescriptorSet,
                0,
                nullptr);

        vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
    }

}