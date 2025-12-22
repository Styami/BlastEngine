#include "object.hpp"

Object::Object() :
    vertices({
		{{0, 0.75}, {1, 0, 0}},
		{{0.5, 0.0}, {0, 0, 1}},
		{{-0.5, 0.0}, {0, 1, 0}},
        {{0, -0.75}, {1, 0, 0}}
	}),
    indices({0, 1, 2, 3, 2, 1}),
    model(1)
{}

const std::vector<Vertex>& Object::getVertices() {
    return vertices;
}

const std::vector<int>& Object::getIndices() {
    return indices;
}

const glm::mat4& Object::getModel() {
    return model;
}