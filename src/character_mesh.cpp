#include "character_mesh.hpp"

CharacterMesh* CharacterMesh::instance;

CharacterMesh* CharacterMesh::getInstance(){
    if(instance){
        return instance;
    } else {
        instance = new CharacterMesh();
        return instance;
    }
}

CharacterMesh::CharacterMesh(){
    // Create 2D mesh for framebuffer to draw onto.
    float delta_u = 1.0 / FontSheet::NUM_CHARS;
    float delta_v = 1.0;

    GLfloat planeVerts[] = {
             -1.0f,  1.0f,  0.0f, 0.0f,
              1.0f,  1.0f,  delta_u, 0.0f,
              1.0f, -1.0f,  delta_u, delta_v,

              1.0f, -1.0f,  delta_u, delta_v,
             -1.0f, -1.0f,  0.0f, delta_v,
             -1.0f,  1.0f,  0.0f, 0.0f,
    };

    std::vector<GLfloat> vertices(planeVerts, planeVerts + sizeof(planeVerts) / sizeof(GLfloat));

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
}
