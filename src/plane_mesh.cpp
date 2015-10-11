#include "plane_mesh.hpp"

PlaneMesh::PlaneMesh() : Mesh() {
    GLfloat plane_verts[] = {-0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                             0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                             0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,};

    GLuint  plane_faces[] = {2, 1, 0, 1, 2, 3};

    std::vector<GLfloat> plane_verts_vector(plane_verts, plane_verts + sizeof(plane_verts) / sizeof(GLfloat));
    std::vector<GLuint> plane_faces_vector(plane_faces, plane_faces + sizeof(plane_faces) / sizeof(GLuint));

    this->loadMeshData(plane_verts_vector, plane_faces_vector);
}
