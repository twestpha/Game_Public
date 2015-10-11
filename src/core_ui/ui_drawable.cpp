#include "ui_drawable.hpp"

UIDrawable::UIDrawable() {
    Texture texture(glm::vec4(1.0, 0.0, 1.0, 1.0));
    load(texture, Regular);
}

UIDrawable::UIDrawable(Texture texture) : FlatDrawable(){
    load(texture, Regular);
}

UIDrawable::UIDrawable(Texture texture, PositioningMode mode) : FlatDrawable(){
    load(texture, mode);
}

UIDrawable::UIDrawable(Shader shader, Texture texture) : FlatDrawable(shader.getGLId()){
    load(texture, Regular);
}

void UIDrawable::load(Texture texture, PositioningMode mode){
    window_width = Window::getInstance()->getWidth();
    window_height = Window::getInstance()->getHeight();

    glm::mat3 mesh_projection = glm::mat3(  window_width, 0.0f  , 0.0f,
                                            0.0f , window_height, 0.0f,
                                            0.0f , 0.0f  , 1.0f );

    inv_mesh_projection = glm::transpose(mesh_projection);

    attachTexture(texture);
    setPixelCoordinates(0, 0, texture.getWidth(), texture.getHeight());

    parent = NULL;

    pos_mode = mode;

}

void UIDrawable::attachTexture(Texture texture){
    glBindTexture(GL_TEXTURE_2D, texture.getGLId());
    int miplevel = 0;
    int w, h;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &h);

    glm::vec3 image_size = glm::vec3(w, h, 1.0);
    glm::vec3 gl_mesh_size = image_size * inv_mesh_projection;

    width = gl_mesh_size.x;
    height = gl_mesh_size.y;

    FlatDrawable::attachTexture(texture);

    updateDimensions();
}

glm::vec2 UIDrawable::getGLPosition(){
    return glm::vec2( 2.0f*(float(x_pixels)/float(window_width))-1.0f, -2.0f*(float(y_pixels)/float(window_height))+1.0f);
}

void UIDrawable::setGLPosition(glm::vec2 position){
    glm::vec2 new_pos;
    if (pos_mode == Regular) {
        new_pos = glm::vec2(position.x + width, position.y - height);
    } else if (pos_mode == CenterHorizontal) {
        new_pos = glm::vec2(position.x, position.y - height);
    } else if (pos_mode == CenterVertical) {
        new_pos = glm::vec2(position.x + width, position.y);
    } else if (pos_mode == Center) {
        new_pos = glm::vec2(position.x, position.y);
    }
    this->position = new_pos;
}

void UIDrawable::draw(){
    FlatDrawable::draw();

    if (outline){
        glUniform1i(glGetUniformLocation(shader.getGLId(), "is_outline"), true);
        mesh->drawOutline();
        glUniform1i(glGetUniformLocation(shader.getGLId(), "is_outline"), false);
    }
}

void UIDrawable::setGLCoordinates(glm::vec2 start, glm::vec2 end){
    this->width = (end.x - start.x)/2;
    this->height = (end.y - start.y)/2;
    this->position = glm::vec2(start.x + width, start.y + height);
}

void UIDrawable::setPositioningMode(PositioningMode mode) {
    pos_mode = mode;
}

void UIDrawable::updateDimensions(){
    // Use me for things that scale
    width = (float(width_pixels)/float(window_width));
    height = (float(height_pixels)/float(window_height));
}

void UIDrawable::parseConstraints(pugi::xml_node constraints_node){

    bool has_x_position = constraints_node.child("x");
    bool has_y_position = constraints_node.child("y");
    bool has_width = constraints_node.child("width");
    bool has_height = constraints_node.child("height");
    bool has_x2_position = constraints_node.child("x2");
    bool has_y2_position = constraints_node.child("y2");

    // Must have x and y position. Then, must have x2 OR width but not both, and y2 OR height but not both
    if(constraintsAreValid(has_x_position, has_y_position, has_width, has_height, has_x2_position, has_y2_position)){

        if(has_width){
            width_pixels = atoi(constraints_node.child_value("width"));
        } else if(has_x2_position) {
            const char* x2_anchor = constraints_node.child("x2").child_value("anchor");
            int x2_offset = atoi(constraints_node.child("x2").child_value("offset"));
            width_pixels = (parseAnchor(x2_anchor, true) + x2_offset) - x_pixels;
        }

        if(has_height){
            height_pixels = atoi(constraints_node.child_value("height"));
        } else if(has_y2_position) {
            const char* y2_anchor = constraints_node.child("y2").child_value("anchor");
            int y2_offset = atoi(constraints_node.child("y2").child_value("offset"));
            height_pixels = (parseAnchor(y2_anchor, false) + y2_offset) - y_pixels;
        }

        const char* x_anchor = constraints_node.child("x").child_value("anchor");
        int x_offset = atoi(constraints_node.child("x").child_value("offset"));
        x_pixels = parseAnchor(x_anchor, true) + x_offset;

        const char* y_anchor = constraints_node.child("y").child_value("anchor");
        int y_offset = atoi(constraints_node.child("y").child_value("offset"));
        y_pixels = parseAnchor(y_anchor, false) + y_offset;

    } else {
        Debug::error("Incorrect constraints:\n [%d] X position\n [%d] Y position\n [%d] Width\n [%d] Height\n [%d] X2 position\n [%d] Y2 position\n",
            has_x_position, has_y_position, has_width, has_height, has_x2_position, has_y2_position);
    }
}

bool UIDrawable::XOR(bool a, bool b){
    return (!a != !b);
}

int UIDrawable::parseAnchor(const char* anchor, bool is_x){
    if(!parent){
        // Parent is the screen

        if(strcmp(anchor, "left") == 0 || strcmp(anchor, "up") == 0){
            return 0;
        }
        if(strcmp(anchor, "centered") == 0){
            if(is_x){
                return window_width/2 - width_pixels/2;
            }
            return window_height/2 - height_pixels/2;
        }
        if(strcmp(anchor, "right") == 0 || strcmp(anchor, "down") == 0){
            if(is_x){
                return window_width;
            }
            return window_height;
        }
    } else {
        // Parent is the container class

        if(strcmp(anchor, "left") == 0 || strcmp(anchor, "up") == 0){
            if(anchor[0] == 'l'){
                return parent->getXPixels();
            } else {
                return parent->getYPixels();
            }
        }
        if(strcmp(anchor, "centered") == 0){
            if(is_x){
                return parent->getXPixels() + parent->getWidthPixels()/2 - width_pixels/2;
            }
            return parent->getYPixels() + parent->getHeightPixels()/2 - height_pixels/2;
        }
        if(strcmp(anchor, "right") == 0 || strcmp(anchor, "down") == 0){
            if(is_x){
                return parent->getXPixels() + parent->getWidthPixels();
            }
            return parent->getYPixels() + parent->getHeightPixels();
        }
    }

    return 0; // Error
}

void UIDrawable::setPixelCoordinates(int new_x, int new_y, int new_x2, int new_y2){

    width_pixels = new_x2 - new_x;
    height_pixels = new_y2 - new_y;

    x_pixels = new_x;
    y_pixels = new_y;

    updateDimensions();
    setGLPosition(getGLPosition());
}

bool UIDrawable::constraintsAreValid(bool x, bool y, bool w, bool h, bool x2, bool y2){
    return (x && y && XOR(w, x2) && XOR(h, y2));
}

void UIDrawable::setParent(UIDrawable * parent){
    this->parent = parent;
}

void UIDrawable::receiveNotification(UIDrawable* fromChild){

}
