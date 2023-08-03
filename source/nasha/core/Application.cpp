#include "Application.h"

#include <chrono>

namespace nasha{
    Application::Application() {
        g_globalPool = DescriptorPool::Builder(g_vkSetup)
                .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                .build();
        loadGameObjects();
    }
    Application::~Application()= default;

    void Application::run() {
        std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (std::unique_ptr<Buffer>& uboBuffer : uboBuffers) {
            uboBuffer = std::make_unique<Buffer>(
                    g_vkSetup,
                    sizeof(GlobalUbo),
                    1,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffer->map();
        }

        auto globalSetLayout = DescriptorSetLayout::Builder(g_vkSetup)
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                .build();

        std::vector<VkDescriptorSet> globalDescriptorSet(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSet.size(); i++){
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            DescriptorWriter(*globalSetLayout, *g_globalPool)
                    .writeBuffer(0, &bufferInfo)
                    .build(globalDescriptorSet[i]);
        }

        SimpleRenderSystem simpleRenderSystem{g_vkSetup,
                                              g_renderer.getSwapChainRenderPass(),
                                              globalSetLayout->getDescriptorSetLayout()
        };
        PointLightSystem pointLightSystem{g_vkSetup,
                                          g_renderer.getSwapChainRenderPass(),
                                          globalSetLayout->getDescriptorSetLayout()
        };
        Camera camera{};
        camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

        auto viewerObject = GameObject::createGameObject();
        viewerObject.transform.translation.z = -2.5f;
        Control cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();
        while (!g_window.shouldClose()) {
            glfwPollEvents();
            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;
            cameraController.moveInPlaneXZ(g_window.getGLFWWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
            float aspect = g_renderer.getAspectRatio();
//            camera.setOrthographicProjection(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);
            if (auto commandBuffer = g_renderer.beginFrame()) {
                int frameIndex = g_renderer.getFrameIndex();

                FrameInfo frameInfo{
                        frameIndex,
                        frameTime,
                        commandBuffer,
                        camera,
                        globalDescriptorSet[frameIndex],
                        g_gameObjects
                };

                /* ----- UPDATE ----- */
                GlobalUbo ubo{};
                ubo.projection = camera.getProjection();
                ubo.view = camera.getView();
                ubo.inverseView = camera.getInverseView();
                pointLightSystem.update(frameInfo, ubo);
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                /* ----- RENDER ----- */
                g_renderer.beginSwapChainRenderPass(commandBuffer);

                // Order her matters
                simpleRenderSystem.renderGameObjects(frameInfo);
                pointLightSystem.render(frameInfo);

                g_renderer.endSwapChainRenderPass(commandBuffer);
                g_renderer.endFrame();
            }
        }
        vkDeviceWaitIdle(g_vkSetup.device());
    }

    void Application::loadGameObjects() {
        std::shared_ptr<Model> model = Model::createModelFromFile(g_vkSetup, "../source/nasha/models/flat_vase.obj");
        GameObject flatVase = GameObject::createGameObject();
        flatVase.model = model;
        flatVase.transform.translation = {-0.5f, 0.5f, 0.0f};
        flatVase.transform.scale = {3.0f, 3.0f, 3.0f};
        g_gameObjects.emplace(flatVase.gameId(), std::move(flatVase));

        model = Model::createModelFromFile(g_vkSetup, "../source/nasha/models/smooth_vase.obj");
        GameObject smoothVase = GameObject::createGameObject();
        smoothVase.model = model;
        smoothVase.transform.translation = {0.5f, 0.5f, 0.0f};
        smoothVase.transform.scale = {3.0f, 3.0f, 3.0f};
        g_gameObjects.emplace(smoothVase.gameId(), std::move(smoothVase));

        model = Model::createModelFromFile(g_vkSetup, "../source/nasha/models/quad.obj");
        GameObject floor = GameObject::createGameObject();
        floor.model = model;
        floor.transform.translation = {0.0f, 0.5f, 0.0f};
        floor.transform.scale = {3.0f, 3.0f, 3.0f};
        g_gameObjects.emplace(floor.gameId(), std::move(floor));

        std::vector<glm::vec3> lightColors{
                {1.f, .1f, .1f},
                {.1f, .1f, 1.f},
                {.1f, 1.f, .1f},
                {1.f, 1.f, .1f},
                {.1f, 1.f, 1.f},
                {1.f, 1.f, 1.f}  //
        };

        for (int i = 0; i < lightColors.size(); i++) {
            auto pointLight = GameObject::makePointLight(0.2f);
            pointLight.color = lightColors[i];
            auto rotateLight = glm::rotate(
                    glm::mat4(1.f),
                    (i * glm::two_pi<float>()) / lightColors.size(),
                    {0.f, -1.f, 0.f});
            pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.5f, -1.5f, -1.0f, 1.f));
            g_gameObjects.emplace(pointLight.gameId(), std::move(pointLight));
        }
    }
}
