// playable.cpp
// Trevor Westphal

#include "playable.hpp"

Doodad* Playable::selection_ring;

// Magical numbers
#define AT_TARGET_PATH_RADIUS 0.1f
#define AT_TARGET_LAST_RADIUS 4.0f

#define ANGLE_PRECISION 0.1f

#define TURN_CW 1
#define TURN_CCW -1
#define TURN_NONE 0

#define PATH_WIDTH 2.0f

//#############################################
// Text headers from
// http://www.network-science.de/ascii/
// using 'banner3' setting
//#############################################

//##################################################################################################
//
//  ######  ######## ######## ##     ## ########
// ##    ## ##          ##    ##     ## ##     ##
// ##       ##          ##    ##     ## ##     ##
//  ######  ######      ##    ##     ## ########
//       ## ##          ##    ##     ## ##
// ##    ## ##          ##    ##     ## ##
//  ######  ########    ##     #######  ##
//
//##################################################################################################

Playable::Playable() : Drawable(){

}

Playable::Playable(Mesh& mesh, Shader& shader, glm::vec3 position, GLfloat scale) : Drawable(mesh, shader, position, scale) {

    if(! selection_ring){
    	Mesh* selection_ring_mesh = new Mesh("res/models/selection_ring.dae");
    	selection_ring = new Doodad(*selection_ring_mesh, shader, position, 2.0f);
    	selection_ring->setEmissive(Texture("res/textures/selection_ring.png"));
        selection_ring->rotateGlobalEuler(M_PI/2.0f, 0.0f, 0.0f);
    }

	selected = false;
    temp_selected = false;

    #warning Fix the 90* offset bug
    // rotateGlobalEuler(M_PI/2.0f, 0.0f, 0.0f);

    target_position = position;

    first_step_since_order = false;

    last_attack_timestamp = GameClock::getInstance()->getCurrentTime();

    // remove me when weapon is implemented
    health = 200;
    max_health = 200;
    weapon_cooldown = 0.5f; // In milliseconds
    weapon_range = 3.0f;
    weapon_damage = 20;
}

Drawable* Playable::clone() {
    return new Playable(*this);
}

void Playable::loadFromXML(std::string filepath){
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filepath.c_str());

    if(!result){
        Debug::error("Could not load unit: %s\n", filepath.c_str());
        return;
    }

    Debug::info("Parsed unit: %s\n", filepath.c_str());

    pugi::xml_node unit_node = doc.child("unit");

    unit_type = unit_node.child_value("unit_type");
    speed = std::stof(unit_node.child_value("speed"));
    acceleration = std::stof(unit_node.child_value("acceleration"));
    turning_speed = std::stof(unit_node.child_value("turning_speed"));
    radius = std::stof(unit_node.child_value("radius"));
    sight_radius = std::stof(unit_node.child_value("sight_radius"));

}

void Playable::updateUniformData(){
	glUniform1f(glGetUniformLocation(shader->getGLId(), "scale"), scale);
}

//##################################################################################################
//
//  #######  ########  ########  ######## ########   ######
// ##     ## ##     ## ##     ## ##       ##     ## ##    ##
// ##     ## ##     ## ##     ## ##       ##     ## ##
// ##     ## ########  ##     ## ######   ########   ######
// ##     ## ##   ##   ##     ## ##       ##   ##         ##
// ##     ## ##    ##  ##     ## ##       ##    ##  ##    ##
//  #######  ##     ## ########  ######## ##     ##  ######
//
//##################################################################################################

void Playable::receiveOrder(Playable::Order order, glm::vec3 target, bool should_enqueue, std::vector<glm::vec3> path, Playable* targeted_unit){

    // Error that exists: Pathing is done from current position, not future position. Need to fix that.

    // Are we targeting another playable?
    bool is_targeting = bool(targeted_unit);

    // Get the corresponding body and final orders
    Playable::Order body_order = determineBodyOrder(order, is_targeting);
    Playable::Order last_order = determineLastOrder(order, is_targeting);

    // Allocate the temp queues
    std::queue<std::tuple<Playable::Order, glm::vec3>> temp_order_queue;
    std::queue<Playable*> temp_target_queue;

    // Feed the path and the body orders into the queue
    int size = path.size();

    for(int i = 0; i < size - 1; ++i){
        // Protect from 0-length paths
        if(size > 0){
            temp_order_queue.push(std::make_tuple(body_order, path[i]));
            temp_target_queue.push(NULL);
        }
    }

    // Finalize the queue with the last target and last_order
    temp_order_queue.push(std::make_tuple(last_order, target));
    temp_target_queue.push(targeted_unit);

    // If we're not enqueuing the order, we should clear out the old
    if(!should_enqueue){

        first_step_since_order = true;

        // These could theoretically be together, because they should always be
        // the same length. However, I feel more comfortable with them apart.

        while(! order_queue.empty()){
            order_queue.pop();
        }

        while(!target_queue.empty()){
            target_queue.pop();
        }
    }

    // Queue them up
    while( ! temp_order_queue.empty()){
        order_queue.push(temp_order_queue.front());
        target_queue.push(temp_target_queue.front());

        temp_order_queue.pop();
        temp_target_queue.pop();
    }

    // Get the positioning and direction set up
    target_position = position;
    setTargetPositionAndDirection(std::get<1>(order_queue.front()));
}

