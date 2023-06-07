#pragma once

#include "Core.h"
#include "Window.h"
#include "Pipeline.h"
#include "Device.h"
#include "SwapChain.h"
#include "GameObject.h"

#include <memory>
#include <vector>

namespace Nasha {

    class API Application {
    private:
        void loadGameObjects();
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void freeCommandBuffers();
        void drawFrame();
        void recreateSwapchain();
        void recordCommandBuffer(int imageIndex);
        void renderGameObjects(VkCommandBuffer commandBuffer);

    public:
        Application();
        virtual ~Application();

        Application(const Application &) = delete;
        Application &operator=(const Application &) = delete;

        void Run();

    private:
        Window window{WIDTH, HEIGHT, "NASHA"};
        Device device{window};
        std::unique_ptr<SwapChain> swapChain;
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout{};
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<GameObject> gameObjects;
//        Pipeline pipeline{device,
//                          "../Nasha/src/Nasha/shaders/glsl_shader.vert.spv",
//                          "../Nasha/src/Nasha/shaders/glsl_shader.frag.spv",
//                          Pipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};

    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;
    };

// We have to define it in client
    Application* createApplication();
}  // namespace Nasha

