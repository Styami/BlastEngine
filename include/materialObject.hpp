#ifndef MATERIALOBJECT_HPP
#define MATERIALOBJECT_HPP

#include <glm/glm.hpp>

struct MaterialObject {
    float Ns;
    float Ni;
    float d;
    int illum;
    glm::vec4 Tf;
    glm::vec4 Ka;
    glm::vec4 Kd;
    glm::vec4 Ks;
    glm::vec4 Ke;
    int indexAmbiantMap;
    int indexDiffuseMap;
    int indexSpecularMap;
    int indexBumpMap;
};

#endif