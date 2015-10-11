#ifndef TextRenderer_h
#define TextRenderer_h

#include "includes/gl.hpp"
#include "includes/glm.hpp"

#include "character_drawable.hpp"
#include "character_mesh.hpp"


#include "window.hpp"
#include "font_sheet.hpp"

class TextRenderer {
public:
    TextRenderer(std::string, GLint);

    void print(int, int, const char*, ...);
    CharacterDrawable* getCharacterBox();

private:
    void drawString(std::string);
    void drawStringInitial(int x, int y, std::string to_draw);

    CharacterMesh* character_mesh;
    CharacterDrawable* character_box;

    std::unordered_map<std::string, glm::vec3> colors;

    GLfloat point;
};

#endif
