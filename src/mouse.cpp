#include "mouse.hpp"

Mouse* Mouse::instance;

Mouse* Mouse::getInstance(){
    if(instance){
        return instance;
    } else {
        instance = new Mouse();
        return instance;
    }

}

Mouse::Mouse(): UIDrawable(Texture("res/textures/cursor_ui.png")) {


    int window_width = Window::getInstance()->getWidth();
    int window_height = Window::getInstance()->getHeight();

    inv_mouse_projection = glm::inverse(mouse_projection);

    // CURSOR, SELECTION, COMMAND, UP, RIGHT, DOWN, LEFT, UP_LEFT, UP_RIGHT, DOWN_RIGHT, DOWN_LEFT
    Texture selection = Texture("res/textures/cursor_select_ui.png");
    Texture command = Texture("res/textures/cursor_select_ui.png");
    Texture up = Texture("res/textures/cursor_scroll_up_ui.png");
    Texture right = Texture("res/textures/cursor_scroll_right_ui.png");
    Texture down = Texture("res/textures/cursor_scroll_down_ui.png");
    Texture left = Texture("res/textures/cursor_scroll_left_ui.png");
    Texture up_left = Texture("res/textures/cursor_scroll_up_left_ui.png");
    Texture up_right = Texture("res/textures/cursor_scroll_up_right_ui.png");
    Texture down_right = Texture("res/textures/cursor_scroll_down_right_ui.png");
    Texture down_left = Texture("res/textures/cursor_scroll_down_left_ui.png");

    mouse_sprites.push_back(texture.getGLId());
    mouse_sprites.push_back(selection.getGLId());
    mouse_sprites.push_back(command.getGLId());
    mouse_sprites.push_back(up.getGLId());
    mouse_sprites.push_back(right.getGLId());
    mouse_sprites.push_back(down.getGLId());
    mouse_sprites.push_back(left.getGLId());
    mouse_sprites.push_back(up_left.getGLId());
    mouse_sprites.push_back(up_right.getGLId());
    mouse_sprites.push_back(down_right.getGLId());
    mouse_sprites.push_back(down_left.getGLId());

    hovering = false;

    // Set the cursor to the pointer by default
    setCursorSprite(Mouse::cursorType::CURSOR);

    // Center the mouse on the screen
    Window::getInstance()->centerMouse();

}

void Mouse::setCursorSprite(cursorType cursor_type){
    this->current_type = cursor_type;
}

void Mouse::draw(){
    glm::vec2 current_position = getGLPosition();

    // Position the cursor specially for certain cursors
    // mostly because down, right, up_right, down_right, and down_left run out of the screen
    // also selection needs to be moved to the center
    if(!hovering){
        if(current_type == cursorType::DOWN || current_type == cursorType::DOWN_LEFT){
            setGLPosition(current_position + glm::vec2(0.0, 2*height));
        } else if(current_type == cursorType::RIGHT || current_type == cursorType::UP_RIGHT){
            setGLPosition(current_position + glm::vec2(-2*width, 0.0));
        } else if(current_type == cursorType::DOWN_RIGHT){
            setGLPosition(current_position + glm::vec2(-2*width, 2*height));
        } else if(current_type == cursorType::SELECTION){
            setGLPosition(current_position + glm::vec2(-1*width, height));
        } else {
            setGLPosition(current_position);
        }
    } else {
        setGLPosition(current_position);
        current_type = cursorType::CURSOR;
    }

    attachTexture(mouse_sprites[ static_cast<int>(current_type) ]);

    width_pixels = 66;
    height_pixels = 66;
    updateDimensions();

    UIDrawable::draw();

    hovering = false;
}

glm::vec2 Mouse::getGLPosition(){
    glm::vec2 mouse_position = getScreenPosition();

    x_pixels = int(mouse_position.x) - 1;
    y_pixels = int(mouse_position.y) - 1;

    return UIDrawable::getGLPosition();
}

glm::vec2 Mouse::getScreenPosition(){
    int x, y;
    SDL_PumpEvents();
    SDL_GetMouseState(&x, &y);
    return glm::vec2(x, y);
}

void Mouse::setHovering(){
    hovering = true;
}

bool Mouse::isHovering(){
    return hovering;
}

bool Mouse::isPressed(Button button){
    SDL_PumpEvents();
    bool is_pressed = false;
    switch(button){
        case LEFT:
            is_pressed = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT);
            break;
        case RIGHT:
            is_pressed = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT);
            break;
        case MIDDLE:
            is_pressed = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE);
            break;
    }
    return is_pressed;
}
