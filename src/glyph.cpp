#include "glyph.hpp"

Glyph::Glyph(FT_GlyphSlot& glyph, glm::vec2 uv_offset){
    this->advance = glyph->metrics.horiAdvance / 64.0;
    this->width = glyph->metrics.width / 64.0;
    this->height = glyph->metrics.height / 64.0;
    this->bearing_x = glyph->metrics.horiBearingX / 64.0;
    this->bearing_y = glyph->metrics.horiBearingY / 64.0;

    this->uv_offset = uv_offset;
}

double Glyph::getAdvance(){
    return this->advance;
}

double Glyph::getWidth(){
    return this->width;
}

double Glyph::getHeight(){
    return this->height;
}

double Glyph::getBearingX(){
    return this->bearing_x;
}

double Glyph::getBearingY(){
    return this->bearing_y;
}

glm::vec2 Glyph::getUVOffset(){
    return this->uv_offset;
}

std::string Glyph::toString(){
    std::string output = "";

    output += "advance: " + std::to_string(advance) + "\n";
    output += "width : " + std::to_string(width) + "\n";
    output += "height: " + std::to_string(height) + "\n";
    output += "bearing_x: " + std::to_string(bearing_x) + "\n";
    output += "bearing_y: " + std::to_string(bearing_y) + "\n";

    return output;
}
