#pragma once

#include "Device.h"

namespace Nasha{
    class Pipeline{
    public:
        Pipeline(Device& device,
                 const std::string& vertFilePath,
                 const std::string& fragFilePath);
        ~Pipeline();

        Pipeline(const Pipeline &) = delete;
        Pipeline &operator=(const Pipeline &) = delete;
    private:
        /*--------- Main functions ---------*/
        void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath);

        /*-------- Helper functions --------*/
        static std::vector<char> readFile(const std::string& filePath);
        VkShaderModule createShaderModule(const std::vector<char>& code);
    public:
    private:
        Device& m_device;
        VkShaderModule m_vertShaderModule{};
        VkShaderModule m_fragShaderModule{};
        VkPipelineLayout m_pipelineLayout{};
        VkPipeline m_graphicsPipeline{};
    };
}