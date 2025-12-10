#include "object.hpp"

Object::Object() :
    vertices({
		{{0, 0.5}, {1, 0, 0}},
		{{0.5, -0.5}, {0, 0, 1}},
		{{-0.5, -0.5}, {0, 1, 0}}
	}),
    model(1)
{}

const std::vector<Vertex>& Object::getVertices() {
    return vertices;
}

const glm::mat4& Object::getModel() {
    return model;
}