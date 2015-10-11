#ifndef DrawableSelector_h
#define DrawableSelector_h

#include <vector>

#include "drawable.hpp"
#include "doodad.hpp"
#include "level.hpp"

class DrawableSelector {
public:
    DrawableSelector(Level& level);
    ~DrawableSelector();

    Drawable& getCurrentDrawable();

    void moveToNext();
    void moveToPrevious();

    void handleInput(SDL_Event event);

    int getNumDrawables();

private:
    void addTemplate(Drawable& drawable);
    Drawable& createDefaultDoodad();
    Drawable& createOtherDoodad();
    Drawable& createOtherOtherDoodad();

    // Pointers are owned by DrawableSelector
    vector<Drawable*> templates;

    Level* level;

    int index;
};

#endif
