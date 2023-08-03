#include "Model.h"

#include "Utils.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <unordered_map>

namespace std {
    template <>
    struct hash<nasha::Model::Vertex> {
        size_t operator()(nasha::Model::Vertex const &vertex) const {
            size_t seed = 0;
            nasha::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };
}

namespace nasha{

    Model::Model(VkSetup& device, const Model::Builder& builder) : m_device{device}{
        createVertexBuffers(builder.vertices);
        createIndexBuffers(builder.indices);
    }

    Model::~Model()= default;

    std::unique_ptr<Model> Model::createModelFromFile(VkSetup &device, const std::string &filepath) {
        Builder builder{};
        builder.loadModels(filepath);
        return std::make_unique<Model>(device, builder);
    }

    void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
        m_vertexCount = static_cast<uint32_t>(vertices.size());
        assert(m_vertexCount >=3 && "Vertex Count must be greater or equal to 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;
        uint32_t vertexSize = sizeof(vertices[0]);

        Buffer stagingBuffer{m_device,
                             vertexSize,
                             m_vertexCount,
                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void *)vertices.data());

        m_vertexBuffer = std::make_unique<Buffer>(m_device,
                                                  vertexSize,
                                                  m_vertexCount,
                                                  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        m_device.copyBuffer(stagingBuffer.getBuffer(), m_vertexBuffer->getBuffer(), bufferSize);
    }

    void Model::createIndexBuffers(const std::vector<uint32_t> &indices) {
        m_indexCount = static_cast<uint32_t>(indices.size());
        hasIndexBuffer = m_indexCount > 0;
        if (!hasIndexBuffer) {
            return;
        }
        VkDeviceSize bufferSize = sizeof(indices[0]) * m_indexCount;
        uint32_t indexSize = sizeof(indices[0]);

        Buffer stagingBuffer{m_device,
                             indexSize,
                             m_indexCount,
                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void *)indices.data());

        m_indexBuffer = std::make_unique<Buffer>(m_device,
                                                 indexSize,
                                                 m_indexCount,
                                                 VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        m_device.copyBuffer(stagingBuffer.getBuffer(), m_indexBuffer->getBuffer(), bufferSize);
    }

    void Model::bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = { m_vertexBuffer->getBuffer() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

        if (hasIndexBuffer){
            vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void Model::draw(VkCommandBuffer commandBuffer) const {
        if (hasIndexBuffer){
            vkCmdDrawIndexed(commandBuffer,
                             m_indexCount,
                             1, 0, 0, 0);
        }else {
            vkCmdDraw(commandBuffer, m_vertexCount, 1, 0, 0);
        }
    }

    std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back({0, 0,
                                         VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
        attributeDescriptions.push_back({1, 0,
                                         VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
        attributeDescriptions.push_back({2, 0,
                                         VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
        attributeDescriptions.push_back({3, 0,
                                         VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});
        return attributeDescriptions;
    }

    void Model::Builder::loadModels(const std::string &filepath) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn;
        std::string error;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, filepath.c_str())){
            throw std::runtime_error(warn + error);
        }
        vertices.clear();
        indices.clear();

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (const auto& shape: shapes){
            for (const auto& index: shape.mesh.indices) {
                Vertex vertex{};

                if (index.vertex_index >= 0){
                    vertex.position = {
                            attrib.vertices[3 * index.vertex_index + 0],
                            attrib.vertices[3 * index.vertex_index + 1],
                            attrib.vertices[3 * index.vertex_index + 2]
                    };

                    vertex.color = {
                            attrib.colors[3 * index.vertex_index + 0],
                            attrib.colors[3 * index.vertex_index + 1],
                            attrib.colors[3 * index.vertex_index + 2]
                    };
                }
                if (index.normal_index >= 0){
                    vertex.normal = {
                            attrib.normals[3 * index.normal_index + 0],
                            attrib.normals[3 * index.normal_index + 1],
                            attrib.normals[3 * index.normal_index + 2]
                    };
                }
                if (index.texcoord_index >= 0){
                    vertex.uv = {
                            attrib.texcoords[2 * index.texcoord_index + 0],
                            attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                }

                if (uniqueVertices.count(vertex) == 0){
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }
                indices.push_back(uniqueVertices[vertex]);
            }
        }
    }
}