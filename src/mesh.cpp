// Mesh:
//      Mesh handles all of the lowest level OpenGL drawing. It is a representation of a VAO.
//      From a top level perspective the Mesh holds geometry data: vertices and faces. This data
//      can either be passed in as std::vectors in the constructor or loaded from a .obj file. The
//      VAO is not complete without having an associated shader. Before using the mesh you must specify
//      which shader you will be using so that it knows the geometry data. Ultimately when using this object,
//      you will get a VAO back. This VAO can then be bound such that you can attach uniform data and textures
//      to the shader at a higher level.

// The single vertex specification is:
//      x, y, z, nx, ny, nz, tx, ty, tz, bx, by, bz, u, v
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

#include "mesh.hpp"

Mesh::Mesh(string directory, string filename) : File(directory, filename) {
    #warning make sure all meshes have filenames even if they are not loaded from a file
    // This constructor loads geometry data (vertices and faces) from a .obj file.
    MeshLoader mesh_loader = MeshLoader(getFilepath());
    std::vector<GLfloat> vertices = mesh_loader.getVertexArray();
    std::vector<GLuint> elements  = mesh_loader.getFaceArray();

    loadMeshData(vertices, elements);
}

Mesh::Mesh(string filepath) : File(filepath) {
    MeshLoader mesh_loader = MeshLoader(filepath);
    std::vector<GLfloat> vertices = mesh_loader.getVertexArray();
    std::vector<GLuint> elements  = mesh_loader.getFaceArray();

    loadMeshData(vertices, elements);
}

Mesh::Mesh(std::vector<GLfloat> vertices, std::vector<GLuint> elements){
    // This constructor loads geometry data (vertices and faces) from std::vectors.
    loadMeshData(vertices, elements);
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> elements){
    // This constructor loads geometry data (vertices and faces) from std::vectors.
    std::vector<GLfloat> out_vertices;
    for (int i = 0; i < vertices.size(); ++i){
        Vertex vertex = vertices[i];
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
    }

    loadMeshData(out_vertices, elements);
}

void Mesh::loadMeshData(std::vector<GLfloat> vertices, std::vector<GLuint> elements){

    // We need to know how many faces to draw later on.
    num_faces = elements.size();

    // Create our Vertex Array Object (VAO) which will hold our vertex and element data.
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint ebo;

    // Store all of the vertex data in a Vertex Buffer Object (VBO)
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    // Store all of the face data in a Element Buffer Object (EBO)
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size()* sizeof(GLuint), elements.data(), GL_STATIC_DRAW);

}

void Mesh::bindVAO(){
    // This will bind the vertex array for you so that you can use the mesh at a higher level.
    glBindVertexArray(vao);
}
void Mesh::draw(){
    // Draws the actual geometry. Textures and everything else are attached at a higher level.
    glDrawElements(GL_TRIANGLES, this->num_faces, GL_UNSIGNED_INT, 0);

}

string Mesh::asJsonString() {
    // Returns the json formatted string of the mesh.
    // A single mesh is represented as (example):
    //      "mesh": "fence.dae",
    string json_string = "\"mesh\": ";
    json_string += "\"" + getFilename() + "\"";

    // If the filename is blank then this is not a 'saveable' mesh
    if (isBlank()) {
        json_string = "";
    }

    return json_string;
}

void Mesh::attachGeometryToShader(Shader& shader){
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
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
                               14*sizeof(float), 0);

        GLint normalAttrib = glGetAttribLocation(shader.getGLId(), "normal");
        glEnableVertexAttribArray(normalAttrib);
        // Load the normal pointer from our vertex spec with width 8. The normal values
        // start at index 2. Tell it to load 3 values.
        glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE,
                               14*sizeof(float), (void*)(3*sizeof(float)));

        GLint tangent_attrib = glGetAttribLocation(shader.getGLId(), "tangent");
        glEnableVertexAttribArray(tangent_attrib);
        glVertexAttribPointer(tangent_attrib, 3, GL_FLOAT, GL_FALSE,
        14*sizeof(float), (void*)(6*sizeof(float)));

        GLint bitangent_attrib = glGetAttribLocation(shader.getGLId(), "bitangent");
        glEnableVertexAttribArray(bitangent_attrib);
        glVertexAttribPointer(bitangent_attrib, 3, GL_FLOAT, GL_FALSE,
        14*sizeof(float), (void*)(9*sizeof(float)));

        // Link the texture coordinates to the shader.
        GLint texAttrib = glGetAttribLocation(shader.getGLId(), "texcoord");
        glEnableVertexAttribArray(texAttrib);
        // Load the texture attributes from our vertex spec with width 8. The texture
        // values start at index 8. Tell it to load 2 values.
        glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
                               14*sizeof(float), (void*)(12*sizeof(float)));

        // Keep track of which shaders we have already bound data to
        bound_shaders.push_back(&shader);


    }

}