Playable::Order Playable::determineBodyOrder(Playable::Order order, bool is_targeting){
    if(order == Playable::Order::MOVE){
        return Playable::Order::MOVE;
    } else if (order == Playable::Order::ATTACK && is_targeting){
        return Playable::Order::MOVE;
    } else {
        return Playable::Order::ATTACK_MOVE;
    }
}

Playable::Order Playable::determineLastOrder(Playable::Order order, bool is_targeting){

    if(order == Playable::Order::STOP || order == Playable::Order::HOLD_POSITION){
        return order;
    }

    if(order == Playable::Order::MOVE && is_targeting){
        return Playable::Order::MOVE_TARGET;
    } else if (order == Playable::Order::MOVE){
        return Playable::Order::MOVE;
    } else if (order == Playable::Order::ATTACK && is_targeting){
        return Playable::Order::ATTACK_TARGET;
    } else {
        return Playable::Order::ATTACK_MOVE;
    }
}

void Playable::holdPosition(){
    // Nothing yet
}

void Playable::stop(){
    // Nothing yet
}

//##################################################################################################
//
//  ######  ######## ##       ########  ######  ########
// ##    ## ##       ##       ##       ##    ##    ##
// ##       ##       ##       ##       ##          ##
//  ######  ######   ##       ######   ##          ##
//       ## ##       ##       ##       ##          ##
// ##    ## ##       ##       ##       ##    ##    ##
//  ######  ######## ######## ########  ######     ##
//
//##################################################################################################

void Playable::select(){
    selected = true;
}

void Playable::deSelect(){
    selected = false;
    temp_selected = false;
}

void Playable::tempSelect(){
    temp_selected = true;
}

void Playable::tempDeSelect(){
    temp_selected = false;
}

void Playable::setTeam(int t){
    if (t == 1){
        Drawable::setEmissive(Texture(glm::vec4(0.5, 0.5, 1, 0.1)));
    } else if (t == 2) {
        Drawable::setEmissive(Texture(glm::vec4(0.172f, 0.855f, 0.424f, 0.1)));
    }
    team_number = t;
}


//##################################################################################################
//
// ##        #######   ######     ###    ######## ####  #######  ##    ##
// ##       ##     ## ##    ##   ## ##      ##     ##  ##     ## ###   ##
// ##       ##     ## ##        ##   ##     ##     ##  ##     ## ####  ##
// ##       ##     ## ##       ##     ##    ##     ##  ##     ## ## ## ##
// ##       ##     ## ##       #########    ##     ##  ##     ## ##  ####
// ##       ##     ## ##    ## ##     ##    ##     ##  ##     ## ##   ###
// ########  #######   ######  ##     ##    ##    ####  #######  ##    ##
//
//##################################################################################################

bool Playable::atTargetPosition(){
    if(order_queue.size() == 0){
        // We're at the last position
        return getDistance(position.x, position.z, target_position.x, target_position.z) < AT_TARGET_PATH_RADIUS;
    } else {
        return getDistance(position.x, position.z, target_position.x, target_position.z) < AT_TARGET_LAST_RADIUS;
    }
}

float Playable::getDistance(float a1, float a2, float b1, float b2){
    float x_diff = fabs(a1 - b1);
    float z_diff = fabs(a2 - b2);
    return sqrt(x_diff*x_diff + z_diff*z_diff);
}

void Playable::setTargetPositionAndDirection(glm::vec3 target){
    old_target_position = target_position;
    target_position = target;

    target_direction = getCurrentTargetDirection();
}

float Playable::getCurrentTargetDirection(){
    float x_delta = target_position.x - position.x;
    float z_delta = target_position.z - position.z;
    return atan2(x_delta, z_delta);
}

//##################################################################################################
//
//  ######  ######## ######## ######## ########  #### ##    ##  ######
// ##    ##    ##    ##       ##       ##     ##  ##  ###   ## ##    ##
// ##          ##    ##       ##       ##     ##  ##  ####  ## ##
//  ######     ##    ######   ######   ########   ##  ## ## ## ##   ####
//       ##    ##    ##       ##       ##   ##    ##  ##  #### ##    ##
// ##    ##    ##    ##       ##       ##    ##   ##  ##   ### ##    ##
//  ######     ##    ######## ######## ##     ## #### ##    ##  ######
//
//##################################################################################################

