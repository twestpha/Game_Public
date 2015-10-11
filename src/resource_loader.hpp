// Factory class for creating any type of resource and ensuring that there is a place for them to be held in memory

#ifndef ResourceLoader_h
#define ResourceLoader_h

#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

#include "mesh.hpp"
#include "texture.hpp"
#include "shader.hpp"

using namespace std;

class ResourceLoader {
public:
    ResourceLoader();

    Mesh& loadMesh(string filename);
    Texture& loadTexture(string filename);
    Shader& loadShader(string vs_filename, string fs_filename);

    void setMeshPath(string mesh_path);
    void setTexturePath(string texture_path);
    void setShaderPath(string shader_path);

    string getMeshPath();
    string getTexturePath();
    string getShaderPath();

    string asJsonString();

private:

    template <class type> bool hasKey(string id);

    unordered_map<string, Mesh> meshes;
    unordered_map<string, Texture> textures;
    unordered_map<string, Shader> shaders;

    string mesh_path;
    string texture_path;
    string shader_path;

};

#endif
