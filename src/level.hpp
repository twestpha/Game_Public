#ifndef Level_h
#define Level_h

#include <fstream>

#include "game_map.hpp"
#include "unit_manager.hpp"
#include "resource_loader.hpp"
#include "file.hpp"

using namespace std;

class Level : public File {
public:
    Level(string filename, RenderDeque& render_stack);

    GameMap& getGameMap();
    UnitManager& getUnitManager();
    ResourceLoader& getResourceLoader();
    UnitHolder& getUnitHolder();

    string asJsonString();

    void saveAs(string filename);

private:
    UnitHolder unit_holder;
    ResourceLoader resource_loader;
    GameMap game_map;
    UnitManager unit_manager;
    
};


#endif
