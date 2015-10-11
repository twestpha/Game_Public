// pathfinder.cpp
// Trevor Westphal

#include "pathfinder.hpp"

PathFinder::PathFinder(Terrain& ground) : ground(&ground) {
	allocateArray(ground);
}

void PathFinder::allocateArray(Terrain& ground){
	depth = ground.getDepth();
	width = ground.getWidth();

	node_state_array = new int*[width];
    for(int i = 0; i < width; ++i){
        node_state_array[i] = new int[depth];
        for(int j = 0; j < depth; ++j){
        	node_state_array[i][j] = UNVISITED;
        }
    }
}

std::vector<glm::vec3> PathFinder::find_path(float start_x, float start_y, float target_x, float target_y, float radius){
	// No A* search if there is a straight line from start to target
	if( canPathOnLine(ground, start_x, start_y, target_x, target_y, radius) ){
		std::vector<glm::vec3> temp;
		temp.push_back(glm::vec3(target_x, 0.0f, target_y));
		return temp;
	}

	int start_x_int = int(start_x);
	int start_y_int = int(start_y);
	int target_x_int = int(target_x);
	int target_y_int = int(target_y);

	int x_offset = width/2;
	int y_offset = depth/2;

	std::vector<Node*> visited_nodes;											    // The set of nodes already evaluated.

	std::priority_queue<Node*, std::vector<Node*>, LessThanByGScore> frontier_nodes;// The set of tentative nodes to be evaluated...

	Node *start_node = new Node(start_x_int, start_y_int, 0.0f);					// ...initially containing the start node
	frontier_nodes.push(start_node);												// Cost from start along best known path (included)

	std::unordered_map<Node*, Node*> parent_of;										// The map of navigated nodes.

	int count = 0;
	int index_x, index_y;
	float temp_g_score;
	float distance_to_goal;
	bool can_move_to;
	bool is_not_visited;
	bool is_not_in_frontier;
	int node_state;

	// Closest node is buggy
	Node* closest_node;
	float closest_node_distance = 99999.0f;

	while(! frontier_nodes.empty()){
		count ++;
		Node* current_node = frontier_nodes.top();

        if(current_node->x == target_x_int && current_node->y == target_y_int){
        	// Clear out the old visited nodes
        	while(! frontier_nodes.empty()){
        		node_state_array[frontier_nodes.top()->x + x_offset][frontier_nodes.top()->y + y_offset] = UNVISITED;
        		frontier_nodes.pop();
        	}
        	for(int i = 0; i < visited_nodes.size(); ++i){
        		node_state_array[visited_nodes[i]->x + x_offset][visited_nodes[i]->y + y_offset] = UNVISITED;
        	}
        	return reconstruct_path(ground, parent_of, current_node, radius);
        }

		distance_to_goal = distance_between(current_node->x, current_node->y, target_x, target_y);
        if(distance_to_goal < closest_node_distance){
        	closest_node_distance = distance_to_goal;
        	closest_node = current_node;
        }

        frontier_nodes.pop();

        visited_nodes.push_back(current_node);
        node_state_array[current_node->x + x_offset][current_node->y + y_offset] = VISITED;

        std::vector<Node*> neighbor_nodes = getNeighborNodes(current_node);

        for(int i = 0; i < neighbor_nodes.size(); ++i){

        	Node* n = neighbor_nodes[i];
        	index_x = n->x + x_offset;
        	index_y = n->y + y_offset;

        	temp_g_score = current_node->g + distance_between(current_node->x, current_node->y, n->x, n->y);

        	can_move_to = checkCircle(ground, n->x, n->y, radius);

        	is_not_visited = false;
        	is_not_in_frontier = false;

        	if(index_y > 0 && index_y < depth && index_x > 0 && index_x < width){
        		node_state = node_state_array[n->x + x_offset][n->y + y_offset];
        		is_not_visited = node_state == UNVISITED;
        		is_not_in_frontier = node_state != IN_FRONTIER;
        	}

        	if(is_not_visited && can_move_to && (is_not_in_frontier || temp_g_score < n->g)){
        		parent_of[n] = current_node;
        		n->g = temp_g_score;

        		if(is_not_in_frontier){
        			frontier_nodes.push(n);
    				node_state_array[n->x + x_offset][n->y + y_offset] = IN_FRONTIER;
        		}
        	}
        }
 	}

 	// Clear out the old visited nodes
	for(int i = 0; i < visited_nodes.size(); ++i){
		node_state_array[visited_nodes[i]->x + x_offset][visited_nodes[i]->y + y_offset] = 0;
	}
	return reconstruct_path(ground, parent_of, closest_node, radius);
}

