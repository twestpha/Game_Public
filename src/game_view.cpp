#include "game_view.hpp"

GameView::GameView(Level& level, RenderDeque& render_stack) : level(&level), gamebuffer(), ui_buffer(), render_stack(&render_stack), healthbar(Texture("res/textures/healthbar_test.png", GL_NEAREST, false), UIDrawable::Center) {

    // // Gaussian Blur shaders
    // Shader blur_horiz("shaders/flat_drawable_noflip.vs", "shaders/framebuffer_horiz_blur.fs");
    // gamebuffer.addShaderPass(blur_horiz);
    //
    // Shader blur_vert("shaders/flat_drawable_noflip.vs", "shaders/framebuffer_vert_blur.fs");
    // gamebuffer.addShaderPass(blur_vert);

    Shader mousebox_shader("shaders/mousebox.vs",
        "shaders/mousebox.fs");

    // Unnecessary, but good to do
    Mouse::getInstance();

    text_renderer = new TextRenderer("Inconsolata-Bold.ttf", 20);

    // Creation of selection box
    selection_box = new UIDrawable(mousebox_shader, 0);
    selection_box->setOutline(true);

    // Creation of a test ui
    // For toggling the view state, the menu is better off for now as a pointer
    // independent of ui_drawables.
    // We should probably come up with a Window::getInstance() manager class that keeps a list
    // of the currently showing UIWindows. This will be good for UIWindows with
    // sub windows.
    menu = new UIWindow();
    menu->loadFromXML("res/layouts/test.xml");
    menu->hide();
    ui_drawables.push_back(menu);

    graphics_menu = new UIWindow();
    graphics_menu->loadFromXML("res/layouts/graphics_settings.xml");
    graphics_menu->hide();
    ui_drawables.push_back(graphics_menu);

    toggle_key_state = false;
    debug_showing = Debug::is_on;

    menu_key_state = false;

    mouse_count = 0;
    left_mouse_button_unclick = false;

    attack_command_prime = false;

    Profile::getInstance()->updateShaderSettings();

    // Set the callback function to be the game view input
    InputHandler::Callback_Type callback_function = std::bind(&GameView::handleInput, this, std::placeholders::_1);
    InputHandler::getInstance()->pushCallback(callback_function);

    InputHandler::State_Callback_Type state_callback_temp = std::bind(&GameView::handleInputState, this);
    InputHandler::getInstance()->setStateCallback(state_callback_temp);

    ui_drawables.push_back(DebugConsole::getInstance());

}

void GameView::update(){
    // Tick the game clock.
    GameClock::getInstance()->tick();

    // Swap display/rendering buffers
    Window::getInstance()->display();

    // Update the units
    level->getUnitManager().updateUnits();

    // Render things
    drawCore();
    drawOtherStuff();

    render_stack->drawAllToScreen();

    handleMouseDragging();

}

void GameView::drawCore(){

    // Render the game map to the gamebuffer
    render_stack->enqueueFramebuffer(gamebuffer);
    level->getGameMap().render();

    // Draw the gamebuffer N - 1 times (the last pass is drawn to the screen).
    // This is how many times the fxaa shader samples the image.
    // A good number is 4, 8 looks blurry, 1 doesn't do much.
    int fxaa_level = Profile::getInstance()->getFxaaLevel();
    if (Profile::getInstance()->isFramebuffersOn()){
        for (int i = 0; i < fxaa_level - 1; ++i){
            gamebuffer.draw();
        }
    }

    // Push the ui framebuffer to the rendering stack
    render_stack->enqueueFramebuffer(ui_buffer);

    glm::vec3 unit_pos = level->getUnitHolder().getUnits()[0].getPosition();
    Camera& camera = level->getGameMap().getCamera();
    glm::vec2 healthbar_pos = GLMHelpers::calculateScreenPosition(camera.getProjectionMatrix(), camera.getViewMatrix(), (unit_pos + glm::vec3(0, 3, 0)));
    healthbar_pos += glm::vec2(0, 0.01);
    healthbar.setGLPosition(healthbar_pos);
    healthbar.draw();

    // Draw all of the ui elements on top of the level
    for(int i = 0; i < ui_drawables.size(); ++i){
        ui_drawables[i]->draw();
    }

    // Draw the debug information
    if (debug_showing){
        float frame_time = GameClock::getInstance()->getDeltaTime();
        float average_frame_time = GameClock::getInstance()->getAverageDeltaTime();

        // Testing text renderer pixel perfection.
        text_renderer->print(10, 10, "fps: %.2f",
            1.0 / frame_time);
        text_renderer->print(10, 30, "avg: %.2f", 1.0 / average_frame_time);

    }

    // The mouse draws on top of everything else
    Mouse::getInstance()->draw();

}

