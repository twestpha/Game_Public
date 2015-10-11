#include "unit_holder.hpp"

UnitHolder::UnitHolder(){

}

void UnitHolder::addUnit(Playable& unit){
    units.push_back(unit);
}

vector<Playable>& UnitHolder::getUnits(){
    return units;
}

void UnitHolder::populate(ResourceLoader& resource_loader) {
    // Creation of test playables
    # warning Move mesh loading into playable loading
    Mesh& playable_mesh_ref = resource_loader.loadMesh("small_airship.dae");
    Shader& playable_shader_ref = resource_loader.loadShader("shaders/doodad.vs",
        "shaders/doodad.fs");
    float playable_scale = 1.0f;

    for(int i = 0; i < 1; ++i){
        for(int j = 0; j < 1; ++j){
            glm::vec3 playable_position = glm::vec3(-10 - 3.0f*i, 0.0f, 5 - 3.0f*j);
            Playable temp(playable_mesh_ref, playable_shader_ref, playable_position, playable_scale);
            temp.loadFromXML("res/units/airship.xml");
            temp.setScale(0.8);
            temp.setFlying(true);

            Texture& diff_ref = resource_loader.loadTexture("small_airship.png");
            temp.setDiffuse(diff_ref);
            if (rand() % 2){
                temp.setTeam(1);
            } else {
                temp.setTeam(2);
            }

            addUnit(temp);
        }
    }
}
