#ifndef DESCRIPTOR_HPP
#define DESCRIPTOR_HPP

#include <cstddef>
#include <vulkan/vulkan.hpp>

namespace be {
    class Descriptor {
        public:
            Descriptor();
            Descriptor(vk::Device device, size_t numberFrame);
            Descriptor(const Descriptor& another);
            be::Descriptor& operator=(const Descriptor& another);
            void clean(const vk::DescriptorPool& descriptoPool);
            void build();
            be::Descriptor& setUsage(vk::BufferUsageFlags usage);
            be::Descriptor& setType(vk::DescriptorType type);
            be::Descriptor& setSharingMode(vk::SharingMode sharingMode);
            be::Descriptor& setShaderStage(vk::ShaderStageFlagBits shaderStage);
            be::Descriptor& setDeviceSize(vk::DeviceSize size);
            be::Descriptor& setPhysicalDevice(vk::PhysicalDevice physicalDevice);
            be::Descriptor& setCommandPool(vk::CommandPool commandPool);
            template<typename T>
            void update(const std::vector<T>& dataToUpdate, size_t numFrame);
            void map();
            void allocate(const vk::DescriptorPool& descriptorPool);
            const std::vector<vk::Buffer>& getBuffers() const;
            const std::vector<vk::DescriptorSetLayout>& getLayouts() const;
            const std::vector<vk::DescriptorSet>& getSets() const;

        private:
            uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
            std::vector<vk::Buffer> m_buffers;
            vk::Device m_device;
            vk::DeviceSize m_size;
            std::vector<vk::DeviceMemory> m_memories;
            std::vector<vk::DescriptorSetLayout> m_descriptorSetLayouts;
            std::vector<vk::DescriptorSet> m_descriptorSets;
            std::vector<void*> m_data;
            vk::BufferUsageFlags m_usage;
            vk::DescriptorType m_type;
            vk::SharingMode m_sharingMode;
            vk::ShaderStageFlagBits m_shaderStage;
            vk::PhysicalDevice m_physicalDevice;
            vk::CommandPool m_commandPool;
    };
}

template<typename T>
void be::Descriptor::update(const std::vector<T>& dataToUpdate, size_t numFrame) {
    memcpy(m_data[numFrame], dataToUpdate.data(), m_size);
}
#endif