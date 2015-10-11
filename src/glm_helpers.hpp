#ifndef GLMHelpers_h
#define GLMHelpers_h

#include <string>

#include "includes/glm.hpp"

using namespace std;

namespace GLMHelpers {
    string vec3AsJsonString(glm::vec3 vector, string name);
    glm::vec2 calculateScreenPosition(glm::mat4 proj, glm::mat4 view, glm::vec3 world_point);
}


#endif
