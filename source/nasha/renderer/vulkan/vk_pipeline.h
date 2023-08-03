#pragma once
#include "vk_types.inl"

#include <renderer/vulkan/vk_device.h>

#include <string>
#include <vector>

namespace nasha{
    struct PipelineConfigInfo{
        PipelineConfigInfo() = default;
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo &operator=(const PipelineConfigInfo&) = delete;

        std::vector<VkVertexInputBindingDescription> bindingDescription{};
        std::vector<VkVertexInputAttributeDescription> attributeDescription{};
        VkPipelineViewportStateCreateInfo viewportInfo{};
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
        VkPipelineMultisampleStateCreateInfo multisampleInfo{};
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
        std::vector<VkDynamicState> dynamicStateEnables{};
        VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };
    class Pipeline{
    public:
        Pipeline(VkSetup& device, const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);
        ~Pipeline();

        Pipeline(const Pipeline&) = delete;
        Pipeline &operator=(const Pipeline&) = delete;

        void bind(VkCommandBuffer commandBuffer);

        static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
        static void enableAlphaBlending(PipelineConfigInfo& configInfo);

    private:
        void createGraphicPipeline(const std::string& vertFilePath,
                                   const std::string& fragFilePath,
                                   const PipelineConfigInfo& configInfo);

        // Helper Function
        static std::vector<char> readFile(const std::string& filePath);
        void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

    public:
    private:
        VkSetup& m_device;
        // Graphics Pipeline
        VkPipeline m_graphicsPipeline{};
        // Shader Modules
        VkShaderModule m_vertShaderModule{};
        VkShaderModule m_fragShaderModule{};

        VkRenderPass renderPass{}; // Render Pass
        VkPipelineLayout m_pipelineLayout{};
    };
}