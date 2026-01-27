#include "meshObject.hpp"
#include "vertex.hpp"
#include <cstddef>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <iostream>

MeshObject::MeshObject(const std::filesystem::path& filename) :
    model(1)
{
    //tinyobj::ObjReaderConfig readerConfig;
    //readerConfig.mtl_search_path = filename.string();

    tinyobj::ObjReader objReader;
    if (!objReader.ParseFromFile(filename.string())) {
        if (!objReader.Error().empty()) {
            throw std::runtime_error("Failed to open the obj file");
        }
        exit(1);
    }

    if (!objReader.Warning().empty()) 
        std::cout << "TinyObjLoader : " << objReader.Warning() << std::endl;
    
    const tinyobj::attrib_t& attrib = objReader.GetAttrib();
    const std::vector<tinyobj::shape_t>& shapes = objReader.GetShapes();
    //const std::vector<tinyobj::material_t>& materials = objReader.GetMaterials();
    std::unordered_map<Vertex, int> verticies_map;
    for(const tinyobj::shape_t& shape : shapes) {
        size_t offset = 0;
            
        for (size_t i = 0; i < shape.mesh.num_face_vertices.size(); i++) {
            size_t vertexNumberOnFace = shape.mesh.num_face_vertices[i];

            for (size_t j = 0; j < vertexNumberOnFace; j++) {
                tinyobj::index_t vertexIndex = shape.mesh.indices[offset + j];
                
                glm::vec3 position;
                position.x = attrib.vertices[3 * vertexIndex.vertex_index];
                position.y = attrib.vertices[3 * vertexIndex.vertex_index + 1];
                position.z = attrib.vertices[3 * vertexIndex.vertex_index + 2];
                position = glm::rotate(glm::mat4(1), glm::radians(-90.f), glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1), glm::radians(-90.f), glm::vec3(1, 0, 0)) * glm::vec4(position, 1);
                
                glm::vec3 normal;
                normal.x = attrib.normals[3 * vertexIndex.normal_index];
                normal.y = attrib.normals[3 * vertexIndex.normal_index + 1];
                normal.z = attrib.normals[3 * vertexIndex.normal_index + 2];
                
                glm::vec2 texCoord;
                texCoord.x = attrib.texcoords[2 * vertexIndex.texcoord_index];
                texCoord.y = attrib.texcoords[2 * vertexIndex.texcoord_index + 1];
                
                Vertex vertex{position, {1, 1, 1}, normal, texCoord};

                if(verticies_map.count(vertex) == 0) {
                    verticies_map[vertex] = verticies_map.size();
                    vertices.push_back(vertex);
                }
                indices.push_back(verticies_map[vertex]);
            }
            offset += vertexNumberOnFace;
        }
    }
}

const std::vector<Vertex>& MeshObject::getVertices() {
    return vertices;
}

const std::vector<int>& MeshObject::getIndices() {
    return indices;
}

const glm::mat4& MeshObject::getModel() {
    return model;
}