void GameView::drawOtherStuff(){
    // Empty for now
}

void GameView::handleInputState(){
    Camera& camera = level->getGameMap().getCamera();
    glm::mat4 proj_matrix = camera.getProjectionMatrix();
    Terrain& terrain = level->getGameMap().getGround();

    // Get the mouse coordinates gl, and the world
    glm::vec2 mouse_gl_pos = Mouse::getInstance()->getGLPosition();
    glm::vec3 mouse_world_pos = level->getGameMap().calculateWorldPosition(mouse_gl_pos);

    SDL_PumpEvents();
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    //##############################################################################
    // Shift Key Handling
    //##############################################################################
    bool shift_pressed = state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT];

    //##############################################################################
    // Left Mouse Button Handling
    //##############################################################################
    if (left_mouse_button_unclick){
        left_mouse_button_unclick = false;
    }

    if (Mouse::getInstance()->isPressed(Mouse::LEFT)){
        // Left mouse button
        if (attack_command_prime){

            attack_command_prime = false;
            level->getUnitManager().issueOrder(Playable::Order::ATTACK, mouse_world_pos, shift_pressed);
            mouse_count = -1;
            left_mouse_button_unclick = true;

        } /* Probably more orders here */
        else if (mouse_count == 0){
            initial_left_click_position = mouse_gl_pos;
        } else {
            final_left_click_position = mouse_gl_pos;
        }
        mouse_count++;
    } else if(mouse_count != 0){
        mouse_count = 0;
        left_mouse_button_unclick = true;
    }

    //##############################################################################
    // Right Mouse Button Handling
    //##############################################################################
    if (right_mouse_button_unclick){
        right_mouse_button_unclick = false;
    }

    if (Mouse::getInstance()->isPressed(Mouse::RIGHT)){
        // Right mouse button
        if (!right_mouse_button_click){
            level->getUnitManager().issueOrder(Playable::Order::MOVE, mouse_world_pos, shift_pressed);
        }

        attack_command_prime = false;
        right_mouse_button_click = true;

    } else if (right_mouse_button_click){
        right_mouse_button_click = false;
        right_mouse_button_unclick = true;
    }

    //##############################################################################
    // Hold-Action Key Handling
    //##############################################################################
    if (state[SDL_SCANCODE_H]){
        level->getUnitManager().issueOrder(Playable::Order::HOLD_POSITION, mouse_world_pos, shift_pressed);
    }

    //##############################################################################
    // Stop-Action Key Handling
    //##############################################################################
    if (state[SDL_SCANCODE_S] && !debug_showing){
        level->getUnitManager().issueOrder(Playable::Order::STOP, mouse_world_pos, shift_pressed);
    }

    //##############################################################################
    // Attack-Action Key Handling
    //##############################################################################
    if (state[SDL_SCANCODE_A]){
        attack_command_prime = true;
    }
    //
    //##############################################################################
    // Camera Movement Handling
    //##############################################################################
    if (debug_showing){
        handleKeyboardCameraMovement();
    } else {
        handleMouseCameraMovement();
    }

}