float PathFinder::distance_between(int current_x, int current_y, int target_x, int target_y){
	int x_delta = abs(current_x - target_x);
	int y_delta = abs(current_y - target_y);

	return sqrt(float(x_delta*x_delta + y_delta*y_delta));
}


float PathFinder::heuristic_estimate(int a, int b, int c, int d){
	// Heuristic function
	return distance_between(a, b, c, d);
}

std::vector<glm::vec3> PathFinder::reconstruct_path(Terrain *ground, std::unordered_map<Node*, Node*> parent_of, Node* origin, float radius){

	std::vector<Node*> temp;
	std::vector<glm::vec3> final;

	if(! origin){
		return final;
	}

	temp.push_back(origin);

	while(parent_of[origin]){
		origin = parent_of[origin];
		temp.push_back(origin);
	}

	Node* anchor = temp[0];
	Node* current = 0;
	Node* previous = 0;

	for(int i = 0; i < temp.size(); ++i){
		// Get the current node
		current = temp[i];

		// see if we can path between the anchor and the current
		bool line_between = (i == 0) || (canPathOnLine(ground, anchor->x, anchor->y, current->x, current->y, radius));

		if(line_between){
			previous = current;
		} else {
			final.insert(final.begin(), glm::vec3(previous->x, 0.0f, previous->y));
			anchor = previous;
			previous = current;
		}
	}

	return final;
}

std::vector<Node*> PathFinder::getNeighborNodes(Node *current_node){
	std::vector<Node*> temp;
	// could do up/down/right/left only
	temp.push_back(new Node(current_node->x + 1, current_node->y - 1, current_node->g + 1.4f));
	temp.push_back(new Node(current_node->x + 1, current_node->y + 0, current_node->g + 1.0f));
	temp.push_back(new Node(current_node->x + 1, current_node->y + 1, current_node->g + 1.4f));
	temp.push_back(new Node(current_node->x + 0, current_node->y - 1, current_node->g + 1.0f));
	temp.push_back(new Node(current_node->x + 0, current_node->y + 1, current_node->g + 1.0f));
	temp.push_back(new Node(current_node->x - 1, current_node->y - 1, current_node->g + 1.4f));
	temp.push_back(new Node(current_node->x - 1, current_node->y + 0, current_node->g + 1.0f));
	temp.push_back(new Node(current_node->x - 1, current_node->y + 1, current_node->g + 1.4f));
	return temp;
}

bool PathFinder::canPathOnLine(Terrain* ground, float x1, float y1, float x2, float y2, float radius){

 	// http://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C.2B.2B
	// Bresenham's line algorithm

	const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));

	if(steep){
	  std::swap(x1, y1);
	  std::swap(x2, y2);
	}

	if(x1 > x2){
	  std::swap(x1, x2);
	  std::swap(y1, y2);
	}

	const float dx = x2 - x1;
	const float dy = fabs(y2 - y1);

	float error = dx / 2.0f;
	const int ystep = (y1 < y2) ? 1 : -1;
	int y = (int)y1;

	const int maxX = int(x2);

	int x_path, y_path;

	for(int x = int(x1); x < maxX; x++){

		if(steep){
		  x_path = y;
		  y_path = x;
		} else {
		  x_path = x;
		  y_path = y;
		}

		if(checkCircle(ground, x_path, y_path, radius) == false){
			return false;
		}

		error -= dy;
		if(error < 0) {
		  y += ystep;
		  error += dx;
		}
	}

	return true;
}

bool PathFinder::checkCircle(Terrain* ground, int x0, int y0, int radius){
	// http://rosettacode.org/wiki/Bitmap/Midpoint_circle_algorithm#C
	// Circle drawing algorithm

  	int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    bool radius_path = true;

    radius_path &= ground->canPath(x0, y0 + radius);
    radius_path &= ground->canPath(x0, y0 - radius);
    radius_path &= ground->canPath(x0 + radius, y0);
    radius_path &= ground->canPath(x0 - radius, y0);

    while(x < y)
    {
        if(f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x + 1;

        radius_path &= ground->canPath(x0 + x, y0 + y);
        radius_path &= ground->canPath(x0 - x, y0 + y);
        radius_path &= ground->canPath(x0 + x, y0 - y);
        radius_path &= ground->canPath(x0 - x, y0 - y);
        radius_path &= ground->canPath(x0 + y, y0 + x);
        radius_path &= ground->canPath(x0 - y, y0 + x);
        radius_path &= ground->canPath(x0 + y, y0 - x);
        radius_path &= ground->canPath(x0 - y, y0 - x);

        if(!radius_path){
        	return false;
        }
    }

    return true;
}
