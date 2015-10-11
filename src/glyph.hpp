#ifndef Glyph_h
#define Glyph_h

#include "includes/freetype.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

class Glyph {
public:
    Glyph() {;}
    Glyph(FT_GlyphSlot& glyph, glm::vec2 uv_offset);

    double getWidth();
    double getHeight();
    double getBearingX();
    double getBearingY();
    double getAdvance();
    glm::vec2 getUVOffset();

    std::string toString();

private:
    double width;
    double height;
    double bearing_x;
    double bearing_y;
    double advance;

    glm::vec2 uv_offset;
};

#endif
