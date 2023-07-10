#pragma once

#include "Device.h"
#include "Model.h"
#include <string>
#include <vector>

namespace Nasha{
    struct PipelineConfigInfo{
        PipelineConfigInfo(const PipelineConfigInfo &) = delete;
        PipelineConfigInfo &operator=(const PipelineConfigInfo &) = delete;

        std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };
    class Pipeline{
    private:
        static std::vector<char> readFile(const std::string& filePath);
        void createGraphicsPipeline(const std::string& vertFilePath,
                                    const std::string& fragFilePath,
                                    const PipelineConfigInfo& configInfo);
        void createShaderModule(const std::vector<char>& code,
                                VkShaderModule* shaderModule);
    public:
        Pipeline(Device &device,
                 const std::string& vertFilePath,
                 const std::string& fragFilePath,
                 const PipelineConfigInfo& configInfo);
        ~Pipeline();

//        Pipeline(const Pipeline &) = delete;
//        Pipeline &operator=(const Pipeline &) = delete;

        static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
        void bind(VkCommandBuffer commandBuffer);

    private:
        Device& m_device;
        VkPipeline m_graphicPipeline;
        VkShaderModule m_vertShaderModule;
        VkShaderModule m_fragShaderModule;
    public:
//        data
    };
}