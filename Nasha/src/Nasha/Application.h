#pragma once

#include "Core.h"
#include "Window.h"
#include "Device.h"

// std
#include <memory>
#include <vector>

namespace Nasha{
    class API Application{
    public:
        Application();
        virtual ~Application();

        Application(const Application &) = delete;
        Application &operator=(const Application &) = delete;

        void Run();

    private:
        Window m_window{WIDTH, HEIGHT, "Nasha"};
        Device m_device{m_window};

    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;
#ifdef NDEBUG
        static const bool debug = false;
#else
        static const bool debug = true;
#endif
    };

    Application* createApplication();
}
