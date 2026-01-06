#include "meshObject.hpp"

MeshObject::MeshObject() :
    vertices({
		{{0, -1, 0}, {1, 0, 0}},
		{{1, 0.0, 0}, {0, 0, 1}},
		{{-1, 0.0, 0}, {0, 1, 0}},
        {{0, 1, 0}, {1, 0, 0}}
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