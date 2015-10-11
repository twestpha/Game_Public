#ifndef World_h
#define World_h

#include <vector>
#include <random>
#include <math.h>

#include "game_view.hpp"
#include "game_view_edit.hpp"
#include "debug.hpp"
#include "window.hpp"
#include "level.hpp"

using namespace std;

class World{
public:
    World(string, bool);
    ~World();

    void update();

private:
    RenderDeque render_stack;
    Level level;
    GameView* game_view;

    bool edit_mode;
};

#endif
