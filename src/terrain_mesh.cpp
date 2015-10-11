#include "terrain_mesh.hpp"

// The single vertex specification is:
//      x, y, z, nx, ny, nz, tx, ty, tz, bx, by, bz, u, v, su, sv
// Where:
//      x = x position of vertex
//      y = y position of vertex
//      z = z position of vertex
//      nx = x component of the normal vector
//      ny = y component of the normal vector
//      nz = z component of the normal vector
//      tx = x component of the tangent vector
//      ty = y component of the tangent vector
//      tz = z component of the tangent vector
//      bx = x component of the binormal vector
//      by = y component of the binormal vector
//      bz = z component of the binormal vector
//      u = U texture coordinate on UV map
//      v = V texture coordinate on UV map
//      su = U texture coordinate on splat map
//      sv = V texture coordinate on splat map

TerrainMesh::TerrainMesh(std::vector<TerrainVertex> vertices, std::vector<GLuint> elements){
    // This constructor loads geometry data (vertices and faces) from std::vectors.
    std::vector<GLfloat> out_vertices;
    for (int i = 0; i < vertices.size(); ++i){
        TerrainVertex vertex = vertices[i];
        out_vertices.push_back(vertex.position.x);
        out_vertices.push_back(vertex.position.y);
        out_vertices.push_back(vertex.position.z);
        out_vertices.push_back(vertex.normal.x);
        out_vertices.push_back(vertex.normal.y);
        out_vertices.push_back(vertex.normal.z);
        out_vertices.push_back(vertex.tangent.x);
        out_vertices.push_back(vertex.tangent.y);
        out_vertices.push_back(vertex.tangent.z);
        out_vertices.push_back(vertex.binormal.x);
        out_vertices.push_back(vertex.binormal.y);
        out_vertices.push_back(vertex.binormal.z);
        out_vertices.push_back(vertex.texcoord.x);
        out_vertices.push_back(vertex.texcoord.y);
        out_vertices.push_back(vertex.splatcoord.x);
        out_vertices.push_back(vertex.splatcoord.y);
    }

    TerrainMesh::loadMeshData(out_vertices, elements);
}

void TerrainMesh::attachGeometryToShader(Shader& shader){
    // As soon as you've bound a certain VAO, every time you call glVertexAttribPointer,
    // that information will be stored in that VAO. This makes switching between different vertex data
    // and vertex formats as easy as binding a different VAO.
    glUseProgram(shader.getGLId());

    // Because binding the geometry to the shader multiple times on the same vao causes problems, we
    // check we have already bound the data.
    bool already_bound = std::find(bound_shaders.begin(), bound_shaders.end(), &shader) != bound_shaders.end();

    // If this shader does not already have the data, bind it.
    if(!already_bound){
        glBindVertexArray(vao);
        // Bind the VBO to ensure the correct vertex data gets pushed to the shader.
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // Get the reference to the "position" attribute defined in
        // the vertex shader
        GLint posAttrib = glGetAttribLocation(shader.getGLId(), "position");
        glEnableVertexAttribArray(posAttrib);
        // Load the position attributes from our vertex spec with width 8. The position
        // values start at index 0. Tell it to load 3 values.
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 16*sizeof(float), 0);

        GLint normalAttrib = glGetAttribLocation(shader.getGLId(), "normal");
        glEnableVertexAttribArray(normalAttrib);
        // Load the normal pointer from our vertex spec with width 8. The normal values
        // start at index 2. Tell it to load 3 values.
        glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 16*sizeof(float),
            (void*)(3*sizeof(float)));

        GLint tangent_attrib = glGetAttribLocation(shader.getGLId(), "tangent");
        glEnableVertexAttribArray(tangent_attrib);
        glVertexAttribPointer(tangent_attrib, 3, GL_FLOAT, GL_FALSE, 16*sizeof(float),
            (void*)(6*sizeof(float)));

        GLint bitangent_attrib = glGetAttribLocation(shader.getGLId(), "bitangent");
        glEnableVertexAttribArray(bitangent_attrib);
        glVertexAttribPointer(bitangent_attrib, 3, GL_FLOAT, GL_FALSE, 16*sizeof(float),
            (void*)(9*sizeof(float)));

        // Link the texture coordinates to the shader.
        GLint texAttrib = glGetAttribLocation(shader.getGLId(), "texcoord");
        glEnableVertexAttribArray(texAttrib);
        // Load the texture attributes from our vertex spec with width 8. The texture
        // values start at index 8. Tell it to load 2 values.
        glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 16*sizeof(float),
            (void*)(12*sizeof(float)));

        // Link the texture coordinates to the shader.
        GLint splatAttrib = glGetAttribLocation(shader.getGLId(), "splatcoord");
        glEnableVertexAttribArray(splatAttrib);
        // Load the texture attributes from our vertex spec with width 8. The texture
        // values start at index 8. Tell it to load 2 values.
        glVertexAttribPointer(splatAttrib, 2, GL_FLOAT, GL_FALSE, 16*sizeof(float),
            (void*)(14*sizeof(float)));

        // Keep track of which shaders we have already bound data to
        bound_shaders.push_back(&shader);

    }

}
