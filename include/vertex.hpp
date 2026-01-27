#ifndef VERTEX_HPP
#define VERTEX_HPP
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include <glm/gtx/hash.hpp>
#include <array>
#include <cstddef>
#include <functional>
#include <vulkan/vulkan.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 texCoord;

    bool operator==(const Vertex&) const = default;
    static vk::VertexInputBindingDescription getBindingDescription();
    static std::array<vk::VertexInputAttributeDescription, 4> getAttributeDescriptions();
};

template<>
struct std::hash<Vertex> {
    size_t operator()(const Vertex& vertex) const noexcept{
        return (hash<glm::vec3>()(vertex.pos) ^
                ((hash<glm::vec3>()(vertex.color) << 1) ^
                ((hash<glm::vec3>()(vertex.normal) >> 1) ^
                (hash<glm::vec2>()(vertex.texCoord) << 1) >> 1)));
    }
};

#endif