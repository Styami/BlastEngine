#ifndef DESCRIPTOR_HPP
#define DESCRIPTOR_HPP

#include <cstddef>
#include <vulkan/vulkan.hpp>
#include "buffer.hpp"
#include "texture.hpp"

namespace be {
    class Descriptor {
        public:
            Descriptor();
            Descriptor(vk::Device device);
            Descriptor(const Descriptor& another);
            be::Descriptor& operator=(const Descriptor& another);
            void clean();
            void createSetLayout(const std::vector<vk::DescriptorSetLayoutBinding>& descriptorSetLayoutBinding);
            void createPool(const std::vector<vk::DescriptorPoolSize>& createInfo, int numFrame);
            void createSet(size_t numberFrame, const std::vector<be::Buffer>& buffers, const std::vector<be::Texture>& textures = {});
            const vk::DescriptorSetLayout& getLayout() const;
            size_t getLayoutSize() const;
            const std::vector<vk::DescriptorSet>& getSets() const;

        private:
            vk::Device m_device;
            vk::DescriptorSetLayout m_descriptorSetLayout;
            size_t m_layoutSize;
            std::vector<vk::DescriptorSet> m_descriptorSets;
            vk::DescriptorPool m_descriptorPool;
    };
}
#endif