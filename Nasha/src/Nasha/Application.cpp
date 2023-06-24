#include "Application.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <chrono>

namespace Nasha{

    Application::Application(){
        loadGameObjects();
    };
    Application::~Application(){};

    void Application::Run(){
        RenderSystem simpleRenderSystem{device, renderer.getSwapChainRenderPass()};

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
                renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer,
                                                     gameObjects,
                                                     camera);
                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame();
            }
        }
        vkDeviceWaitIdle(device.device());
    }


    void Application::loadGameObjects() {
        std::shared_ptr<Model> model = Model::createModelFromFile(device, "../Nasha/src/Nasha/models/flat_vase.obj");
        auto gameObj = GameObject::creteGameObject();
        gameObj.m_model = model;
        gameObj.m_transform.translation = {.0f, .5f, 2.5f};
        gameObj.m_transform.scale = glm::vec3(3.f);

        gameObjects.push_back((std::move(gameObj)));
    }

}