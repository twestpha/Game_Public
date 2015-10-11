#include "doodad.hpp"

Doodad::Doodad(const Json::Value& doodad_json, ResourceLoader& resource_loader){

    string mesh_path = resource_loader.getMeshPath();
    string texture_path = resource_loader.getTexturePath();

    std::string mesh_filename = doodad_json["mesh"].asString();
    Mesh& mesh_ref = resource_loader.loadMesh(mesh_filename);

    // Scale of the doodad
    float scale = doodad_json["scale"].asFloat();

    // World position of the doodad
    glm::vec3 position;
    position.x = doodad_json["position"]["x"].asFloat();
    position.y = doodad_json["position"]["y"].asFloat();
    position.z = doodad_json["position"]["z"].asFloat();
    // Euler rotation of the doodad
    glm::vec3 rotation;
    rotation.x = doodad_json["rotation"]["x"].asFloat();
    rotation.y = doodad_json["rotation"]["y"].asFloat();
    rotation.z = doodad_json["rotation"]["z"].asFloat();

    Shader& shader_ref = resource_loader.loadShader("shaders/doodad.vs", "shaders/doodad.fs");
    load(mesh_ref, shader_ref, position, scale);
    setRotationEuler(rotation);

    // Load the textures
    const Json::Value textures = doodad_json["textures"];
    std::string diffuse_filename = textures["diff"].asString();
    std::string normal_filename = textures["norm"].asString();
    std::string specular_filename = textures["spec"].asString();
    std::string emissive_filename = textures["emit"].asString();

    if (diffuse_filename != ""){
        diffuse_filename = texture_path + diffuse_filename;
        Texture diff(diffuse_filename);
        setDiffuse(diff);
    }
    if (normal_filename != ""){
        normal_filename = texture_path + normal_filename;
        Texture norm(normal_filename);
        setNormal(norm);
    }
    if (specular_filename != ""){
        specular_filename = texture_path + specular_filename;
        Texture spec(specular_filename);
        setSpecular(spec);
    }
    if (emissive_filename != ""){
        emissive_filename = texture_path + emissive_filename;
        Texture emit(emissive_filename);
        setEmissive(emit);
    }

}

Doodad::Doodad(Mesh& mesh, ResourceLoader& resource_loader) {
    Shader& shader_ref = resource_loader.loadShader("shaders/doodad.vs", "shaders/doodad.fs");
    load(mesh, shader_ref, glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
}

Doodad::Doodad(Mesh& mesh, Shader& shader) : Drawable(mesh, shader){

}

Doodad::Doodad(Mesh& mesh, Shader& shader, glm::vec3 position, GLfloat scale):
    Drawable(mesh, shader, position, scale) {
}

Drawable* Doodad::clone() {
    return new Doodad(*this);
}

string Doodad::asJsonString() {
    // Returns this dooad as a json string
    // A single doodad is represented as (example):
    // {
    //     "mesh": "fence.dae",
    //     "scale": 1.0,
    //     "position": {
    //         "x": 0.0,
    //         "y": 0.0,
    //         "z": 0.0
    //     },
    //     "rotation": {
    //         "x": 1.5707,
    //         "y": 0.0,
    //         "z": 0.0
    //     },
    //     "textures": {
    //         "diff": "fence_diff.png",
    //         "norm": "fence_norm.png",
    //         "spec": "fence_spec.png",
    //         "emit": "fence_emit.png"
    //     }
    // },
    string json_string = "{\n";

    // Mesh
    json_string += mesh->asJsonString() + ",\n";

    // Scale, position and rotation (should be moved to game map eventually)
    json_string += "\"scale\": " + to_string(scale) + ",\n";
    json_string += GLMHelpers::vec3AsJsonString(position, "position") + ",\n";
    json_string += GLMHelpers::vec3AsJsonString(rotation, "rotation") + ",\n";

    // Textures (grrrrrrr json spec sucks)
    json_string += "\"textures\": {\n";

    string diffuse_str = diffuse.asJsonString("diff");
    string specular_str = specular.asJsonString("spec");
    string normal_str = normal.asJsonString("norm");
    string emissive_str = emissive.asJsonString("emit");

    if (diffuse_str != ""){
        json_string += diffuse.asJsonString("diff") + ",\n";
    }
    if (specular_str != ""){
        json_string += specular.asJsonString("spec") + ",\n";
    }
    if (normal_str != ""){
        json_string += normal.asJsonString("norm") + ",\n";
    }
    if (emissive_str != ""){
        json_string += emissive.asJsonString("emit") + ",\n";
    }

    // Remove the last two characters because dumb comma issue
    json_string.pop_back();
    json_string.pop_back();
    json_string += "}\n";

    json_string += "}";
    return json_string;
}

void Doodad::updateUniformData(){
    // Set the scale, this is not really going to be a thing, probably
    // ^ It's definitely a thing
    glUniform1f(glGetUniformLocation(shader->getGLId(), "scale"), scale);

}
