#ifndef Camera_h
#define Camera_h

#include "includes/glm.hpp"

#include <stdio.h>

#include "game_clock.hpp"
#include "window.hpp"
#include "includes/json.hpp"
#include "glm_helpers.hpp"

const float MOVE_SENSITIVITY = 10.0;
const float ROTATE_SENSITIVITY = 2.0;

using namespace std;

class Camera {
public:
    Camera() : Camera(glm::vec3(), glm::vec3(), MOVE_SENSITIVITY, ROTATE_SENSITIVITY) {;}
    Camera(const Json::Value& camera_json);
    Camera(glm::vec3 p) : Camera(p, glm::vec3(), MOVE_SENSITIVITY, ROTATE_SENSITIVITY) {;}
    Camera(glm::vec3 p, glm::vec3 r) : Camera(p, r, MOVE_SENSITIVITY, ROTATE_SENSITIVITY) {;}
    Camera(glm::vec3 p, glm::vec3 r, float ms, float rs) : Camera(p, r, ms, rs, 45.0f) {;}
    Camera(glm::vec3, glm::vec3, float, float, float);

    void moveX(int);
    void moveY(int);
    void moveZ(int);

    void rotateX(int);
    void rotateY(int);
    void rotateZ(int);

    void moveGlobalX(int);
    void moveGlobalY(int);
    void moveGlobalZ(int);

    void setPosition(glm::vec3);
    void setRotation(glm::vec3);

    void print();

    void setFOV(float fov);

    void zoomIn(float zoom_amt);
    void zoomOut(float zoom_amt);

    string asJsonString();

    glm::vec3 getPosition();
    glm::vec3 getRotation();
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
    float getFOV();

private:
    void initializer(glm::vec3, glm::vec3, float, float, float);
    void updateProjectionMatrix();
    void notifyViewChanged();

    glm::vec3 position;
    glm::vec3 rotation;

    glm::vec3 local_x;
    glm::vec3 local_y;
    glm::vec3 local_z;

    float move_sensitivity;
    float rotate_sensitivity;

    // Proj matrix and intrinsic values
    float fov;
    float aspect_ratio;
    float near_clip;
    float far_clip;
    glm::mat4 proj_matrix;
    glm::mat4 view_matrix;

    bool view_changed_since_last_calculation;

};


#endif
