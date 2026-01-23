#ifndef UTILS_HPP
#define UTILS_HPP
#include <vulkan/vulkan.hpp>

uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, vk::PhysicalDevice physicalDevice);
vk::CommandBuffer beginSingleTimeCommands(vk::Device device, vk::CommandPool commandPool);
void endSingleTimeCommands(vk::Device device, vk::CommandPool commandPool, vk::CommandBuffer commandBuffer, vk::Queue graphicsQueue);
vk::Format findSupportedFormat(vk::PhysicalDevice physicalDevice, const std::vector<vk::Format>& formats, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
std::tuple<vk::DeviceMemory, vk::Image> createImage(
    vk::Device device,
    vk::PhysicalDevice physicalDevice,
    vk::ImageType type,
    vk::Format format,
    vk::Extent3D extent,
    uint32_t mipLevel,
    uint32_t arrayLayers,
    vk::SampleCountFlagBits sampleCount,
    vk::ImageTiling tiling,
    vk::ImageUsageFlags usage,
    vk::SharingMode sharingMode,
    vk::MemoryPropertyFlags properties
);

vk::ImageView createImageView(vk::Device device, vk::Image image, vk::ImageViewType type, vk::Format format, vk::ImageSubresourceRange imageSubresourceRange);
#endif