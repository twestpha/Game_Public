#include "drawable_selector.hpp"

DrawableSelector::DrawableSelector(Level& level) : level(&level) {
    index = 0;

    addTemplate(createDefaultDoodad());
    addTemplate(createOtherDoodad());
    addTemplate(createOtherOtherDoodad());

}

DrawableSelector::~DrawableSelector() {
    for (int i = 0; i < templates.size(); ++i){
        delete templates[i];
        templates[i] = NULL;
    }

    templates.clear();
}

void DrawableSelector::moveToNext() {
    ++index;
    if (index > getNumDrawables() - 1) {
        index = 0;
    }
}

void DrawableSelector::moveToPrevious() {
    --index;
    if (index < 0) {
        index = getNumDrawables() - 1;
    }
}

void DrawableSelector::handleInput(SDL_Event event) {
    SDL_Keycode keycode = event.key.keysym.sym;
    SDL_Scancode scancode = event.key.keysym.scancode;

    switch(event.type){
        case SDL_KEYDOWN:
            if (scancode == SDL_SCANCODE_RIGHT) {
                moveToNext();
            } else if (scancode == SDL_SCANCODE_LEFT) {
                moveToPrevious();
            }
        break;

        default:
        break;
    }
}

Drawable& DrawableSelector::getCurrentDrawable() {
    return *(templates[index]);
}

int DrawableSelector::getNumDrawables() {
    return templates.size();
}

void DrawableSelector::addTemplate(Drawable& drawable) {
    templates.push_back(&drawable);
}

Drawable& DrawableSelector::createDefaultDoodad() {

    ResourceLoader& resource_loader = this->level->getResourceLoader();
    resource_loader.setMeshPath("res/models/");
    resource_loader.setTexturePath("res/textures/");

    Mesh& mesh_ref = resource_loader.loadMesh("rock1.dae");
    Drawable* new_drawable = new Doodad(mesh_ref, resource_loader);
    new_drawable->setRotationEuler(M_PI / 2.0, 0, 0);
    new_drawable->setScale(3);

    Texture& diff_ref = resource_loader.loadTexture("rock1.png");
    new_drawable->setDiffuse(diff_ref);

    return *new_drawable;
}

Drawable& DrawableSelector::createOtherDoodad() {

    ResourceLoader& resource_loader = this->level->getResourceLoader();
    resource_loader.setMeshPath("res/models/");
    resource_loader.setTexturePath("res/textures/");

    Mesh& mesh_ref = resource_loader.loadMesh("fence.dae");
    Drawable* new_drawable = new Doodad(mesh_ref, resource_loader);
    new_drawable->setRotationEuler(M_PI / 2.0, 0, 0);
    new_drawable->setScale(3);

    Texture& diff_ref = resource_loader.loadTexture("fence_diff.png");
    new_drawable->setDiffuse(diff_ref);

    return *new_drawable;
}

Drawable& DrawableSelector::createOtherOtherDoodad() {

    ResourceLoader& resource_loader = this->level->getResourceLoader();
    resource_loader.setMeshPath("res/models/");
    resource_loader.setTexturePath("res/textures/");

    Mesh& mesh_ref = resource_loader.loadMesh("tree_stump.dae");
    Drawable* new_drawable = new Doodad(mesh_ref, resource_loader);
    new_drawable->setRotationEuler(M_PI / 2.0, 0, 0);
    new_drawable->setScale(3);

    Texture& diff_ref = resource_loader.loadTexture("tree_stump.png");
    new_drawable->setDiffuse(diff_ref);

    return *new_drawable;
}
