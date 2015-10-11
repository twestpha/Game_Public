// profile.cpp
// Trevor Westphal

#include "profile.hpp"

Profile* Profile::instance;

Profile* Profile::getInstance(){
	if(instance){
		return instance;
	} else {
		instance = new Profile();
		return instance;
	}
}

Profile::Profile(){

    // Setup the resolutions
    resolution_map[0] = std::make_tuple( 800,  600);
    resolution_map[1] = std::make_tuple(1024,  768);
	resolution_map[2] = std::make_tuple(1366,  768);
	resolution_map[3] = std::make_tuple(1600,  900);
    resolution_map[4] = std::make_tuple(1920, 1080);
    resolution_map[5] = std::make_tuple(1920, 1200);
    resolution_map[6] = std::make_tuple(2560, 1440);
    resolution_map[7] = std::make_tuple(2560, 1600);
    resolution_map[8] = std::make_tuple(3840, 2160);
    resolution_map[9] = std::make_tuple(5120, 2880);

	loadSettings();
}

int Profile::getFxaaLevel(){
	if (framebuffers_on){
		return fxaa_level;
	} else {
		return 0;
	}
}

bool Profile::getVsync(){
	return vsync_on;
}

void Profile::toggleShadows(){
	shadows_on = !shadows_on;
	updateShaderSettings();
}

void Profile::toggleNormals(){
	normals_on = !normals_on;
	updateShaderSettings();
}

void Profile::toggleLighting(){
	lighting_on = !lighting_on;
	updateShaderSettings();
}

void Profile::toggleParticles(){
	particles_on = !particles_on;
	updateShaderSettings();
}

void Profile::toggleFramebuffers(){
	framebuffers_on = !framebuffers_on;
	updateShaderSettings();
}

void Profile::toggleVsync(){
	vsync_on = !vsync_on;
	Window::getInstance()->setVsync(vsync_on);
}

void Profile::toggleDepthmap() {
	depthmap_on = !depthmap_on;
}

bool Profile::getWindowed(){
	return windowed_on;
}

void Profile::loadSettings(){
	char buffer[128];

	const char* settings_filename = "settings/settings.conf";

    FILE * ifile;
    ifile = fopen(settings_filename, "r");

    if(ifile == NULL){
        Debug::error("Could not open configuration file '%s'.\n",
			settings_filename);
        return;
    }

     while(! feof(ifile)){
     	if(fgets(buffer, 128, ifile) == NULL){
            // Can't read into buffer
            break;
        }

        // skip commented lines
        if(buffer[0] != '#'){
        	std::tuple<char*, char*> arguments = split(buffer, '=');

        	char* keyword =  std::get<0>(arguments);
        	char* value = std::get<1>(arguments);

        	if(strcmp(keyword, "fxaa") == 0){
        		fxaa_level = atoi(value);
        	} else if(strcmp(keyword, "vsync") == 0){
        		vsync_on = (strcmp(value, "true") == 0);
        	} else if(strcmp(keyword, "windowed") == 0){
        		windowed_on = (strcmp(value, "true") == 0);
        	} else if(strcmp(keyword, "resolution") == 0){
                resolution_index = atoi(value);
                if(resolution_map.count(resolution_index) == 0){
                    // Warning: this does not notify the user
                    resolution_index = 0;
                }
        	} else if(strcmp(keyword, "texturedetail") == 0){
                // TODO implement texture detail system
            } else if(strcmp(keyword, "particles") == 0){
				particles_on = (strcmp(value, "true") == 0);
			} else if(strcmp(keyword, "shadows") == 0){
				shadows_on = (strcmp(value, "true") == 0);
			} else if(strcmp(keyword, "framebuffers") == 0){
				framebuffers_on = (strcmp(value, "true") == 0);
			} else if(strcmp(keyword, "lighting") == 0){
				lighting_on = (strcmp(value, "true") == 0);
			} else if(strcmp(keyword, "normals") == 0){
				normals_on = (strcmp(value, "true") == 0);
			} else if(strcmp(keyword, "depthmap") == 0){
				depthmap_on = (strcmp(value, "true") == 0);
			}
        }
    }

}

void Profile::updateShaderSettings(){
	// Usually OpenGL code shouldn't be this high up but this is for
	// sending settings to the shader
	glGenBuffers(1, &settings_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, settings_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(GLfloat) * 3, NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 4, settings_ubo, 0, sizeof(GLfloat) * 3);

	// For some reason you can't pass a bool through a UBO.
	// So just represent it as a float.
	GLfloat lighting = (lighting_on) ? 1.0f : 0.0f;
	GLfloat shadows = (shadows_on) ? 1.0f : 0.0f;
	GLfloat normals = (normals_on) ? 1.0f : 0.0f;

	glBindBuffer(GL_UNIFORM_BUFFER, settings_ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLfloat), &lighting);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat), sizeof(GLfloat), &shadows);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat) * 2, sizeof(GLfloat), &normals);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

std::tuple<char*, char*> Profile::split(char* str, char key){
	bool found_key = false;

	int str_count = 0;
	int str_2_count = 0;

	char* sub_str_1 = new char[32];
	char* sub_str_2 = new char[32];

	for(int i = 0; str[i] != 0; i++){
		if(str[i] == key){
			found_key = true;
			i++;
		}
		if(!found_key){
			str_count++;
			sub_str_1[i] = str[i];
		} else {
			sub_str_2[str_2_count] = str[i];
			str_2_count++;
		}
	}

	sub_str_1[str_count] = 0;
	sub_str_2[str_2_count-1] = 0;

	return std::make_tuple(sub_str_1, sub_str_2);
}

int Profile::getWindowHeight(){
    return std::get<1>(resolution_map[resolution_index]);
}

int Profile::getWindowWidth(){
    return std::get<0>(resolution_map[resolution_index]);
}
