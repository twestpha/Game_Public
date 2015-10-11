#ifndef DrawablePlacer_h
#define DrawablePlacer_h

#include "includes/glm.hpp"
#include "includes/sdl.hpp"

#include "drawable.hpp"
#include "resource_loader.hpp"
#include "doodad.hpp"
#include "level.hpp"
#include "drawable_selector.hpp"
#include "game_clock.hpp"

class DrawablePlacer {
public:
    DrawablePlacer(Level& level);

    void setDrawable(Drawable& drawable);
    Drawable& getDrawable();

    void update(glm::vec3 mouse_world_pos);
    void handleInput(SDL_Event event);

    float getScaleSensitivity();
    float getRotateSensitivity();

    void activate();
    void deactivate();

private:

    Drawable* createDefaultDoodad();

    DrawableSelector selector;
    Level* level;
    Drawable* current_drawable;
    glm::vec3 axis;

    float scale_sensitivity;
    float rotate_sensitivity;

};

#endif