int Playable::steerToStayOnPath(){

    // Short-circuit for the last target, we want to move precisely to it.
    if(order_queue.size() == 0){

        // Set the target position and (more importantly) the current target direction
        float current_target_direction = getCurrentTargetDirection();

        // http://stackoverflow.com/questions/1878907/the-smallest-difference-between-2-angles
        float angle_delta = atan2(sin(current_target_direction-rotation.y), cos(current_target_direction-rotation.y));

        // Needs to rotate to face the movement direction
        if(fabs(angle_delta) > ANGLE_PRECISION){
            if(angle_delta < 0){
                return TURN_CCW;
            }
            return TURN_CW;
        }
        return TURN_NONE;
    }

    // Predict future point           * Scaling the amount of prediction would go here
    float prediction_x = position.x + sin(rotation.y);
    float prediction_z = position.z + cos(rotation.y);

    // Put them in vec2s
    glm::vec2 line_0 = glm::vec2(old_target_position.x, old_target_position.z);
    glm::vec2 line_1 = glm::vec2(target_position.x, target_position.z);
    glm::vec2 point = glm::vec2(prediction_x, prediction_z);

    // Find the point on the path nearest to prediction
    float distance = distanceFromPointToLine(line_0, line_1, point);

    // Steer the appropriate direction
    if(distance > PATH_WIDTH){
        glm::vec2 result_steering_CCW = glm::vec2(position.x + sin(rotation.y - turning_speed),
                                                  position.z + cos(rotation.y - turning_speed));

        glm::vec2 result_steering_CW  = glm::vec2(position.x + sin(rotation.y + turning_speed),
                                                  position.z + cos(rotation.y + turning_speed));

        float CCW_distance = distanceFromPointToLine(line_0, line_1, result_steering_CCW);
        float CW_distance  = distanceFromPointToLine(line_0, line_1, result_steering_CW);

        if(CCW_distance > CW_distance){
            return TURN_CW;
        } else {
            return TURN_CCW;
        }
    }

    return TURN_NONE;
}

int Playable::steerAwayFromUnit(Playable *unit){

}

int Playable::steerAwayFromObstacle(Terrain *ground){

}

float Playable::distanceFromPointToLine(glm::vec2 line_0, glm::vec2 line_1, glm::vec2 point){
     glm::vec2 v = line_1 - line_0;
     glm::vec2 w = point - line_0;

     float c1 = glm::dot(w,v);
     float c2 = glm::dot(v,v);
     float b = c1 / c2;

     glm::vec2 Pb = line_0 + (b * v);
     return getDistance(point.x, point.y, Pb.x, Pb.y);
}

void Playable::setFlying(bool flying) {
    if (flying) {
        distance_off_ground = 7.0;
    } else {
        distance_off_ground = 0.0;
    }
}


//##################################################################################################
//
//  ######   #######  ##     ## ########     ###    ########
// ##    ## ##     ## ###   ### ##     ##   ## ##      ##
// ##       ##     ## #### #### ##     ##  ##   ##     ##
// ##       ##     ## ## ### ## ########  ##     ##    ##
// ##       ##     ## ##     ## ##     ## #########    ##
// ##    ## ##     ## ##     ## ##     ## ##     ##    ##
//  ######   #######  ##     ## ########  ##     ##    ##
//
//##################################################################################################

bool Playable::isEnemy(int t){
    return team_number != t;
}

void Playable::attack(Playable *enemy){

    if(health < 1 || enemy == NULL){
        // Can't attack when dead
        // Also can't attack null targets
        return;
    }

    float delta_time = GameClock::getInstance()->getCurrentTime() - last_attack_timestamp;

    if(delta_time > weapon_cooldown){
        enemy->takeDamage(weapon_damage);
        last_attack_timestamp = GameClock::getInstance()->getCurrentTime();
    }
}

void Playable::takeDamage(int damage_amount){
    health = std::max(health - damage_amount, 0);
}

Playable* Playable::getUnitToAttack(std::vector<Playable*> *otherUnits){

    // Find the highest priority unit -or- the unit that is attacking you -or- unit you attacked last
    Playable* other_unit = 0;

    for(int i(0); i < otherUnits->size(); ++i){
        if(otherUnits->at(i) != this){
            other_unit = otherUnits->at(i);
        }
    }

    return other_unit;
}


//##################################################################################################
//
// ##     ## ########  ########     ###    ######## ########
// ##     ## ##     ## ##     ##   ## ##      ##    ##
// ##     ## ##     ## ##     ##  ##   ##     ##    ##
// ##     ## ########  ##     ## ##     ##    ##    ######
// ##     ## ##        ##     ## #########    ##    ##
// ##     ## ##        ##     ## ##     ##    ##    ##
//  #######  ##        ########  ##     ##    ##    ########
//
//##################################################################################################

