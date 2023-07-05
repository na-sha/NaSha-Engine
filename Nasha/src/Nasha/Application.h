#pragma once

#include "Core.h"
#include "Events.h"
#include "Window.h"
#include "Device.h"
#include "RenderSystem.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Camera.h"
#include "BufferHelpers.h"
#include "Descriptors.h"

#include <memory>
#include <vector>
#include <stdexcept>

namespace Nasha {

    class API Application {
    private:
        void loadGameObjects();

    public:
        Application();
        virtual ~Application();

        Application(const Application &) = delete;
        Application &operator=(const Application &) = delete;

        void Run();

    private:
        Window window{WIDTH, HEIGHT, "NASHA"};
        Device device{window};
        Renderer renderer{window, device};

        std::unique_ptr<DescriptorPool> globalPool{};
        GameObject::Map gameObjects;

    public:
        static constexpr int WIDTH = 1024;
        static constexpr int HEIGHT = 768;
    };

// We have to define it in client
    Application* createApplication();
}

