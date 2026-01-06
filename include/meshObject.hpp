#ifndef MESHOBJECT_HPP
#define MESHOBJECT_HPP

#include "vertex.hpp"
#include <vector>

class MeshObject {
    public:
        MeshObject();
        const std::vector<Vertex>& getVertices();
        const std::vector<int>& getIndices();
        const glm::mat4& getModel(); 
    private:
        std::vector<Vertex> vertices;
        std::vector<int> indices;
        glm::mat4 model;
};

#endif