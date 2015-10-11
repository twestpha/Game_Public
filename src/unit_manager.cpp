#include "unit_manager.hpp"

UnitManager::UnitManager(GameMap& game_map, UnitHolder& units) : unit_holder(&units), game_map(&game_map), pathfinder(game_map.getGround()) {

}

void UnitManager::issueOrder(Playable::Order order, glm::vec3 target, bool should_enqueue){

    // even shorter circuit for "not my unit, can't command it".

    // short circuit for stop and hold position
    // No need to path!

    // We need to decide if it's targeting a unit with this command or not
    Playable* targeted_unit = 0;

    vector<Playable>& all_units = unit_holder->getUnits();

    // Check all the other playables to see if one is the target
    for(int i = 0; i < all_units.size(); ++i){
        glm::vec3 unit_pos = all_units[i].getPosition();
        float click_distance_from_unit = getDistance(unit_pos.x, unit_pos.z, target.x, target.z);

        if(click_distance_from_unit < all_units[i].getRadius()){
            targeted_unit = &all_units[i];
        }
    }
    // Debug::info("Selected all_units size: %d\n", selected_units.size());

    // If it's only one unit
    float x_center, z_center, smallest_radius;
    if (selected_units.size() != 0){
        x_center = selected_units[0]->getPosition().x;
        z_center = selected_units[0]->getPosition().z;
        smallest_radius = selected_units[0]->getRadius();
    } else {
        // Short circuit for bug when the selected all_units is empty
        return;
    }

    // Make sure if it skips the click distance calculation so that it will not offset
    float click_distance = -1.0f;
    float max_distance = 0.0f;

    // If there is more than one unit, setup the magic box
    if(selected_units.size() > 1){
        float x_sum = 0.0f;
        float z_sum = 0.0f;

        for(int i = 0; i < selected_units.size(); ++i){
            glm::vec3 unit_pos = selected_units[i]->getPosition();
            x_sum += unit_pos.x;
            z_sum += unit_pos.z;
        }

        x_center = x_sum / selected_units.size();
        z_center = z_sum / selected_units.size();

        for(int i = 0; i < selected_units.size(); ++i){

            glm::vec3 unit_pos = selected_units[i]->getPosition();
            float distance = getDistance(unit_pos.x, unit_pos.z, x_center, z_center);

            if(distance > max_distance){
                max_distance = distance;
            }

            if(selected_units[i]->getRadius() < smallest_radius){
                smallest_radius = selected_units[i]->getRadius();
            }
        }

        click_distance = getDistance(target.x, target.z, x_center, z_center);
    }

    // Start logging the pathfinding time
    float start_time = GameClock::getInstance()->getCurrentTime();

    // Create the path for all all_units in the selection
    vector<glm::vec3> path = pathfinder.find_path(int(x_center), int(z_center), int(target.x), int(target.z), smallest_radius);

    // End logging and report
    float delta_time = GameClock::getInstance()->getCurrentTime() - start_time;
    Debug::info("Took %.2f seconds to find the path.\n", delta_time);

    // Issue the appropriate order
    for(int i = 0; i < selected_units.size(); ++i){

        float x_to_move = target.x;
        float z_to_move = target.z;
        glm::vec3 unit_pos = selected_units[i]->getPosition();

        if(click_distance > max_distance){
            x_to_move += (unit_pos.x - x_center);
            z_to_move += (unit_pos.z - z_center);
        }

        selected_units[i]->receiveOrder(order, glm::vec3(x_to_move, 0.0f, z_to_move), should_enqueue, path, targeted_unit);
    }

}

void UnitManager::selectUnit(glm::vec3 click){

    vector<Playable*> selected_units_copy = selected_units;
    selected_units.clear();

    float nearest = FLT_MAX;
    Playable* nearest_playable = 0;

    vector<Playable>& all_units = unit_holder->getUnits();

    for(int i = 0; i < all_units.size(); ++i){

        glm::vec3 unit_pos = all_units[i].getPosition();
        float distance = getDistance(unit_pos.x, unit_pos.z, click.x, click.z);

        if( distance < all_units[i].getRadius() && distance < nearest){
            nearest = distance;
            nearest_playable = &all_units[i];
        } else {
            all_units[i].deSelect();
        }
    }

    // If we found one that was clicked on and is the nearest
    if(nearest_playable){
        nearest_playable->select();
        selected_units.push_back(nearest_playable);
    } else {
        selected_units = selected_units_copy;

        for(int i = 0; i < selected_units.size(); ++i){
            selected_units[i]->select();
        }
    }
}

void UnitManager::selectUnits(glm::vec3 coord_a, glm::vec3 coord_b){
    vector<Playable*> selected_units_copy = selected_units;
    selected_units.clear();

    float left = min(coord_a.x, coord_b.x);
    float right = max(coord_a.x, coord_b.x);

    float down = min(coord_a.z, coord_b.z);
    float up = max(coord_a.z, coord_b.z);

    vector<Playable>& all_units = unit_holder->getUnits();

    for(int i = 0; i < all_units.size(); ++i){
        if(all_units[i].isTempSelected()){
            all_units[i].select();
            all_units[i].tempDeSelect();
            selected_units.push_back(&all_units[i]);
        } else {
            all_units[i].deSelect();
        }
    }

    if(selected_units.size() == 0){
        selected_units = selected_units_copy;

        for(int i = 0; i < selected_units.size(); ++i){
            selected_units[i]->select();
        }
    }

}

void UnitManager::tempSelectUnits(glm::vec3 coord_a, glm::vec3 coord_b){
    float left = min(coord_a.x, coord_b.x);
    float right = max(coord_a.x, coord_b.x);

    float down = min(coord_a.z, coord_b.z);
    float up = max(coord_a.z, coord_b.z);

    vector<Playable>& all_units = unit_holder->getUnits();

    for(int i = 0; i < all_units.size(); ++i){
        glm::vec3 unit_pos = all_units[i].getPosition();
        float radius = all_units[i].getRadius();

        if(left - radius < unit_pos.x && right + radius > unit_pos.x && down - radius < unit_pos.z && up + radius > unit_pos.z){
            all_units[i].tempSelect();
        } else {
            all_units[i].tempDeSelect();
        }
    }
}

float UnitManager::getDistance(float a1, float a2, float b1, float b2){
    float x_diff = fabs(a1 - b1);
    float z_diff = fabs(a2 - b2);
    return sqrt(x_diff*x_diff + z_diff*z_diff);
}

void UnitManager::updateUnits(){
    // update all the units
    for (Playable& unit : unit_holder->getUnits()){
        vector<Playable*> empty_units;
        unit.update(&(game_map->getGround()), &empty_units);
    }
}
