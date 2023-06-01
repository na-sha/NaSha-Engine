#pragma once

#include "Core.h"
#include <memory>
#include <spdlog/spdlog.h>

namespace Nasha{
    class API Log{
    public:
        static void init();

        inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return m_coreLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return m_clientLogger; }

    private:
        static std::shared_ptr<spdlog::logger> m_coreLogger;
        static std::shared_ptr<spdlog::logger> m_clientLogger;
    };
}

// Core log macros
#define CORE_TRACE(...)    ::Hazel::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CORE_INFO(...)     ::Hazel::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CORE_WARN(...)     ::Hazel::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CORE_ERROR(...)    ::Hazel::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CORE_CRITICAL(...) ::Hazel::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define CLIENT_TRACE(...)         ::Hazel::Log::GetClientLogger()->trace(__VA_ARGS__)
#define CLIENT_INFO(...)          ::Hazel::Log::GetClientLogger()->info(__VA_ARGS__)
#define CLIENT_WARN(...)          ::Hazel::Log::GetClientLogger()->warn(__VA_ARGS__)
#define CLIENT_ERROR(...)         ::Hazel::Log::GetClientLogger()->error(__VA_ARGS__)
#define CLIENT_CRITICAL(...)      ::Hazel::Log::GetClientLogger()->critical(__VA_ARGS__)