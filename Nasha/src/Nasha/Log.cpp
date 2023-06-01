#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Nasha{
    void Log::init() {
        std::vector<spdlog::sink_ptr> logSinks;
        logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Nasha.log", true));

        logSinks[0]->set_pattern("%^[%T] %n: %v%$");
        logSinks[1]->set_pattern("[%T] [%l] %n: %v");

        m_coreLogger = std::make_shared<spdlog::logger>("Nasha", begin(logSinks), end(logSinks));
        spdlog::register_logger(m_coreLogger);
        m_coreLogger->set_level(spdlog::level::trace);
        m_coreLogger->flush_on(spdlog::level::trace);

        m_clientLogger = std::make_shared<spdlog::logger>("Sandbox", begin(logSinks), end(logSinks));
        spdlog::register_logger(m_clientLogger);
        m_clientLogger->set_level(spdlog::level::trace);
        m_clientLogger->flush_on(spdlog::level::trace);
    }
}