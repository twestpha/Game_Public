#include "flat_mesh.hpp"

FlatMesh* FlatMesh::instance;

FlatMesh* FlatMesh::getInstance(){
    if(instance){
        return instance;
    } else {
        instance = new FlatMesh();
        return instance;
    }
}

FlatMesh::FlatMesh(){
    // Create 2D mesh for framebuffer to draw onto.
    GLfloat planeVerts[] = {
             -1.0f,  1.0f,  0.0f, 1.0f,
              1.0f,  1.0f,  1.0f, 1.0f,
              1.0f, -1.0f,  1.0f, 0.0f,

              1.0f, -1.0f,  1.0f, 0.0f,
             -1.0f, -1.0f,  0.0f, 0.0f,
             -1.0f,  1.0f,  0.0f, 1.0f
    };

    std::vector<GLfloat> vertices(planeVerts, planeVerts + sizeof(planeVerts) / sizeof(GLfloat));

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

}

void FlatMesh::draw(){
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void FlatMesh::drawOutline(){
    glDrawArrays(GL_LINE_LOOP, 0, 6);
}

void FlatMesh::bindVAO(){
    glBindVertexArray(vao);
}

void FlatMesh::attachGeometryToShader(Shader shader){
    bool already_bound = std::find(bound_shaders.begin(), bound_shaders.end(), &shader) != bound_shaders.end();

    if(!already_bound){
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        GLint posAttrib = glGetAttribLocation(shader.getGLId(), "position");
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
                               4*sizeof(float), 0);

        GLint texAttrib = glGetAttribLocation(shader.getGLId(), "texcoord");
        glEnableVertexAttribArray(texAttrib);
        glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
                               4*sizeof(float), (void*)(2*sizeof(float)));

        bound_shaders.push_back(&shader);

    }

}
