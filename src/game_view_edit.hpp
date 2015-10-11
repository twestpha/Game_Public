#ifndef GameViewEdit_h
#define GameViewEdit_h

#include "game_view.hpp"
#include "drawable_placer.hpp"
#include "includes/sdl.hpp"

class GameViewEdit : public GameView {
public:
    GameViewEdit(Level& level, RenderDeque& render_stack);

    enum Mode { Painting, Placing };

    void update();
    void handleInputState();
    void handleInput(SDL_Event);
    void setMode(Mode mode);

protected:
    void drawOtherStuff();

    void drawPaintingUI();
    void drawPlacingUI();

    void cycleMode();

    UIDrawable* current_paint;

    TextRenderer fancy_text;
    TextRenderer header_text;

    DrawablePlacer placer;

    Mode current_mode;

    bool tab_key_state;

};

#endif
