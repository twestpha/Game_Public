#include "game_map.hpp"

GameMap::GameMap(string map_filename, UnitHolder& units, RenderDeque& render_stack, ResourceLoader& resource_loader) : camera(), ground(), unit_holder(&units), render_stack(&render_stack),  resource_loader(&resource_loader),has_temp_drawable(false),  shadowbuffer(1.0), depthbuffer(1.0), shadow_shader("shaders/shadow.vs", "shaders/shadow.fs"), depth_shader("shaders/depth.vs", "shaders/depth.fs") {

    ifstream map_input(map_filename);
    if (map_input) {
        load(map_input);
    } else {
        loadBlankGameMap();
    }

    initializeGlobalUniforms();

    // // Billboard test for stuff like health bars
    // billboard_test.setScale(5);

}

void GameMap::render(){
    // Render the shadow map into the shadow buffer
    if (Profile::getInstance()->isShadowsOn()){
        renderToShadowMap();
    }

    if (Profile::getInstance()->isDepthmapOn()){
        renderToDepthMap();
    }

    renderAllNoShader();

    // // Temporary for billboard testing
    // glm::vec3 unit_pos = unit_holder->getUnits()[0].getPosition();
    // glm::vec3 new_pos = unit_pos + glm::vec3(0, 10, 0);
    // billboard_test.setPosition(new_pos);
    //
    // // Draw the billboard (should be drawn last)
    // billboard_test.draw();

}

void GameMap::renderToShadowMap(){
    renderAllWithShader(shadow_shader, shadowbuffer);
}

void GameMap::renderToDepthMap(){
    renderAllWithShader(depth_shader, depthbuffer);
}

void GameMap::renderAllNoShader() {
    // Update the global uniforms like the camera position and shadow projections
    updateGlobalUniforms();

    // Draw all the drawables
    for (Drawable* drawable : drawables){
        drawable->draw();
    }

    if (has_temp_drawable) {
        temp_drawable->draw();
    }

    // Draw all the doodads
    for (Doodad& doodad : doodads){
        doodad.draw();
    }

    // Draw all the units
    for (Playable& unit : unit_holder->getUnits()){
        unit.draw();
    }

    // Draw the ground
    ground.draw();

    // Draw all of the particle emitters
    if (Profile::getInstance()->isParticlesOn()){
        for (Emitter* emitter : emitters){
            emitter->draw();
        }
    }

}

void GameMap::renderAllWithShader(Shader& shader, Framebuffer& buf) {
    updateGlobalUniforms();

    render_stack->pushFramebuffer(buf);

    // Draw all the drawables
    for (Drawable* drawable : drawables){
        // Save the shader this drawable is currently using
        Shader& current_shader = drawable->getShader();
        // Set the drawable to render with the shadow shader
        drawable->setShader(shader);
        // Draw the drawable from the light's perspective
        drawable->draw();
        // Reset the drawable's shader to what it was before
        drawable->setShader(current_shader);
    }

    if (has_temp_drawable) {
        // Save the shader this drawable is currently using
        Shader& temp_shader = temp_drawable->getShader();
        // Set the drawable to render with the shadow shader
        temp_drawable->setShader(shader);
        // Draw the drawable from the light's perspective
        temp_drawable->draw();
        // Reset the drawable's shader to what it was before
        temp_drawable->setShader(temp_shader);
    }

    for (Doodad& doodad : doodads){
        // Save the shader this drawable is currently using
        Shader& current_shader = doodad.getShader();
        // Set the drawable to render with the shadow shader
        doodad.setShader(shader);
        // Draw the drawable from the light's perspective
        doodad.draw();
        // Reset the drawable's shader to what it was before
        doodad.setShader(current_shader);
    }

    // Draw all the units
    for (Playable& unit : unit_holder->getUnits()){
        // Save the shader this drawable is currently using
        Shader& current_shader = unit.getShader();
        // Set the drawable to render with the shadow shader
        unit.setShader(shader);
        // Draw the drawable from the light's perspective
        unit.draw();
        // Reset the drawable's shader to what it was before
        unit.setShader(current_shader);
    }


    Shader& current_shader = ground.getShader();
    ground.setShader(shader);
    ground.draw();
    ground.setShader(current_shader);

    render_stack->popFramebuffer();
}

