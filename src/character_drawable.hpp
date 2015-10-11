#ifndef CharacterDrawable_h
#define CharacterDrawable_h

#include <cmath>

#include "flat_drawable.hpp"
#include "character_mesh.hpp"
#include "core_ui/ui_drawable.hpp"
#include "font_sheet.hpp"
#include "glyph.hpp"

class CharacterDrawable : public UIDrawable {
public:
    CharacterDrawable(Shader shader, FontSheet*, GLint);

    void setCharacter(char);
    void setPixelPosition(int, int);
    void setTextColor(glm::vec3 color);
    void setTextColor(float r, float g, float b);
    void setSpacing(float spacing);
    void moveToNext();
private:
    void updateUniformData();

    glm::vec2 uv_offset;
    glm::vec3 text_color;
    float spacing;

    int cursor_x;
    int cursor_y;

    FontSheet* font_sheet;
    Glyph current_glyph;
};

#endif
