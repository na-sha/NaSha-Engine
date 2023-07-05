#include "RenderSystem.h"

#include <vulkan/vulkan.h>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>

namespace Nasha{

    struct SimplePushConstant{
        glm::mat4 modelMatrix{1.0f};
        glm::mat4 normalMatrix{1.0f};
    };

    RenderSystem::RenderSystem(Device& device,
                               VkRenderPass renderPass,
                               VkDescriptorSetLayout globalSetLayout)
        :m_device{device}{
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    };
    RenderSystem::~RenderSystem(){
        vkDestroyPipelineLayout(m_device.device(),
                                m_pipelineLayout,
                                nullptr);
    };

    void RenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT |
                                       VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstant);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if(vkCreatePipelineLayout(m_device.device(),
                                  &pipelineLayoutInfo,
                                  nullptr,
                                  &m_pipelineLayout) != VK_SUCCESS){
            throw std::runtime_error("Failed to create pipeline layout");
        }
    }

    void RenderSystem::createPipeline(VkRenderPass renderPass) {
        assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
        PipelineConfigInfo pipelineConfig{};
        Pipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        m_pipeline = std::make_unique<Pipeline>(m_device,
                                              "../Nasha/src/Nasha/shaders/glsl_shader.vert.spv",
                                              "../Nasha/src/Nasha/shaders/glsl_shader.frag.spv",
                                              pipelineConfig);
    }

    void RenderSystem::renderGameObjects(FrameInfo& frameInfo) {
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

        for (auto& key : frameInfo.gameObject) {
            auto& obj = key.second;
            SimplePushConstant push{};
            push.modelMatrix = obj.m_transform.mat4();
            push.normalMatrix = obj.m_transform.normalMatrix();

            vkCmdPushConstants(frameInfo.commandBuffer,
                               m_pipelineLayout,
                               VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                               0,
                               sizeof(SimplePushConstant),
                               &push);
            obj.m_model ->bind(frameInfo.commandBuffer);
            obj.m_model ->draw(frameInfo.commandBuffer);
        }
    }

}