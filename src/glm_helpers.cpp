#include "glm_helpers.hpp"

string GLMHelpers::vec3AsJsonString(glm::vec3 vector, string name) {
    // Returns a glm vec3 as a json formatted string
    // Example:
    //      "position": {
    //          "x": 0.0,
    //          "y": 0.0,
    //          "z": 0.0
    //      },
    string json_string = "\"" + name + "\": {\n";

    json_string += "\"x\": " + to_string(vector.x) + ",\n";
    json_string += "\"y\": " + to_string(vector.y) + ",\n";
    json_string += "\"z\": " + to_string(vector.z) + "\n";

    json_string += "}";

    return json_string;
}

glm::vec2 GLMHelpers::calculateScreenPosition(glm::mat4 proj, glm::mat4 view, glm::vec3 world_point) {

    glm::vec4 position4(world_point.x, world_point.y, world_point.z, 1.0);
    glm::vec4 view_space = view * position4;
    view_space = glm::vec4(view_space.x, view_space.y, view_space.z, 1);

    glm::vec4 homogeneous = proj * view_space;
    glm::vec3 normalized = glm::vec3(homogeneous.x, homogeneous.y, homogeneous.z) / homogeneous.w;

    return glm::vec2(normalized.x, normalized.y);
}
