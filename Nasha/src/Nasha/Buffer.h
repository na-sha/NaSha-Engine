#pragma once

#include "Device.h"

namespace Nasha {

    class Buffer {
    public:
        Buffer(
                Device& device,
                VkDeviceSize instanceSize,
                uint32_t instanceCount,
                VkBufferUsageFlags usageFlags,
                VkMemoryPropertyFlags memoryPropertyFlags,
                VkDeviceSize minOffsetAlignment = 1);
        ~Buffer();

        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void unmap();

        void writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        void writeToIndex(void* data, int index);
        VkResult flushIndex(int index);
        VkDescriptorBufferInfo descriptorInfoForIndex(int index);
        VkResult invalidateIndex(int index);

        [[nodiscard]] VkBuffer getBuffer() const { return m_buffer; }
        [[nodiscard]] void* getMappedMemory() const { return m_mapped; }
        [[nodiscard]] uint32_t getInstanceCount() const { return m_instanceCount; }
        [[nodiscard]] VkDeviceSize getInstanceSize() const { return m_instanceSize; }
        [[nodiscard]] VkDeviceSize getAlignmentSize() const { return m_instanceSize; }
        [[nodiscard]] VkBufferUsageFlags getUsageFlags() const { return m_usageFlags; }
        [[nodiscard]] VkMemoryPropertyFlags getMemoryPropertyFlags() const { return m_memoryPropertyFlags; }
        [[nodiscard]] VkDeviceSize getBufferSize() const { return m_bufferSize; }

    private:
        static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

        Device& m_device;
        void* m_mapped = nullptr;
        VkBuffer m_buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;

        VkDeviceSize m_bufferSize;
        uint32_t m_instanceCount;
        VkDeviceSize m_instanceSize;
        VkDeviceSize m_alignmentSize;
        VkBufferUsageFlags m_usageFlags;
        VkMemoryPropertyFlags m_memoryPropertyFlags;
    };

}