void GameView::handleInput(SDL_Event event){
    // Set the cursor to the pointer by default
    Mouse::getInstance()->setCursorSprite(Mouse::cursorType::CURSOR);

    SDL_Scancode key_scancode = event.key.keysym.scancode;
    switch(event.type){
        case SDL_QUIT:
            Window::getInstance()->requestClose();
        break;

        case SDL_KEYDOWN:
            if (key_scancode == SDL_SCANCODE_ESCAPE){
                Window::getInstance()->requestClose();
            } else if (key_scancode == SDL_SCANCODE_T) {
                GameClock::getInstance()->resetAverage();
            } else if ((key_scancode == SDL_SCANCODE_F1) && (!toggle_key_state)){
                toggle_key_state = true;
                debug_showing = !debug_showing;
            } else if ((key_scancode == SDL_SCANCODE_RETURN) && (!debug_console_key_state)){
                debug_console_key_state = true;
                DebugConsole::getInstance()->toggleShowing();
            } else if ((key_scancode == SDL_SCANCODE_G) && (!graphics_menu_key_state)){
                graphics_menu_key_state = true;
                graphics_menu->toggleShowing();
            } else if ((key_scancode == SDL_SCANCODE_F10) && (!menu_key_state)){
                menu_key_state = true;
                menu->toggleShowing();
            } else if ((key_scancode == SDL_SCANCODE_P) && (!printscreen_key_state)){
                printscreen_key_state = true;
                Window::getInstance()->takeScreenshot();
            }
        break;

        case SDL_KEYUP:
            if (key_scancode == SDL_SCANCODE_F1){
                toggle_key_state = false;
            } else if (key_scancode == SDL_SCANCODE_RETURN){
                debug_console_key_state = false;
            } else if (key_scancode == SDL_SCANCODE_G){
                graphics_menu_key_state = false;
            } else if (key_scancode == SDL_SCANCODE_F10){
                menu_key_state = false;
            } else if (key_scancode == SDL_SCANCODE_P){
                printscreen_key_state = false;
            }
        break;

    }

}

void GameView::handleKeyboardCameraMovement(){
    Camera& camera = level->getGameMap().getCamera();

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    // Translation
    if (state[SDL_SCANCODE_W]){
        camera.moveZ(-1);
    }
    if (state[SDL_SCANCODE_S]){
        camera.moveZ(1);
    }
    if (state[SDL_SCANCODE_A]){
        camera.moveX(-1);
    }
    if (state[SDL_SCANCODE_D]){
        camera.moveX(1);
    }
    if (state[SDL_SCANCODE_LSHIFT]){
        camera.moveY(-1);
    }
    if (state[SDL_SCANCODE_SPACE]){
        camera.moveY(1);
    }

    // Rotation
    if (state[SDL_SCANCODE_E]){
        camera.rotateY(-1);
    }
    if (state[SDL_SCANCODE_Q]){
        camera.rotateY(1);
    }
    if (state[SDL_SCANCODE_F]){
        camera.rotateX(-1);
    }
    if (state[SDL_SCANCODE_R]){
        camera.rotateX(1);
    }

    // FOV Changing
    if (state[SDL_SCANCODE_MINUS]){
        camera.zoomIn(0.01);
    }
    if (state[SDL_SCANCODE_EQUALS]){
        camera.zoomOut(0.01);
    }

}

