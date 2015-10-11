#ifndef Doodad_h
#define Doodad_h

#include <iostream>

#include "drawable.hpp"
#include "includes/json.hpp"
#include "shader.hpp"
#include "resource_loader.hpp"
#include "glm_helpers.hpp"

using namespace std;

class Doodad: public Drawable {
public:
    Doodad(const Json::Value& doodad_json, ResourceLoader& resource_loader);
    Doodad(Mesh&, ResourceLoader& resource_loader);
    Doodad(Mesh&, Shader& shader);
    Doodad(Mesh&, Shader& shader, glm::vec3, GLfloat);
    Drawable* clone();

    string asJsonString();

private:
    void updateUniformData();

};

#endif
