// ui_button.cpp
// Trevor Westphal

#include "ui_button.hpp"

UIImage* UIButton::center_image;
UIImage* UIButton::left_image;
UIImage* UIButton::right_image;

UIImage* UIButton::center_hover_image;
UIImage* UIButton::left_hover_image;
UIImage* UIButton::right_hover_image;

UIButton::UIButton(Shader shader) : UIDrawable(shader, Texture(glm::vec4(1, 0, 1, 1))){

}

void UIButton::loadFromXML(std::string filepath){
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filepath.c_str());

    Debug::info("Parsed layout: %s\n", filepath.c_str());

    pugi::xml_node layout_node = doc.child("layout");

    if(strcmp(layout_node.child_value("type"), "ui_button") != 0){
        Debug::error("Mismatched layout types. Expected ui_button, received %s\n", layout_node.child_value("type"));
        return;
    }

    pugi::xml_node constraints_node = layout_node.child("constraints");

    // Parse function
    functionName = layout_node.child_value("function");

    // Parse button text
    text_renderer = new TextRenderer(layout_node.child_value("font"), 22);
    button_text = layout_node.child_value("text");

    // Get button text size
    button_text_width = button_text.length()*16;

    // Parse constraints
    height_pixels = 32;

    parseConstraints(constraints_node);

    // Add in decorative images
    if(center_image == NULL){
        center_image = new UIImage(shader, Texture("res/textures/button_center.png"));
    }

    if(left_image == NULL){
        left_image = new UIImage(shader, Texture("res/textures/button_left.png"));
    }

    if(right_image == NULL){
        right_image = new UIImage(shader, Texture("res/textures/button_right.png"));
    }

    if(center_hover_image == NULL){
        center_hover_image = new UIImage(shader, Texture("res/textures/button_center_hover.png"));
    }

    if(right_hover_image == NULL){
        right_hover_image = new UIImage(shader, Texture("res/textures/button_right_hover.png"));
    }

    if(left_hover_image == NULL){
        left_hover_image = new UIImage(shader, Texture("res/textures/button_left_hover.png"));
    }

    // Convert all pixel coords into screen
    updateDimensions();
    setGLPosition(getGLPosition());

    didLoadXML();
}

bool UIButton::constraintsAreValid(bool x, bool y, bool w, bool h, bool x2, bool y2){
    return (x && y && w);
}

void UIButton::draw(){

    // A check to see if the radio button has been created properly
    if(center_image){

        // Draws pink bounding box. Useful for debugging
        // FlatDrawable::draw();

        center_image->setPositionAndDimensions(x_pixels + 15, y_pixels, width_pixels - 30, 32);
        left_image->setPositionAndDimensions(x_pixels - 1, y_pixels, 17, 32);
        right_image->setPositionAndDimensions(x_pixels + width_pixels - 16, y_pixels, 17, 32);

        center_image->draw();
        left_image->draw();
        right_image->draw();

        text_renderer->print(x_pixels + width_pixels/2 - button_text_width/2, y_pixels, "%s", button_text.c_str());

         glm::vec2 gl_mouse_position = Mouse::getInstance()->getGLPosition();
        if(gl_mouse_position.x < position.x + width &&
           gl_mouse_position.x > position.x - width &&
           gl_mouse_position.y < position.y + height &&
           gl_mouse_position.y > position.y - height){

            center_hover_image->setPositionAndDimensions(x_pixels + 15, y_pixels, width_pixels - 30, 32);
            left_hover_image->setPositionAndDimensions(x_pixels - 1, y_pixels, 17, 32);
            right_hover_image->setPositionAndDimensions(x_pixels + width_pixels - 16, y_pixels, 17, 32);

            center_hover_image->draw();
            left_hover_image->draw();
            right_hover_image->draw();

            bool clicking = Mouse::getInstance()->isPressed(Mouse::LEFT);
            if(clicking && !has_clicked){

                // Run the extended execution of a potential child class
                onClick();

                // Alert the parent of the notification
                parent->receiveNotification(this);

                FunctionHelper::getInstance()->runFunction(functionName);

                has_clicked = true;
            } else if(!clicking){
                has_clicked = false;
            }

        }

    }

}
