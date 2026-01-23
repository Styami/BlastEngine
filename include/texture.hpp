#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include "buffer.hpp"
#include <filesystem>

namespace be {
    class Texture {
        public:
            Texture() = delete;
            Texture(const std::filesystem::path& name, vk::Queue queue);
            static void setDevice(vk::Device device);
            static void setPhysicalDevice(vk::PhysicalDevice physicaldevice);
            void loadImage(const std::filesystem::path& name);
            void createTextureImage(
                vk::ImageType type,
                vk::Format format,
                uint32_t mipLevel,
                uint32_t arrayLayers,
                vk::SampleCountFlagBits sampleCount,
                vk::ImageTiling tiling,
                vk::ImageUsageFlags usage,
                vk::SharingMode sharingMode,
                vk::MemoryPropertyFlags properties
            );
            void copyBufferToImage(vk::CommandPool commandPool);
            void transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::CommandPool commandPool);
    		static void createTextureSampler();
            static void cleanSampler(); 
            vk::ImageView getImageView() const;
            vk::Image getImage() const;
            vk::Buffer getBuffer() const;
            static vk::Sampler getSampler();
            void clean();
        private:
            inline static vk::Device m_device = nullptr;
            inline static vk::PhysicalDevice m_physicalDevice = nullptr;
            vk::Queue m_queue;
            int m_height;
            int m_width;
            int m_texChannels;
            vk::Image m_image;
            vk::ImageView m_imageView;
            vk::DeviceMemory m_memory;
            be::Buffer m_buffer;
            inline static vk::Sampler sampler = nullptr;
    };
}

#endif