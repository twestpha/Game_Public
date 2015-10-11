// ui_radiobutton.cpp
// Trevor Westphal

#include "ui_radiobutton.hpp"

UIImage* UIRadioButton::on_icon;
UIImage* UIRadioButton::off_icon;
UIImage* UIRadioButton::hover_icon;

UIRadioButton::UIRadioButton(Shader shader) : UIDrawable(shader,Texture(glm::vec4(1, 0, 1, 1))){
    this->has_clicked = false;
}

void UIRadioButton::loadFromXML(std::string filepath){
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filepath.c_str());

    Debug::info("Parsed layout: %s\n", filepath.c_str());

    pugi::xml_node layout_node = doc.child("layout");

    if(strcmp(layout_node.child_value("type"), "ui_radiobutton") != 0){
        Debug::error("Mismatched layout types. Expected ui_radiobutton, received %s\n", layout_node.child_value("type"));
        return;
    }

    pugi::xml_node constraints_node = layout_node.child("constraints");
    pugi::xml_node sprites_node = layout_node.child("sprites");

    // Parse function
    functionName = layout_node.child_value("function");

    // Parse radiobutton text
    text_renderer = new TextRenderer(layout_node.child_value("font"), 22);
    radio_text = layout_node.child_value("text");

    // Parse constraints
    width_pixels = radio_text.length()*16 + 16;
    height_pixels = 28;

    parseConstraints(constraints_node);

    // Set the initial value of the radio button
    radioButtonOn = (strcmp(layout_node.child_value("mode"), "on") == 0);

    // Create and position the button icon and hover highlight
    if(on_icon == NULL){
        on_icon = new UIImage(shader, Texture(sprites_node.child_value("on_icon")));
    }

    if(off_icon == NULL){
        off_icon = new UIImage(shader, Texture(sprites_node.child_value("off_icon")));
    }

    if(hover_icon == NULL){
        hover_icon = new UIImage(shader, Texture(sprites_node.child_value("hover_icon")));
    }

    icon_width = atoi(sprites_node.child_value("width"));
    icon_height = atoi(sprites_node.child_value("height"));

    if(radioButtonOn){
        current_icon = on_icon;
    } else {
        current_icon = off_icon;
    }

    // Convert all pixel coords into screen
    updateDimensions();
    setGLPosition(getGLPosition());

    didLoadXML();
}

void UIRadioButton::draw(){

    // A check to see if the radio button has been created properly
    if(current_icon){

        // Draws pink bounding box. Useful for debugging
        // FlatDrawable::draw();

        current_icon->setPositionAndDimensions(x_pixels, y_pixels, icon_width, icon_height);
        current_icon->draw();

        text_renderer->print(x_pixels + 42, y_pixels, "%s", radio_text.c_str());

        glm::vec2 gl_mouse_position = Mouse::getInstance()->getGLPosition();
        if(gl_mouse_position.x < position.x + width &&
            gl_mouse_position.x > position.x - width &&
            gl_mouse_position.y < position.y + height &&
            gl_mouse_position.y > position.y - height){

            hover_icon->setPositionAndDimensions(x_pixels, y_pixels, icon_width, icon_height);
            hover_icon->draw();

            bool clicking = Mouse::getInstance()->isPressed(Mouse::LEFT);
            if(clicking && !has_clicked){

                // Run the extended execution of a potential child class
                onClick();

                // Alert the parent of the notification
                parent->receiveNotification(this);

                // Toggle the button
                toggleRadioButton();

                has_clicked = true;
            } else if(!clicking){
                has_clicked = false;
            }
        }
    }

}

bool UIRadioButton::constraintsAreValid(bool x, bool y, bool w, bool h, bool x2, bool y2){
    return (x && y);
}

void UIRadioButton::toggleRadioButton(){

    FunctionHelper::getInstance()->runFunction(functionName);

    radioButtonOn = !radioButtonOn;

    if(radioButtonOn){
        current_icon = on_icon;
    } else {
        current_icon = off_icon;
    }
}
