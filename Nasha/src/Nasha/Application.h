#pragma once

#include "Core.h"
#include "Events.h"
#include "Window.h"
#include "Device.h"
#include "RenderSystem.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Camera.h"

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
        std::vector<GameObject> gameObjects;

    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;
    };

// We have to define it in client
    Application* createApplication();
}

