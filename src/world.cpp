#include "world.hpp"

World::World(string level_filename, bool edit_mode) : render_stack(), level(level_filename, render_stack){
    this->edit_mode = edit_mode;

    if (edit_mode){
        game_view = new GameViewEdit(level, render_stack);
    } else {
        game_view = new GameView(level, render_stack);
    }

}

World::~World(){

}

void World::update(){
    game_view->update();
}
