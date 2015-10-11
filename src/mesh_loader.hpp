// mesh_loader.h
// Header file for *.obj file importing
// and mesh object creation
// Trevor Westphal
#ifndef MeshLoader_h
#define MeshLoader_h

#include "includes/gl.hpp"
#include "includes/glm.hpp"

#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <map>

#include "pugixml.hpp" // PUGI xml library

#include "debug.hpp"
#include "vertex.hpp"
#include "game_clock.hpp"

using namespace std;

class MeshLoader{
public:
    MeshLoader(string filename);

    std::vector<GLfloat> getVertexArray() {return final_vertices;}
    std::vector<GLuint>  getFaceArray() {return final_faces;}


private:
    void loadMeshFromDAE(string filename);
    void writeFinalArrays(std::vector<Vertex>&, std::vector<GLuint>&);
    void calculateTangentsAndBinormals(std::vector<Vertex>&, std::vector<GLuint>&);
    bool getVerticesAndElements(pugi::xml_node, std::vector<Vertex>&, std::vector<GLuint>&);

    std::vector<GLfloat> final_vertices;
    std::vector<GLuint> final_faces;

    std::vector<Vertex> unique_vertices;
    std::map<int, int> vertex_to_unique;

    string filename;

    bool flat_shading;
};

#endif