void Playable::scanUnits(std::vector<Playable*> *otherUnits){
    // Scan all units, looking for a stuff
    // Nearest friendly, hurt unit - for healing or repair
    // Nearest friendly town hall  - resource return
    // Nearest Enemy unit/struct   - to attack if in engage range
    // Nearest resource            - to gather

    attackable_units.clear();

    Playable* current_unit = 0;

    for(int i = 0; i < otherUnits->size(); ++i){

        current_unit = otherUnits->at(i);

        if(current_unit != this){

            // If it is an enemy and in range, we could potentially attack it
            float distance_to_unit = getDistance(position.x, position.z, current_unit->getPosition().x, current_unit->getPosition().z);

            if(current_unit->getTeam() != team_number && distance_to_unit <= radius + current_unit->radius + weapon_range){
                attackable_units.push_back(current_unit);
            }

        }
    }
}

void Playable::update(Terrain* ground, std::vector<Playable*> *otherUnits){

    // Setting up attack variables
    // Playable* enemy_to_attack = getNearestEnemyToAttack(otherUnits);
    // enemy_in_sight_range = (enemies_in_range.size() > 0);
    // has_been_given_attack_order = (target_order == Playable::Order::ATTACK_MOVE);



    // Attacking         Weapon range + our size + their size

    scanUnits(otherUnits);

    Playable* unit_to_attack = getUnitToAttack(otherUnits);

    bool should_engage_enemies = target_order == Playable::Order::ATTACK_MOVE;

    if(should_engage_enemies){
        attack(unit_to_attack);
    }





    // Turning on the first step
    if(first_step_since_order){
        first_step_since_order = false;
        turning_during_first_step = true;
    }

    if(atTargetPosition() && order_queue.size() > 0){

        // Get the next order and target from the queue
        target_order = std::get<0>(order_queue.front());
        setTargetPositionAndDirection(std::get<1>(order_queue.front()));

        order_queue.pop();
        target_queue.pop();

    } else if(!atTargetPosition()){

        // All if we're not a flying unit



        if(turning_during_first_step){

            position.x += sin(target_direction)*speed;
            position.z += cos(target_direction)*speed;

            // http://stackoverflow.com/questions/1878907/the-smallest-difference-between-2-angles
            float angle_delta = atan2(sin(target_direction-rotation.y), cos(target_direction-rotation.y));

            // Needs to rotate to face the movement direction
            if(fabs(angle_delta) > 0.01){

                float sign = 1.0f;

                if(angle_delta < 0){
                    sign = -1.0f;
                }

                if(fabs(angle_delta) < turning_speed){
                    setRotationEuler(rotation.x, target_direction, rotation.z);
                    turning_during_first_step = false;
                } else {
                    setRotationEuler(rotation.x, rotation.y + (turning_speed*sign), rotation.z);
                }
            }

        } else {

            int path_steer = steerToStayOnPath();

            position.x += sin(rotation.y + (turning_speed * path_steer))*speed;
            position.z += cos(rotation.y + (turning_speed * path_steer))*speed;

            setRotationEuler(rotation.x, rotation.y + (turning_speed * path_steer), rotation.z);

        }

    } else {

        // Do nothing... Randomly turn and idle animate

    }

    ground_pos = ground->getHeightInterpolated(position.x, position.z);
    position.y = ground_pos + distance_off_ground;
}

//##################################################################################################
//
// ########  ########     ###    ##      ## #### ##    ##  ######
// ##     ## ##     ##   ## ##   ##  ##  ##  ##  ###   ## ##    ##
// ##     ## ##     ##  ##   ##  ##  ##  ##  ##  ####  ## ##
// ##     ## ########  ##     ## ##  ##  ##  ##  ## ## ## ##   ####
// ##     ## ##   ##   ######### ##  ##  ##  ##  ##  #### ##    ##
// ##     ## ##    ##  ##     ## ##  ##  ##  ##  ##   ### ##    ##
// ########  ##     ## ##     ##  ###  ###  #### ##    ##  ######
//
//##################################################################################################

void Playable::draw(){

    if(health > 0){
        Drawable::draw();


        if(selected || temp_selected ){
            selection_ring->setPosition(glm::vec3(position.x, ground_pos + 0.5, position.z));
            selection_ring->setRotationEuler(glm::vec3(M_PI/2.0f, rotation.y, 0.0));
            selection_ring->draw();
        }
    }
}

string Playable::asJsonString() {
    // No defined format for a playable in json string
    string json_string = "";

    return json_string;
}
