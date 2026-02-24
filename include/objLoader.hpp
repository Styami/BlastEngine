#ifndef OBJLOADER_HPP
#define OBJLOADER_HPP
#include "materialObject.hpp"
#include "vertex.hpp"
#include <cstddef>
#include <filesystem>
#include <unordered_map>
#include <utility>
#include <vector>

class ObjLoader {
    public:
        ObjLoader() = delete;
        ObjLoader(const std::filesystem::path& path);
        const std::vector<std::filesystem::path>& getTexturePath();
        const std::vector<MaterialObject>& getMaterials();
        const std::vector<Vertex>& getVertices();
        const std::vector<int>& getIndices();
    private:
        std::filesystem::path correctPathFormat(const std::string& entryPath);
        int checkAndGetIndexTexture(std::unordered_map<std::filesystem::path, size_t>& uniqueTexturesNames, const std::string& texturePath);
        std::vector<std::filesystem::path> m_texturesPath;
        std::vector<int> m_vertexIndices;
        std::vector<int> m_materialsIndicies;
        std::vector<Vertex> m_verticies;
        std::vector<MaterialObject> m_materials;
};

#endif
