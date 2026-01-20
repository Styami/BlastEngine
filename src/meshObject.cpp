#include "meshObject.hpp"

MeshObject::MeshObject() :
    vertices({
		{glm::vec3(-1.02, 0.0, 0) + glm::normalize(glm::vec3(std::sqrt(2)/2, -std::sqrt(2)/2, 0)), {1, 0, 0}, {0, 0}},
		{glm::vec3(-1.02, 0.0, 0) + glm::normalize(glm::vec3(std::sqrt(2)/2, -std::sqrt(2)/2, 0)) + (16.f/9) * glm::normalize(glm::vec3(std::sqrt(2)/2, std::sqrt(2)/2, 0)), {0, 0, 1}, {1, 0}},
		{{-1.02, 0.0, 0}, {0, 1, 0}, {0, 1}},
        {glm::vec3(-1.02, 0.0, 0) + (16.f/9) * glm::normalize(glm::vec3(std::sqrt(2)/2, std::sqrt(2)/2, 0)), {1, 0, 0}, {1, 1}}
	}),
    indices({0, 1, 2, 3, 2, 1}),
    model(1)
{}

const std::vector<Vertex>& MeshObject::getVertices() {
    return vertices;
}

const std::vector<int>& MeshObject::getIndices() {
    return indices;
}

const glm::mat4& MeshObject::getModel() {
    return model;
}