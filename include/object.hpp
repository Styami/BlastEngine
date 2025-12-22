#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "vertex.hpp"
#include <vector>

class Object {
    public:
        Object();
        const std::vector<Vertex>& getVertices();
        const std::vector<int>& getIndices();
        const glm::mat4& getModel(); 
    private:
        std::vector<Vertex> vertices;
        std::vector<int> indices;
        glm::mat4 model;
};

#endif