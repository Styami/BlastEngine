#ifndef VERTEX_HPP
#define VERTEX_HPP
#include "glm/glm.hpp"
#include <array>
#include <vulkan/vulkan.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static vk::VertexInputBindingDescription getBindingDescription();
    static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions();
};

#endif