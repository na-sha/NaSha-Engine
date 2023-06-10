#include "RenderSystem.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>

namespace Nasha{

    struct SimplePushConstant{
        glm::mat2 transform{1.0f};
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    RenderSystem::RenderSystem(Device& device, VkRenderPass renderPass)
        :m_device{device}{
        createPipelineLayout();
        createPipeline(renderPass);
    };
    RenderSystem::~RenderSystem(){
        vkDestroyPipelineLayout(m_device.device(),
                                m_pipelineLayout,
                                nullptr);
    };

    void RenderSystem::createPipelineLayout() {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT |
                                       VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstant);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
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

    void RenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects) {
        // update
        int i = 0;
        for (auto& obj : gameObjects) {
            i += 1;
            obj.m_transform2D.rotation =
                    glm::mod<float>(obj.m_transform2D.rotation + 0.001f * i, 2.f * glm::pi<float>());
        }

        m_pipeline -> bind(commandBuffer);

        for(auto& obj: gameObjects){
            SimplePushConstant push{};
            push.offset = obj.m_transform2D.translation;
            push.color = obj.m_color;
            push.transform = obj.m_transform2D.mat2();

            vkCmdPushConstants(commandBuffer,
                               m_pipelineLayout,
                               VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                               0,
                               sizeof(SimplePushConstant),
                               &push);
            obj.m_model ->bind(commandBuffer);
            obj.m_model ->draw(commandBuffer);
        }
    }

}