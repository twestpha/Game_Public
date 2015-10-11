#ifndef FlatMesh_h
#define FlatMesh_h

#include "includes/gl.hpp"

#include <cstdlib>
#include <vector>
#include <algorithm>

#include "mesh_loader.hpp"
#include "shader.hpp"

class FlatMesh {
public:
    static FlatMesh* getInstance();

    void draw();
    void drawOutline();
    void bindVAO();
    void attachGeometryToShader(Shader);

protected:
    GLuint vao;
    GLuint vbo;

    std::vector<Shader*> bound_shaders;

    FlatMesh();
    static FlatMesh* instance;

};

#endif