glm::vec3 GameMap::calculateWorldPosition(glm::vec2 screen_point){
    glm::vec3 ray = calculateRay(screen_point);
    glm::vec3 world_point = findMapPointInit(ray, 100);

    return world_point;
}

void GameMap::addDrawable(Drawable& drawable) {
    drawables.push_back(&drawable);
}

void GameMap::setTempDrawable(Drawable& drawable) {
    temp_drawable = &drawable;
    has_temp_drawable = true;
}

void GameMap::placeTempDrawable() {
    Drawable* new_drawable = temp_drawable->clone();
    addDrawable(*new_drawable);
}

void GameMap::removeTempDrawable() {
    temp_drawable = NULL;
    has_temp_drawable = false;
}

string GameMap::asJsonString() {
    // Returns the game map as a json formatted string
    // The game map is essentially the full 'map' file (minus the ResourceLoader), so the format is (template):
    //      camera_json
    //      doodads_json
    //      emitters_json
    //      terrain_json

    string json_string = "";

    // Camera
    json_string += camera.asJsonString() + ",\n";

    // Doodads (idk about all the drawables yet)
    json_string += "\"doodads\": [\n";
    for (Doodad& doodad : doodads) {
        json_string += doodad.asJsonString();
        if (&doodad == &(doodads.back())){
            json_string += "\n";
        } else {
            json_string += ",\n";
        }
    }
    // This is temporary!!!
    json_string.pop_back();

    if (!drawables.empty()) {
        json_string += ",\n";
    }

    for (Drawable* drawable : drawables) {
        json_string += drawable->asJsonString();
        if (drawable == drawables.back()){
            json_string += "\n";
        } else {
            json_string += ",\n";
        }
    }

    // End temporary stuff
    json_string += "],\n";

    // Particle emitters

    // Terrain
    json_string += ground.asJsonString();

    return json_string;
}

Shadowbuffer& GameMap::getShadowbuffer() {
    return shadowbuffer;
}

Shadowbuffer& GameMap::getDepthbuffer() {
    return depthbuffer;
}

Camera& GameMap::getCamera(){
    return camera;
}

Terrain& GameMap::getGround(){
    return ground;
}

void GameMap::load(ifstream& map_input){
    // Read the file into a string
    // http://bit.ly/1aM8TXS
    string map_contents((istreambuf_iterator<char>(map_input)), istreambuf_iterator<char>());

    // Use jsoncpp to parse the file contents
    Json::Value root;
    Json::Reader reader;
    bool parsing_successful = reader.parse(map_contents, root);
    if (!parsing_successful){
        Debug::error("Failed to parse map\n%s", reader.getFormattedErrorMessages().c_str());
        loadBlankGameMap();
    }

    // The filepaths for mesh and textures so we know where to load the files from
    string mesh_path = root["mesh_path"].asString();
    string texture_path = root["texture_path"].asString();

    resource_loader->setMeshPath(mesh_path);
    resource_loader->setTexturePath(texture_path);

    // Create the camera from the json segment
    camera = Camera(root["camera"]);

    // Read in each doodad
    const Json::Value doodads_json = root["doodads"];
    for (const Json::Value& doodad_json : doodads_json){
        // Create doodad from the json segment
        Doodad temp = Doodad(doodad_json, *resource_loader);
        doodads.push_back(temp);
    }

    // Create each particle emitter
    EmitterFactory emitter_factory;
    const Json::Value emitters_json = root["particle_emitters"];
    for (const Json::Value& emitter_json : emitters_json){
        emitters.push_back(emitter_factory.build(emitter_json));
    }

    // Create the ground from the json segment
    ground = Terrain(root["terrain"], *resource_loader);

    // Add all the doodads to drawables

    // Add all the emitters to drawables

}

void GameMap::loadBlankGameMap() {
    // WOAH this is hardcode

    Debug::error("Cannot load map file, please specify an existing map.");

    // // Initialize the resource loader with the base paths
    // resource_loader->setMeshPath("res/models/");
    // resource_loader->setTexturePath("res/textures/");
    //
    // // Initialize camera at the "normal position"
    // glm::vec3 pos(-0, 40, 40);
    // glm::vec3 rot(1.04, 0, 0);
    // camera = Camera(pos, rot);
    //
    // // Initialize the flat terrain
    // Texture blank(glm::vec4(0, 0, 0, 1), 512, 512);
    //
    // // Workaround because the Terrain constructor needs a filename. Can just change it to accept a texture eventually.
    // string blank_filename = "/tmp/blank_heightmap.png";
    // blank.saveAs(blank_filename);
    // ground = Terrain(blank_filename, 10);
    //
    // // Dont need the file anymore
    // remove(blank_filename.c_str());

}

