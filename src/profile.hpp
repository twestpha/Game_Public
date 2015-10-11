// profile.h
// Trevor Westphal

#ifndef Profile_h
#define Profile_h

#include <tuple>
#include <map>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "debug.hpp"
#include "window.hpp"

class Profile {
public:
	static Profile* getInstance();

	int getFxaaLevel();
	bool getVsync();
	bool getWindowed();
	bool isParticlesOn() {return particles_on;}
	bool isShadowsOn() {return shadows_on;}
	bool isFramebuffersOn() {return framebuffers_on;}
	bool isLightingOn() {return lighting_on;}
	bool isDepthmapOn() { return depthmap_on;}
	int getWindowHeight();
	int getWindowWidth();


	void toggleShadows();
	void toggleVsync();
	void toggleNormals();
	void toggleLighting();
	void toggleParticles();
	void toggleFramebuffers();
	void toggleDepthmap();

	void updateShaderSettings();

private:
	int fxaa_level;
	bool vsync_on;
	bool windowed_on;
	bool particles_on;
	bool shadows_on;
	bool framebuffers_on;
	bool lighting_on;
	bool normals_on;
	bool depthmap_on;
	
	int resolution_index;
	std::map<int, std::tuple<int, int>> resolution_map;

	static Profile* instance;
	Profile();
	void loadSettings();
	std::tuple<char*, char*> split(char*, char);

	GLuint settings_ubo;
};

#endif
