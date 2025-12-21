#ifndef BUFFER_HPP
#define BUFFER_HPP

#include "vertex.hpp"
#include <vulkan/vulkan.hpp>


namespace be {
    class Buffer {
        private:
            uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, vk::PhysicalDevice physicalDevice);
            vk::Buffer m_buffer;
            vk::Device m_device;
            vk::DeviceSize m_size;
            vk::DeviceMemory m_memory;

        public:
            Buffer();
            Buffer(vk::Device device, vk::DeviceSize size);
            Buffer(const Buffer& another);
            be::Buffer& operator=(const Buffer& another);
            void clean();
            void create(vk::BufferUsageFlags usage, vk::SharingMode sharingMode, vk::PhysicalDevice physicalDevice);
            void map(const std::vector<Vertex>& vertices);
            void copyBuffer(be::Buffer& stagingBuffer, vk::CommandPool commandPool, vk::Queue graphicsQueue);
            const vk::Buffer& getBuffer(); 
    };
}

#endif