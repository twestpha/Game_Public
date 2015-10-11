#include "level.hpp"

Level::Level(string filename, RenderDeque& render_stack) : File(filename), unit_holder(), resource_loader(), game_map(filename, unit_holder, render_stack, resource_loader), unit_manager(game_map, unit_holder) {

    // Temporary stuff
    unit_holder.populate(resource_loader);

}

GameMap& Level::getGameMap() {
    return game_map;
}

UnitManager& Level::getUnitManager() {
    return unit_manager;
}

ResourceLoader& Level::getResourceLoader() {
    return resource_loader;
}

UnitHolder& Level::getUnitHolder() {
    return unit_holder;
}

string Level::asJsonString() {
    string json_string = "{\n";

    json_string += resource_loader.asJsonString() + ",\n";
    json_string += game_map.asJsonString() + "\n";

    json_string += "}";

    return json_string;
}

void Level::saveAs(string filepath) {
    Debug::info("Saving level to %s\n", filepath.c_str());

    // Write level to file
    ofstream myfile;
    myfile.open(filepath);
    myfile << asJsonString();
    myfile.close();
}
