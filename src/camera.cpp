#include "camera.hpp"

Camera::Camera(const Json::Value& camera_json){
    float fov = camera_json["fov"].asFloat();

    glm::vec3 position;
    position.x = camera_json["position"]["x"].asFloat();
    position.y = camera_json["position"]["y"].asFloat();
    position.z = camera_json["position"]["z"].asFloat();

    glm::vec3 rotation;
    rotation.x = camera_json["rotation"]["x"].asFloat();
    rotation.y = camera_json["rotation"]["y"].asFloat();
    rotation.z = camera_json["rotation"]["z"].asFloat();

    float move_sensitivity = camera_json["move_sensitivity"].asFloat();
    float rotate_sensitivity = camera_json["rotate_sensitivity"].asFloat();

    initializer(position, rotation, move_sensitivity, rotate_sensitivity, fov);
}

Camera::Camera(glm::vec3 position, glm::vec3 rotation, float move_sensitivity, float rotate_sensitivity, float fov){

    initializer(position, rotation, move_sensitivity, rotate_sensitivity, fov);
}

void Camera::initializer(glm::vec3 position, glm::vec3 rotation, float move_sensitivity, float rotate_sensitivity, float fov){

    this->position = position;
    this->rotation = rotation;

    local_x = glm::vec3(1.0f, 0.0f, 0.0f);
    local_y = glm::vec3(0.0f, 1.0f, 0.0f);
    local_z = glm::vec3(0.0f, 0.0f, 1.0f);

    this->move_sensitivity = move_sensitivity;
    this->rotate_sensitivity = rotate_sensitivity;

    // Default projection matrix
    int width = Window::getInstance()->getWidth();
    int height = Window::getInstance()->getHeight();

    // Set intrinsic parameters
    this->fov = fov;
    aspect_ratio = (float)width / (float)height;
    near_clip = 0.1f;
    far_clip = 500.0f;
    notifyViewChanged();
    updateProjectionMatrix();

}

void Camera::setPosition(glm::vec3 position){
    this->position = position;
    notifyViewChanged();
}

void Camera::setRotation(glm::vec3 rotation){
    this->rotation = rotation;
    notifyViewChanged();
}

void Camera::setFOV(float fov){
    this->fov = fov;
    updateProjectionMatrix();
}

void Camera::zoomIn(float zoom_amt){
    fov += zoom_amt;
    if (fov > 180.0f){
        fov = 180.0f;
    }
    setFOV(fov);
}

void Camera::zoomOut(float zoom_amt){
    fov -= zoom_amt;
    if (fov < 0.0f){
        fov = 0.0f;
    }
    setFOV(fov);
}

void Camera::moveX(int direction){
    position += move_sensitivity * direction * local_x * GameClock::getInstance()->getDeltaTime();
    notifyViewChanged();
}

void Camera::moveY(int direction){
    position += move_sensitivity * direction * local_y * GameClock::getInstance()->getDeltaTime();
    notifyViewChanged();
}

void Camera::moveZ(int direction){
    position += move_sensitivity * direction * local_z * GameClock::getInstance()->getDeltaTime();
    notifyViewChanged();
}

void Camera::moveGlobalX(int direction){
    position.x += move_sensitivity * direction * GameClock::getInstance()->getDeltaTime();
    notifyViewChanged();
}

void Camera::moveGlobalY(int direction){
    position.y += move_sensitivity * direction * GameClock::getInstance()->getDeltaTime();
    notifyViewChanged();
}

void Camera::moveGlobalZ(int direction){
    position.z += move_sensitivity * direction * GameClock::getInstance()->getDeltaTime();
    notifyViewChanged();
}

void Camera::rotateX(int direction){
    rotation.x += rotate_sensitivity * direction * GameClock::getInstance()->getDeltaTime();
    notifyViewChanged();
}

void Camera::rotateY(int direction){
    rotation.y += rotate_sensitivity * direction * GameClock::getInstance()->getDeltaTime();
    notifyViewChanged();
}

void Camera::rotateZ(int direction){
    rotation.z += rotate_sensitivity * direction * GameClock::getInstance()->getDeltaTime();
    notifyViewChanged();
}

string Camera::asJsonString() {
    // Returns the json formatted string of this camera
    // Example:
    //      "camera": {
    //          "fov": 45.0,
    //          "position": {
    //              "x": 0.0,
    //              "y": 40.0,
    //              "z": 40.0
    //          },
    //          "rotation": {
    //              "x": 1.04,
    //              "y": 0.0,
    //              "z": 0.0
    //          },
    //          "move_sensitivity": 10.0,
    //          "rotate_sensitivity": 2.0
    //      }

    string json_string = "\"camera\": {\n";

    json_string += "\"fov\": " + to_string(fov) + ",\n";
    json_string += GLMHelpers::vec3AsJsonString(position, "position") + ",\n";
    json_string += GLMHelpers::vec3AsJsonString(rotation, "rotation") + ",\n";
    json_string += "\"move_sensitivity\": " + to_string(move_sensitivity) + ",\n";
    json_string += "\"rotate_sensitivity\": " + to_string(rotate_sensitivity) + "\n";
    json_string += "}";

    return json_string;
}

glm::mat4 Camera::getViewMatrix(){
    if (view_changed_since_last_calculation){
        // Original vectors
        // eye      <x, y, z>
        // center   <x, y, z> - <0, 0, 1>
        // up       <0, 1, 0>
        glm::vec3 eye    = position;
        glm::vec3 center = position - glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 up     = local_y;

        float cx = cos(rotation.x);
        float sx = sin(rotation.x);

        float cy = cos(rotation.y);
        float sy = sin(rotation.y);

        glm::mat3 rotation_x = glm::mat3(  1 ,  0 ,  0 ,
                                           0 ,  cx, -sx,
                                           0 ,  sx,  cx  );

        glm::mat3 rotation_y = glm::mat3(  cy,  0 , -sy,
                                           0 ,  1 ,  0 ,
                                           sy,  0 ,  cy  );

        glm::mat3 rotation_matrix = rotation_y * rotation_x;

        // Transform the center vector
        center = position - (rotation_matrix * glm::vec3(0.0f, 0.0f, 1.0f));

        // Transform the camera axes
        local_x = rotation_matrix * glm::vec3(1.0f, 0.0f, 0.0f);
        local_y = rotation_matrix * glm::vec3(0.0f, 1.0f, 0.0f);
        local_z = rotation_matrix * glm::vec3(0.0f, 0.0f, 1.0f);

        view_matrix = glm::lookAt(eye, center, up);

        view_changed_since_last_calculation = false;
    }

    return view_matrix;
}

glm::mat4 Camera::getProjectionMatrix(){
    return proj_matrix;
}

glm::vec3 Camera::getPosition(){
    return position;
}

glm::vec3 Camera::getRotation(){
    return rotation;
}

float Camera::getFOV(){
    return fov;
}

void Camera::notifyViewChanged(){
    view_changed_since_last_calculation = true;
}

void Camera::updateProjectionMatrix(){
    // Use all of the intrinsic values to create the projection matrix
    proj_matrix = glm::perspective(fov, aspect_ratio, near_clip, far_clip);
}
