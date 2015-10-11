// playable.h
// Trevor Westphal

#ifndef Playable_h
#define Playable_h

#include "includes/gl.hpp"

#include <vector>
#include <string>
#include <queue>

#include "pugixml.hpp" // PUGI xml library


#include "doodad.hpp"
#include "mesh.hpp"
#include "drawable.hpp"
#include "terrain.hpp"
#include "game_clock.hpp"
#include "pathfinder.hpp"

class Playable : public Drawable {
public:
	// Not a complete list
	enum class Order{ MOVE, MOVE_TARGET, ATTACK, ATTACK_MOVE, ATTACK_TARGET, HOLD_POSITION, STOP };
	enum class PlayableAttribute{ MASSIVE, ARMORED, ARMY, WORKER, FLYING, INVULNERABLE, MECHANICAL };

	Playable();
	Playable(Mesh&, Shader& shader, glm::vec3, GLfloat);
	Drawable* clone();

	void update(Terrain*, std::vector<Playable*>*);
	void loadFromXML(std::string filepath);

	void draw();

	void select();
	void deSelect();
	void tempSelect();
	void tempDeSelect();

	void receiveOrder(Playable::Order, glm::vec3, bool, std::vector<glm::vec3>, Playable*);

	void holdPosition();
	void stop();

	void setFlying(bool flying);

	bool isEnemy(int);

	bool isSelected(){ return selected; }
	bool isTempSelected(){ return temp_selected; }

	string asJsonString();

	float getRadius(){ return radius; }

	int getTeam(){return team_number;}

	// Temporary - REMOVE ME LATER
	void setTeam(int t);


private:

	//################################
	// Non-game Variables (Private)
	//################################

	// Order queues
	std::queue<std::tuple<Playable::Order, glm::vec3>> order_queue;
	std::queue<Playable*> target_queue;

	// Tracking the first turn
	bool first_step_since_order;
	bool turning_during_first_step;

	// Current/Old Target Location, Direction, and Order
	glm::vec3 target_position;
	float target_direction;
	Playable::Order target_order;
	glm::vec3 old_target_position;

	// Selection
	static Doodad* selection_ring;

	// Attacking
	bool enemy_in_sight_range;
	bool has_been_given_attack_order;

	// Scanning stuff
	std::vector<Playable*> attackable_units;

	//################################
	// In-game Variables (Private)
	//################################

	// Type
	std::string unit_type;

	// Team
	// Stuff for now
	// 0 - reserved for computer mobs
	// 1 - Player
	// . - Computer
	int team_number;
	int attack_priority;

	// Movement
	float speed;
	float acceleration;
	float turning_speed;
	float radius;
	float sight_radius;
	float distance_off_ground;
	float ground_pos;

	// Leveling
	int level;
	int per_level_health_boost;
	int per_level_strength_boost;

	// Selection Status
	bool selected;
	bool temp_selected;

	// Health
	int max_health;
	int health;
	int healing_rate;

	std::vector<PlayableAttribute> attributes;

	float last_attack_timestamp;

	// REMOVE WE WHEN WEAPON IS IMPLEMENTED
	float weapon_cooldown;
	int weapon_damage;
	float weapon_range;

	// Not implemented yet
	// Weapon* weapon
	// Ability* ability

	//################################
	// Orders (Private)
	//################################

	static Playable::Order determineBodyOrder(Playable::Order, bool);
	static Playable::Order determineLastOrder(Playable::Order, bool);

	//################################
	// Location (Private)
	//################################

	bool atTargetPosition();
	static float getDistance(float, float, float, float);
	void setTargetPositionAndDirection(glm::vec3);
	float getCurrentTargetDirection();

	//################################
	// Combat (Private)
	//################################

	Playable* nearest_enemy_attack;
	Playable* nearest_friendly_hurt;
	Playable* nearest_friendly_town_hall;
	Playable* nearest_resource;

	void attack(Playable*);
	void takeDamage(int);
	Playable* getUnitToAttack(std::vector<Playable*>*);

	//################################
	// Steering (Private)
	//################################

	int steerToStayOnPath();
	int steerAwayFromUnit(Playable*);
	int steerAwayFromObstacle(Terrain*);
	static float distanceFromPointToLine(glm::vec2, glm::vec2, glm::vec2);

	//################################
	// Other Stuff (Private)
	//################################

	void scanUnits(std::vector<Playable*>*);
	void updateUniformData();

};

#endif
