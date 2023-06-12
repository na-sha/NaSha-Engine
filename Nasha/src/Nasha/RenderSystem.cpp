#include "RenderSystem.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>

namespace Nasha{

    struct SimplePushConstant{
        glm::mat4 transform{1.0f};
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

    void RenderSystem::renderGameObjects(VkCommandBuffer commandBuffer,
                                         std::vector<GameObject>& gameObjects,
                                         const Camera& camera) {
        m_pipeline ->bind(commandBuffer);

        for (auto& obj : gameObjects) {
            obj.m_transform.rotation.y =
                    glm::mod(obj.m_transform.rotation.y + 0.01f, glm::two_pi<float>());
            obj.m_transform.rotation.x =
                    glm::mod(obj.m_transform.rotation.x + 0.005f, glm::two_pi<float>());
        }
        m_pipeline -> bind(commandBuffer);

        for(auto& obj: gameObjects){
            SimplePushConstant push{};
            push.color = obj.m_color;
            push.transform = camera.getProjectionMatrix() * obj.m_transform.mat4();

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