void GameMap::initializeGlobalUniforms(){
    // Create the uniform buffer object for the camera.
    glGenBuffers(1, &camera_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, camera_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, camera_ubo, 0, sizeof(glm::mat4) * 2);

    // Create the uniform buffer object for the shadows.
    glGenBuffers(1, &shadow_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, shadow_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 2, shadow_ubo, 0, sizeof(glm::mat4) * 2);

}

void GameMap::updateGlobalUniforms(){
    // Update the shadow view matrix based on the current
    // camera position
    glm::vec3 light_direction = glm::vec3(1.0f, 2.0f, -0.5f);
    glm::vec3 camera_offset = glm::vec3(camera.getPosition().x, 0, camera.getPosition().z - 40.0);

    // Ideally this shouldn't be created each time
    glm::mat4 depth_view = glm::lookAt(light_direction + camera_offset, camera_offset, glm::vec3(0,1,0));
    glm::mat4 depth_proj = glm::ortho<float>(-60,60,-65, 60,-40,40);

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = camera.getProjectionMatrix();

    // Attach the shadow texture to location 4
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, shadowbuffer.getTexture().getGLId());

    // Put the data in the UBO.
    glBindBuffer(GL_UNIFORM_BUFFER, camera_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
        glm::value_ptr(view));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
        glm::value_ptr(proj));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, shadow_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
        glm::value_ptr(depth_view));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
        glm::value_ptr(depth_proj));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

}

glm::vec3 GameMap::getIntersection(glm::vec3 line, float height){
    glm::vec3 line_start = camera.getPosition();
    glm::vec3 normal = glm::vec3(0.0, 1.0, 0.0);
    glm::vec3 plane = glm::vec3(0.0, height, 0.0);

    // To find the point on the plane of clicking (defined by mouse_plane)
    // From http://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection
    float d = glm::dot((plane - line_start), normal) / glm::dot(line, normal);
    glm::vec3 intersection = d * line + line_start;

    return intersection;
}

glm::vec3 GameMap::calculateRay(glm::vec2 screen_point){
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = camera.getProjectionMatrix();

    glm::vec3 ray = glm::vec3(glm::inverse(proj) * glm::vec4(screen_point, -1.0, 1.0));
    ray.z = -1.0;
    ray = glm::vec3(glm::inverse(view) * glm::vec4(ray, 0.0));
    ray = glm::normalize(ray);
    return ray;
}

std::tuple<float, float, glm::vec3> GameMap::findMapPoint(glm::vec3 ray, int steps, float bottom, float top){
    // Search idea from http://bit.ly/1Jyb6pa
    glm::vec3 world_point;

    float height = top;
    float increment = (top - bottom) / (float)steps;

    float bottom_bound = bottom;
    float top_bound = top;
    for (int i = 0; i <= steps; ++i){
        top_bound = height;
        height = top - i * increment;
        world_point = getIntersection(ray, height);

        float terrain_height = ground.getHeightInterpolated(world_point.x, world_point.z);

        if (height > terrain_height){
            top_bound = height;
        } else if (height < terrain_height){
            bottom_bound = height;
            break;
        }
    }

    return std::make_tuple(bottom_bound, top_bound, world_point);
}

glm::vec3 GameMap::findMapPointInit(glm::vec3 ray, int steps){
    // Ideal mouse point search algorithm
    // Do a low resolution pass of the planes and find
    // which planes the point is between. Then repeat
    // the process but this time only between those two
    // planes. Do this over and over until the mouse point
    // is valid.

    float top = ground.getMaxHeight() + 1.0;
    float bottom = 0;
    glm::vec3 world_point;

    std::tuple<float,float, glm::vec3> bounds;
    for (int i = 0; i < 10; ++i){
        bounds = findMapPoint(ray, steps, bottom, top);
        bottom = std::get<0>(bounds);
        top = std::get<1>(bounds);
        world_point = std::get<2>(bounds);
    }

    return world_point;

}
