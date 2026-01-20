#include "vertex.hpp"
#include <vulkan/vulkan_structs.hpp>

vk::VertexInputBindingDescription Vertex::getBindingDescription() {
    return {0, sizeof(Vertex), vk::VertexInputRate::eVertex};
}

std::array<vk::VertexInputAttributeDescription, 3> Vertex::getAttributeDescriptions() {
    return {
        vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, pos)),
        vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)),
        vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, texCoord))
    };
}