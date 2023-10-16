#ifndef MATERIAL_H
#define MATERIAL_H


#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <string>
#include <vector>
using namespace std;
using namespace glm;

struct Material_encoded {
    vec3 emissive = vec3(0, 0, 0);  // 作为光源时的发光颜色
    vec3 baseColor = vec3(0.2, 0.2, 0.2);

    vec3 param1;
    vec3 param2;
    vec3 param3;
    vec3 param4;

};

class Material {
public:
    vec3 emissive = vec3(0, 0, 0);  // 作为光源时的发光颜色
    vec3 baseColor = vec3(0.2, 0.2, 0.5);
    float subsurface = 0.0;
    float metallic = 0.0;
    float specular = 0.0;

    float specularTint = 0.0;
    float roughness = 0.0;
    float anisotropic = 0.0;

    float sheen = 0.0;
    float sheenTint = 0.0;
    float clearcoat = 0.0;

    float clearcoatGloss = 0.0;
    float IOR = 1.0;
    float transmission = 0.0;

    Material_encoded encoded() {
        Material_encoded m;
        m.emissive = this->emissive;
        m.baseColor = this->baseColor;
        //m.baseColor = vec3(0.2, 0.2, 0.5);

        m.param1 = vec3(this->subsurface, this->metallic, this->specular);
        m.param2 = vec3(this->specularTint, this->roughness, this->anisotropic);
        m.param3 = vec3(this->sheen, this->sheenTint, this->clearcoat);
        m.param4 = vec3(this->clearcoatGloss, this->IOR, this->transmission);
        return m;
    }
};


#endif // !MATERIAL_H
