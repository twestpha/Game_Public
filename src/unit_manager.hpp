#ifndef UnitManager_h
#define UnitManager_h

#include "playable.hpp"
#include "unit_holder.hpp"
#include "game_map.hpp"

using namespace std;

class UnitManager {
public:
    UnitManager(GameMap& game_map, UnitHolder& units);

    void issueOrder(Playable::Order, glm::vec3, bool);
    void selectUnit(glm::vec3);
    void selectUnits(glm::vec3, glm::vec3);
    void tempSelectUnits(glm::vec3, glm::vec3);

    void updateUnits();

private:
    float getDistance(float, float, float, float);

    vector<Playable*> selected_units;
    UnitHolder* unit_holder;

    GameMap* game_map;
    PathFinder pathfinder;

};

#endif
