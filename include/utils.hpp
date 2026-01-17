#ifndef UTILS_HPP
#define UTILS_HPP
#include <vulkan/vulkan.hpp>

uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, vk::PhysicalDevice physicalDevice);
vk::CommandBuffer beginSingleTimeCommands(vk::Device device, vk::CommandPool commandPool);
void endSingleTimeCommands(vk::Device device, vk::CommandPool commandPool, vk::CommandBuffer commandBuffer, vk::Queue graphicsQueue);

#endif