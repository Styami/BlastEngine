#include "objLoader.hpp"
#include "materialObject.hpp"
#include "vertex.hpp"
#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

ObjLoader::ObjLoader(const std::filesystem::path& path) {
    tinyobj::ObjReaderConfig config;
    config.mtl_search_path = path.parent_path();

    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(path.string(), config)) 
        if (!reader.Error().empty())
            throw std::runtime_error(reader.Error());
    
    if (!reader.Warning().empty()) {
        std::cout << "TinyObjLoader : " << reader.Warning().c_str() << std::endl;
    }

    const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();
    const std::vector<tinyobj::material_t>& materials = reader.GetMaterials();
    const tinyobj::attrib_t& attrib = reader.GetAttrib();
    
    std::unordered_map<Vertex, size_t> uniqueVerticies;
    std::unordered_map<std::string, size_t> uniqueMaterials;
    std::unordered_map<std::filesystem::path, size_t> uniqueTexturesNames;

    for (const tinyobj::shape_t& shape : shapes) {
        int offsetVerticies = 0;
        for (size_t indexFace = 0; indexFace < shape.mesh.num_face_vertices.size(); indexFace ++) {
            size_t numVerticiesInFace = shape.mesh.num_face_vertices[indexFace];
            int materialIndex = shape.mesh.material_ids[indexFace];
            tinyobj::material_t material = materials[materialIndex];

            int indexMat = -1;
            if (uniqueMaterials.count(material.name) == 0) {
                int indexMapKa = -1, indexMapKs = -1, indexMapKd = -1, indexMapBump = -1;
                indexMapBump = checkAndGetIndexTexture(uniqueTexturesNames, material.bump_texname);
                indexMapKa = checkAndGetIndexTexture(uniqueTexturesNames, material.ambient_texname);
                indexMapKd = checkAndGetIndexTexture(uniqueTexturesNames, material.diffuse_texname);
                indexMapKs = checkAndGetIndexTexture(uniqueTexturesNames, material.specular_texname);

                MaterialObject mat = {
                                        .Ns = material.shininess,
                                        .Ni = material.ior,
                                        .d = material.dissolve,
                                        .illum = material.illum,
                                        .Tf = {material.transmittance[0], material.transmittance[1], material.transmittance[2], 0},
                                        .Ka = {material.ambient[0], material.ambient[1], material.ambient[2], 0},
                                        .Kd = {material.diffuse[0], material.diffuse[1], material.diffuse[2], 0},
                                        .Ks = {material.specular[0], material.specular[1], material.specular[2], 0},
                                        .Ke = {material.emission[0], material.emission[1], material.emission[2], 0},
                                        .indexAmbiantMap = indexMapKa,
                                        .indexDiffuseMap = indexMapKd,
                                        .indexSpecularMap = indexMapKs,
                                        .indexBumpMap = indexMapBump
                };
                uniqueMaterials[material.name] = uniqueMaterials.size();
                m_materials.push_back(mat);
            }

            indexMat = uniqueMaterials[material.name];
            for (size_t j = 0; j < numVerticiesInFace; j++) {
                tinyobj::index_t vertexIndex = shape.mesh.indices[offsetVerticies + j];

                glm::vec3 position;
                position.x = attrib.vertices[3 * vertexIndex.vertex_index]; 
                position.y = attrib.vertices[3 * vertexIndex.vertex_index + 1];
                position.z = attrib.vertices[3 * vertexIndex.vertex_index + 2];
            
                glm::vec3 normal;
                normal.x = attrib.vertices[3 * vertexIndex.normal_index]; 
                normal.y = attrib.vertices[3 * vertexIndex.normal_index + 1];
                normal.z = attrib.vertices[3 * vertexIndex.normal_index + 2];
    
                glm::vec2 texCoord;
                texCoord.x = attrib.texcoords[2 * vertexIndex.texcoord_index];
                texCoord.y = attrib.texcoords[2 * vertexIndex.texcoord_index + 1];

                glm::vec3 color;
                color.r = attrib.colors[3 * vertexIndex.vertex_index];
                color.g = attrib.colors[3 * vertexIndex.vertex_index + 1];
                color.b = attrib.colors[3 * vertexIndex.vertex_index + 2];

                Vertex v = {.pos = position, .color = color, .normal = normal, .texCoord = texCoord, .indexMat = indexMat};
                if (uniqueVerticies.count(v) == 0) {
                    uniqueVerticies[v] = uniqueVerticies.size();
                    m_verticies.push_back(v);
                }
                m_vertexIndices.push_back(uniqueVerticies[v]);
            }

            offsetVerticies += numVerticiesInFace;
        }

    }
}

std::filesystem::path ObjLoader::correctPathFormat(const std::string& entryPath) {
    std::filesystem::path finalPath;
    std::string acc;
    for (char c : entryPath) {
        if (c == '\\' || c == '/') {
            if (finalPath.empty()) {
                finalPath = acc;
            } else {
                finalPath /= acc;
            }
            acc.erase();
        } else {
            acc += c;
        }
    }
    return finalPath / acc;
}

int ObjLoader::checkAndGetIndexTexture(std::unordered_map<std::filesystem::path, size_t>& uniqueTexturesNames, const std::string& texturePath) {
    std::filesystem::path correctTexturePath = correctPathFormat(texturePath);
    int indexTexture = - 1;
    if (!texturePath.empty()) {
        if (uniqueTexturesNames.count(correctTexturePath) == 0) {
            indexTexture = m_texturesPath.size();
            uniqueTexturesNames[correctTexturePath] = indexTexture;
            m_texturesPath.push_back(correctTexturePath);
        }   
        indexTexture = uniqueTexturesNames[correctTexturePath];
    }
    return indexTexture;
}

const std::vector<std::filesystem::path>& ObjLoader::getTexturePath() {
    return m_texturesPath;
}

const std::vector<MaterialObject>& ObjLoader::getMaterials() {
    return m_materials;
}

const std::vector<Vertex>& ObjLoader::getVertices() {
    return m_verticies;
}

const std::vector<int>& ObjLoader::getIndices() {
    return m_vertexIndices;
}
