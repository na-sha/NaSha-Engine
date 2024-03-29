#pragma once

#include <renderer/vulkan/vk_device.h>
#include <renderer/vulkan/vk_buffer.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace nasha{
    class Model{
    public:
        struct Vertex{
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 uv{};

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

            bool operator==(const Vertex &other) const {
                return position == other.position &&
                       color == other.color &&
                       normal == other.normal &&
                       uv == other.uv;
            }
        };
        struct Builder{
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};

            void loadModels(const std::string& filepath);
        };

        Model(VkSetup& device, const Model::Builder& builder);
        ~Model();

        Model(const Model&) = delete;
        Model &operator=(const Model&) = delete;

        static std::unique_ptr<Model> createModelFromFile(VkSetup& device, const std::string& filepath);

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer) const;
    private:
        void createVertexBuffers(const std::vector<Vertex> &vertices);
        void createIndexBuffers(const std::vector<uint32_t> &indices);
    public:
    private:
        VkSetup& m_device;

        std::unique_ptr<Buffer> m_vertexBuffer{};
        uint32_t m_vertexCount{};

        bool hasIndexBuffer{false};
        std::unique_ptr<Buffer> m_indexBuffer{};
        uint32_t m_indexCount{};
    };
}