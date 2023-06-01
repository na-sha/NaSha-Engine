#include "Application.h"

namespace Nasha{
    Application::Application() = default;

    Application::~Application() = default;

    void Application::Run() {
        while(!m_window.shouldClose()){
            glfwPollEvents();
        }
    }
}