void GameView::handleMouseCameraMovement(){
    Camera& camera = level->getGameMap().getCamera();
    glm::mat4 proj_matrix = camera.getProjectionMatrix();
    Terrain& terrain = level->getGameMap().getGround();

    // Get the mouse coordinates gl, and the world
    glm::vec2 mouse_gl_pos = Mouse::getInstance()->getGLPosition();
    glm::vec3 mouse_world_pos = level->getGameMap().calculateWorldPosition(mouse_gl_pos);

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    // Mouse scrolling the screen when not in debug mode
    if(mouse_count == 0){
        // LEFT
        if(camera.getPosition().x >= -1.0 * level->getGameMap().getGround().getWidth()/2 + 55){
            if(mouse_gl_pos.x < -0.95){
                camera.moveGlobalX(-10);
            } else if(mouse_gl_pos.x < -0.85){
                camera.moveGlobalX(-5);
            }
        }

        // RIGHT
        if(camera.getPosition().x <= 1.0 * level->getGameMap().getGround().getWidth()/2 - 55){
            if(mouse_gl_pos.x > 0.95){
                camera.moveGlobalX(10);
            } else if (mouse_gl_pos.x > 0.85){
                camera.moveGlobalX(5);
            }
        }

        // DOWN
        if(camera.getPosition().z <= 1.0 * level->getGameMap().getGround().getDepth()/2 - 3){
            if(mouse_gl_pos.y < -0.95){
                camera.moveGlobalZ(10);
            } else if(mouse_gl_pos.y < -0.85){
                camera.moveGlobalZ(5);
            }
        }

        // UP                            . Compensating for the camera angle
        if(camera.getPosition().z >= -1.0 * level->getGameMap().getGround().getDepth()/2 + 55){
            if(mouse_gl_pos.y > 0.95){
                camera.moveGlobalZ(-10);
            } else if (mouse_gl_pos.y > 0.85){
                camera.moveGlobalZ(-5);
            }
        }

        // Changing the mouse cursor based on scrolling
        if(mouse_gl_pos.x < -0.85){
            Mouse::getInstance()->setCursorSprite(Mouse::cursorType::LEFT);
        }
        if(mouse_gl_pos.x > 0.85){
            Mouse::getInstance()->setCursorSprite(Mouse::cursorType::RIGHT);
        }
        if(mouse_gl_pos.y > 0.85){
            Mouse::getInstance()->setCursorSprite(Mouse::cursorType::UP);
        }
        if(mouse_gl_pos.y < -0.85){
            Mouse::getInstance()->setCursorSprite(Mouse::cursorType::DOWN);
        }

        if(mouse_gl_pos.x < -0.85 && mouse_gl_pos.y < -0.85){
            Mouse::getInstance()->setCursorSprite(Mouse::cursorType::DOWN_LEFT);
        }
        if(mouse_gl_pos.x > 0.85 && mouse_gl_pos.y < -0.85){
            Mouse::getInstance()->setCursorSprite(Mouse::cursorType::DOWN_RIGHT);
        }
        if(mouse_gl_pos.x < -0.85 && mouse_gl_pos.y > 0.85){
            Mouse::getInstance()->setCursorSprite(Mouse::cursorType::UP_LEFT);
        }
        if(mouse_gl_pos.x > 0.85 && mouse_gl_pos.y > 0.85){
            Mouse::getInstance()->setCursorSprite(Mouse::cursorType::UP_RIGHT);
        }
    }

    // FOV Changing
    if (state[SDL_SCANCODE_MINUS]){
        camera.zoomIn(0.01);
    }
    if (state[SDL_SCANCODE_EQUALS]){
        camera.zoomOut(0.01);
    }
}

void GameView::handleMouseDragging(){
    // draw selection rectangle here and change the cursor based on amount of dragging
    glm::vec3 init = level->getGameMap().calculateWorldPosition(initial_left_click_position);
    glm::vec3 fina = level->getGameMap().calculateWorldPosition(final_left_click_position);

    bool dragged_x = fabs(initial_left_click_position.x - final_left_click_position.x) > 0.05;
    bool dragged_y = fabs(initial_left_click_position.y - final_left_click_position.y) > 0.05;

    if(mouse_count > 1 && !Mouse::getInstance()->isHovering() && (dragged_x || dragged_y)){
        // draw from initial_left_click_position to final_left_click_position
        Mouse::getInstance()->setCursorSprite(Mouse::cursorType::SELECTION);

        level->getUnitManager().tempSelectUnits(init, fina);

        selection_box->setGLCoordinates(initial_left_click_position, final_left_click_position);
        selection_box->draw();
    }
    if(left_mouse_button_unclick && !Mouse::getInstance()->isHovering() && (dragged_x || dragged_y)){

        level->getUnitManager().selectUnits(init, fina);

    } else if(left_mouse_button_unclick && !Mouse::getInstance()->isHovering()){
        level->getUnitManager().selectUnit(level->getGameMap().calculateWorldPosition(Mouse::getInstance()->getGLPosition()));
    }
}
