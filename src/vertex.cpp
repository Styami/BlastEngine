#include "vertex.hpp"
#include <cstddef>
#include <vulkan/vulkan_core.h>


VkVertexInputBindingDescription Vertex::getBindingDescription() {
    return {0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX};
}

std::array<VkVertexInputAttributeDescription, 2> Vertex::getAttributeDescriptions() {
    return {
        VkVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, pos)),
        VkVertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color))
    };
}