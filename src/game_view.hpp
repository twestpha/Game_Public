#ifndef GameView_h
#define GameView_h

#include <ctime>
#include <vector>

#include "mesh.hpp"
#include "drawable.hpp"
#include "camera.hpp"
#include "flat_mesh.hpp"
#include "flat_drawable.hpp"
#include "text_renderer.hpp"
#include "core_ui/ui_drawable.hpp"
#include "mouse.hpp"
#include "framebuffer.hpp"
#include "shadowbuffer.hpp"
#include "screenbuffer.hpp"
#include "window.hpp"
#include "core_ui/ui_window.hpp"
#include "pathfinder.hpp"
#include "game_clock.hpp"
#include "debug_console.hpp"
#include "game_map.hpp"
#include "level.hpp"
#include "glm_helpers.hpp"

class GameView {
public:

    GameView(Level& level, RenderDeque& render_stack);

    virtual void update();

    virtual void handleInputState();
    virtual void handleInput(SDL_Event);

protected:

    void drawCore();
    virtual void drawOtherStuff();

    void handleKeyboardCameraMovement();
    void handleMouseCameraMovement();
    void handleMouseDragging();

    Level* level;

    Framebuffer gamebuffer;
    Framebuffer ui_buffer;
    RenderDeque* render_stack;

    UIDrawable* selection_box;
    std::vector<UIDrawable*> ui_drawables;

    TextRenderer* text_renderer;

    // Mouse controls
    int mouse_count;
    bool left_mouse_button_unclick;
    glm::vec2 initial_left_click_position;
    glm::vec2 final_left_click_position;

    bool middle_mouse_button_click;

    bool right_mouse_button_click;
    bool right_mouse_button_unclick;

    bool attack_command_prime;

    bool toggle_key_state;
    bool debug_showing;

    bool printscreen_key_state;

    UIWindow* menu;
    bool menu_key_state;

    UIWindow* graphics_menu;
    bool graphics_menu_key_state;

    bool debug_console_key_state;

    UIDrawable healthbar;

};

#endif
