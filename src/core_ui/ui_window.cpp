// ui_window.cpp
// Trevor Westphal

#include "ui_window.hpp"

UIImage* UIWindow::up_edge_image;
UIImage* UIWindow::right_edge_image;
UIImage* UIWindow::left_edge_image;
UIImage* UIWindow::down_edge_image;

UIImage* UIWindow::up_left_corner_image;
UIImage* UIWindow::up_right_corner_image;
UIImage* UIWindow::down_left_corner_image;
UIImage* UIWindow::down_right_corner_image;

UIWindow::UIWindow() : UIDrawable(Texture(glm::vec4(0, 0, 0, 1))){
    is_showing = false;
}

UIWindow::UIWindow(Shader shader) : UIDrawable(shader, Texture(glm::vec4(0, 0, 0, 1))){

    is_showing = false;
}

void UIWindow::loadFromXML(std::string filepath){
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filepath.c_str());

    Debug::info("Parsed layout: %s\n", filepath.c_str());

    pugi::xml_node layout_node = doc.child("layout");

    if(strcmp(layout_node.child_value("type"), "ui_window") != 0){
        Debug::error("Mismatched layout types. Expected ui_window, received %s\n", layout_node.child_value("type"));
        return;
    }

    pugi::xml_node constraints_node = layout_node.child("constraints");
    pugi::xml_node corner_sprites_node = layout_node.child("corner_sprites");
    pugi::xml_node edge_sprites_node = layout_node.child("edge_sprites");
    pugi::xml_node subelements_node = layout_node.child("subelements");

    // Position and Sizing the Window
    parseConstraints(constraints_node);

    // Creating the edges
    if(up_edge_image == NULL){
        up_edge_image = new UIImage(shader, Texture(edge_sprites_node.child_value("up")));
    }

    if(down_edge_image == NULL){
        down_edge_image = new UIImage(shader, Texture(edge_sprites_node.child_value("down")));
    }

    if(right_edge_image == NULL){
        right_edge_image = new UIImage(shader, Texture(edge_sprites_node.child_value("right")));
    }

    if(left_edge_image == NULL){
        left_edge_image = new UIImage(shader, Texture(edge_sprites_node.child_value("left")));
    }

    // Creating and Positioning the corners
    if(up_left_corner_image == NULL){
        up_left_corner_image = new UIImage(shader, Texture(corner_sprites_node.child_value("up_left")));
    }

    if(up_right_corner_image == NULL){
        up_right_corner_image = new UIImage(shader, Texture(corner_sprites_node.child_value("up_right")));
    }

    if(down_right_corner_image == NULL){
        down_right_corner_image = new UIImage(shader, Texture(corner_sprites_node.child_value("down_right")));
    }

    if(down_left_corner_image == NULL){
        down_left_corner_image = new UIImage(shader, Texture(corner_sprites_node.child_value("down_left")));
    }

    // setup the offsets
    edge_inner_offset = atoi(edge_sprites_node.child_value("inner_offset"));
    edge_outer_offset = atoi(edge_sprites_node.child_value("outer_offset"));
    edge_thickness = atoi(edge_sprites_node.child_value("thickness"));

    corner_inner_offset = atoi(corner_sprites_node.child_value("inner_offset"));
    corner_outer_offset = atoi(corner_sprites_node.child_value("outer_offset"));
    corner_width = atoi(corner_sprites_node.child_value("width"));
    corner_height = atoi(corner_sprites_node.child_value("height"));

    for (pugi::xml_node_iterator it = subelements_node.begin(); it != subelements_node.end(); ++it){
         // printf("Named subelement: %s\n", it->name());

        UIDrawable* ui_element = 0;


        if(strcmp(it->name(), "ui_radiobutton") == 0){
            ui_element = new UIRadioButton(shader);
        } else if(strcmp(it->name(), "ui_button") == 0){
            ui_element = new UIButton(shader);
        }

        if(ui_element){
            ui_element->setParent(this);
            ui_element->loadFromXML(it->child_value("layout_filepath"));
            subelements.push_back(ui_element);
        }
    }

    // Convert all pixel coords into screen
    updateDimensions();
    setGLPosition(getGLPosition());

    up_edge_image->setPositionAndDimensions(x_pixels + corner_inner_offset, y_pixels - edge_inner_offset, width_pixels - corner_width, edge_thickness);

    didLoadXML();
}

void UIWindow::draw(){
    if(is_showing){
        Mouse::getInstance()->setHovering();

        FlatDrawable::draw();

        up_edge_image->setPositionAndDimensions(x_pixels + corner_outer_offset, y_pixels - edge_inner_offset, width_pixels - corner_width, edge_thickness);
        right_edge_image->setPositionAndDimensions(x_pixels + width_pixels - edge_outer_offset, y_pixels + corner_outer_offset, edge_thickness, height_pixels - corner_height);
        left_edge_image->setPositionAndDimensions(x_pixels - edge_inner_offset, y_pixels + corner_outer_offset, edge_thickness, height_pixels - corner_height);
        down_edge_image->setPositionAndDimensions(x_pixels + corner_inner_offset, y_pixels + height_pixels - edge_outer_offset, width_pixels - corner_width, edge_thickness);

        up_edge_image->draw();
        right_edge_image->draw();
        left_edge_image->draw();
        down_edge_image->draw();

        up_left_corner_image->setPositionAndDimensions(x_pixels - corner_inner_offset, y_pixels - corner_inner_offset, corner_width, corner_height);
        up_right_corner_image->setPositionAndDimensions(x_pixels + width_pixels - corner_outer_offset, y_pixels - corner_inner_offset, corner_width, corner_height);
        down_left_corner_image->setPositionAndDimensions(x_pixels - corner_inner_offset, y_pixels + height_pixels - corner_outer_offset, corner_width, corner_height);
        down_right_corner_image->setPositionAndDimensions(x_pixels + width_pixels - corner_outer_offset, y_pixels + height_pixels - corner_outer_offset, corner_width, corner_height);

        up_left_corner_image->draw();
        up_right_corner_image->draw();
        down_left_corner_image->draw();
        down_right_corner_image->draw();

        for(int i(0); i < subelements.size(); ++i){
            subelements[i]->draw();
        }
    }
}

void UIWindow::show(){
    is_showing = true;
}

void UIWindow::hide(){
    is_showing = false;
}

void UIWindow::toggleShowing(){
    if (is_showing){
        hide();
    } else {
        show();
    }
}
