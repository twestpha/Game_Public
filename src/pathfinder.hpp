// pathfinder.h
// Trevor Westphal

#ifndef PathFinder_h
#define PathFinder_h

#include "includes/gl.hpp"

#include <unordered_map>  // std::unordered_map
#include <queue>          // std::priority_queue
#include <vector>         // std::vector
#include <algorithm>	  // std::swap

#include "terrain.hpp"

using namespace std;

class Node{
public:
	Node(int x, int y, int g){
		this->x = x;
		this->y = y;
		this->g = g;
	}

	int x;
	int y;
	float g;
};

struct LessThanByGScore{
  bool operator()(const Node* lhs, const Node* rhs) const {
  	// Reverse ordering for lowest
    return lhs->g > rhs->g;
  }
};

class PathFinder {
public:
	PathFinder(Terrain&);
	vector<glm::vec3> find_path(float, float, float, float, float);
	
private:
	void allocateArray(Terrain&);

	float distance_between(int, int, int, int);
	float heuristic_estimate(int, int, int, int);
	vector<glm::vec3> reconstruct_path(Terrain*, std::unordered_map<Node*, Node*>, Node*, float);
	std::vector<Node*> getNeighborNodes(Node*);
	bool canPathOnLine(Terrain*, float, float, float, float, float);
	bool checkCircle(Terrain*, int, int, int);

	int **node_state_array;
	int depth;
	int width;

	Terrain* ground;

	const int UNVISITED = 0;
	const int IN_FRONTIER = 1;
	const int VISITED = 2;
};

#endif
