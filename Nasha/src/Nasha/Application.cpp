#include "Application.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <chrono>

namespace Nasha{
    // UBO --> Uniform buffer object
    struct GlobalUBO{
        glm::mat4 projectionView{1.0f};
        glm::vec3 lightDirection = glm::normalize(glm::vec3{2.0f, -3.0f, -1.0f});
    };

    Application::Application(){
        globalPool = DescriptorPool::Builder(device)
                .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                .build();
        loadGameObjects();
    };
    Application::~Application(){};

    void Application::Run(){
        std::vector<std::unique_ptr<BufferHelpers>> UBOBuffer(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < UBOBuffer.size(); i++) {
            UBOBuffer[i] = std::make_unique<BufferHelpers>(
                    device,
                    sizeof(GlobalUBO),
                    1,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                    );
            UBOBuffer[i]->map();
        }

        auto globalSetLayout = DescriptorSetLayout::Builder(device)
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                .build();

        std::vector<VkDescriptorSet> globalDescriptorSet(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSet.size(); i++) {
            auto bufferInfo = UBOBuffer[i] ->descriptorInfo();
            DescriptorWriter(*globalSetLayout, *globalPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSet[i]);
        }

        RenderSystem simpleRenderSystem{device,
                                        renderer.getSwapChainRenderPass(),
                                        globalSetLayout ->getDescriptorSetLayout()};

        Camera camera{};
        // camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

        auto viewerObject = GameObject::creteGameObject();
        Events cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while(!window.shouldClose()){
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(window.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.m_transform.translation, viewerObject.m_transform.rotation);

            float aspect = renderer.getAspectRatio();
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

            if (auto commandBuffer = renderer.beginFrame()){
                int frameIndex = renderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSet[frameIndex]
                };
                //update
                GlobalUBO ubo{};
                ubo.projectionView = camera.getProjectionMatrix() * camera.getViewMatrix();
                UBOBuffer[frameIndex] ->writeToBuffer(&ubo);
                UBOBuffer[frameIndex] ->flush();

                //render
                renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame();
            }
        }
        vkDeviceWaitIdle(device.device());
    }


    void Application::loadGameObjects() {
        std::shared_ptr<Model> model = Model::createModelFromFile(device, "../Nasha/src/Nasha/models/viking_room.obj");
        auto gameObj = GameObject::creteGameObject();
        gameObj.m_model = model;
        gameObj.m_transform.translation = {.0f, .5f, 2.5f};
        gameObj.m_transform.scale = glm::vec3(3.f);

        gameObjects.push_back((std::move(gameObj)));
    }

}