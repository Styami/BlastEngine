#ifndef BUFFER_HPP
#define BUFFER_HPP

#include "vertex.hpp"
#include <vulkan/vulkan.hpp>


namespace be {
    class Buffer {        
        public:
            Buffer();
            Buffer(vk::Device device, vk::DeviceSize size);
            Buffer(const Buffer& another);
            Buffer(Buffer&& another);
            Buffer& operator=(const Buffer& another);
            Buffer& operator=(Buffer&& another);
            bool operator==(const Buffer& another) const;
            bool operator!=(const Buffer& another) const;
            void clean();
            void create(vk::BufferUsageFlags usage, vk::SharingMode sharingMode, vk::PhysicalDevice physicalDevice);
            template<typename T>
            void map(const std::vector<T>& data);
            void map();
            template<typename T>
            void update(T* updatedData);
            void copyBuffer(be::Buffer& stagingBuffer, vk::CommandPool commandPool, vk::Queue graphicsQueue);
            const vk::Buffer& getBuffer() const;
            vk::DeviceSize getSize() const;
        private:
            vk::Buffer m_buffer;
            vk::Device m_device;
            vk::DeviceSize m_size;
            vk::DeviceMemory m_memory;
            void* m_data;
    };
}

template<typename T>
void be::Buffer::map(const std::vector<T>& data) {
    m_data = m_device.mapMemory(m_memory, 0, m_size);
    memcpy(m_data, data.data(), m_size);
    m_device.unmapMemory(m_memory);
}

template<typename T>
void be::Buffer::update(T* updatedData) {
    memcpy(m_data, updatedData, m_size);

}

#endif