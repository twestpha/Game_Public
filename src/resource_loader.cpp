#include "resource_loader.hpp"

ResourceLoader::ResourceLoader() : mesh_path(""), texture_path(""), shader_path("") {

}

Mesh& ResourceLoader::loadMesh(string filename) {
    string mesh_id = mesh_path + filename;

    bool has_key = hasKey<Mesh>(mesh_id);
    if (!has_key){
        meshes[mesh_id] = Mesh(mesh_id);
    }
    return meshes[mesh_id];
}

Texture& ResourceLoader::loadTexture(string filename) {
    string texture_id = texture_path + filename;

    bool has_key = hasKey<Texture>(texture_id);
    if (!has_key){
        textures[texture_id] = Texture(texture_id);
    }

    return textures[texture_id];
}

Shader& ResourceLoader::loadShader(string vs_filename, string fs_filename) {
    string shader_id = shader_path + vs_filename + fs_filename;

    bool has_key = hasKey<Shader>(shader_id);
    if (!has_key){
        shaders[shader_id] = Shader(vs_filename, fs_filename);
    }
    return shaders[shader_id];

}

template <class type> bool ResourceLoader::hasKey(string id) {
    bool is_mesh = is_same<type, Mesh>::value;
    bool is_texture = is_same<type, Texture>::value;
    bool is_shader = is_same<type, Shader>::value;

    bool has_key = false;
    if (is_mesh){
        unordered_map<string, Mesh>::const_iterator key_iter = meshes.find(id);
        has_key = key_iter != meshes.end();
    } else if (is_texture){
        unordered_map<string, Texture>::const_iterator key_iter = textures.find(id);
        has_key = key_iter != textures.end();
    } else if (is_shader){
        unordered_map<string, Shader>::const_iterator key_iter = shaders.find(id);
        has_key = key_iter != shaders.end();
    }

    return has_key;

}

void ResourceLoader::setMeshPath(string mesh_path) {
    this->mesh_path = mesh_path;
}

void ResourceLoader::setTexturePath(string texture_path) {
    this->texture_path = texture_path;
}

void ResourceLoader::setShaderPath(string shader_path) {
    this->shader_path = shader_path;
}

string ResourceLoader::getMeshPath() {
        return mesh_path;
}

string ResourceLoader::getTexturePath() {
        return texture_path;
}

string ResourceLoader::getShaderPath() {
        return shader_path;
}

string ResourceLoader::asJsonString() {
    // Example:
        // "mesh_path": "res/models/",
        // "texture_path": "res/textures/",
        // "shader_path": "shaders/",

    string json_string = "";

    json_string += "\"mesh_path\": \"" + mesh_path + "\",\n";
    json_string += "\"texture_path\": \"" + texture_path + "\",\n";
    json_string += "\"shader_path\": \"" + shader_path + "\"";

    return json_string;
}
