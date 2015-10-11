#include "drawable_placer.hpp"

DrawablePlacer::DrawablePlacer(Level& level) : level(&level), selector(level), scale_sensitivity(1.0), rotate_sensitivity(1.0) {

    current_drawable = &(selector.getCurrentDrawable());

}

void DrawablePlacer::setDrawable(Drawable& drawable) {
    current_drawable = &drawable;
    this->level->getGameMap().setTempDrawable(*current_drawable);
}

void DrawablePlacer::update(glm::vec3 mouse_world_pos) {
    setDrawable(selector.getCurrentDrawable());
    current_drawable->setPosition(mouse_world_pos);
}

Drawable& DrawablePlacer::getDrawable() {
    return *current_drawable;
}

void DrawablePlacer::handleInput(SDL_Event event) {
    bool scale_up = false;
    bool scale_down = false;
    bool place_doodad = false;

    float scale_diff = scale_sensitivity *  GameClock::getInstance()->getDeltaTime();

    glm::vec3 rotate_amt;
    float rotate_diff = rotate_sensitivity * GameClock::getInstance()->getDeltaTime();

    selector.handleInput(event);

    SDL_Keycode keycode = event.key.keysym.sym;
    switch(event.type){
        case SDL_KEYDOWN:
            if (keycode == 't') {
                rotate_amt.x = rotate_diff;
            } else if (keycode == 'b') {
                rotate_amt.x = -rotate_diff;
            } else if (keycode == 'f') {
                rotate_amt.y = rotate_diff;
            } else if (keycode == 'h') {
                rotate_amt.y = -rotate_diff;
            } else if (keycode == 'r') {
                rotate_amt.z = -rotate_diff;
            } else if (keycode == 'y') {
                rotate_amt.z = rotate_diff;
            } else if (keycode == 'u') {
                scale_sensitivity += 0.1;
            } else if (keycode == 'j') {
                scale_sensitivity -= 0.1;
            } else if (keycode == 'i') {
                rotate_sensitivity += 0.1;
            } else if (keycode == 'k') {
                rotate_sensitivity -= 0.1;
            }
        break;
        case SDL_MOUSEWHEEL:
            scale_up = event.wheel.y > 0;
            scale_down = event.wheel.y < 0;
        break;

        case SDL_MOUSEBUTTONDOWN:
            place_doodad = event.button.button == SDL_BUTTON_LEFT;
        break;

        default:
        break;
    }

    float current_scale = current_drawable->getScale();
    if (scale_up) {
        current_drawable->setScale(current_scale + scale_diff);
    } else if (scale_down) {
        current_drawable->setScale(current_scale - scale_diff);
    }

    current_drawable->rotateGlobalEuler(rotate_amt);

    if (place_doodad) {
        this->level->getGameMap().placeTempDrawable();
    }

}

void DrawablePlacer::activate() {
    setDrawable(*current_drawable);
}

void DrawablePlacer::deactivate() {
    this->level->getGameMap().removeTempDrawable();
}

float DrawablePlacer::getScaleSensitivity() {
    return scale_sensitivity;
}

float DrawablePlacer::getRotateSensitivity() {
    return rotate_sensitivity;
}
