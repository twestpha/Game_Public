#include "character_drawable.hpp"

CharacterDrawable::CharacterDrawable(Shader shader, FontSheet* font_sheet, GLint point)
    :UIDrawable(shader.getGLId(), font_sheet->getTexture()) {
        mesh = CharacterMesh::getInstance();
        this->mesh->attachGeometryToShader(shader);
        uv_offset = glm::vec2();

        height_pixels = point;
        width_pixels = point;
        spacing = 1;

        setTextColor(1.0, 1.0, 1.0);

        updateDimensions();
        setGLPosition(getGLPosition());

        this->font_sheet = font_sheet;

}

void CharacterDrawable::setPixelPosition(int x, int y){
    // Set the cursor position
    cursor_x = x;
    cursor_y = y;

    setGLPosition(getGLPosition());
}

void CharacterDrawable::setTextColor(glm::vec3 color){
    text_color = color;
}

void CharacterDrawable::setTextColor(float r, float g, float b){
    setTextColor(glm::vec3(r, g, b));
}

void CharacterDrawable::setSpacing(float spacing){
    this->spacing = spacing;
}

void CharacterDrawable::moveToNext(){
    cursor_x += current_glyph.getAdvance() * spacing;
    setGLPosition(getGLPosition());
}

void CharacterDrawable::setCharacter(char to_render){
    // Get the glyph in the font sheet that corresponds to the current
    // character to be drawn.
    current_glyph = font_sheet->getGlyph(to_render);

    // The actual x and y position that the character will
    // be drawn to is dependent on the glyph.
    x_pixels = cursor_x + current_glyph.getBearingX();
    y_pixels = cursor_y - current_glyph.getBearingY() + font_sheet->getMaxYBearing();
    // Update the pixel perfect position
    setGLPosition(getGLPosition());

    // Update the uv offset for the character in the font sheet texture.
    uv_offset = current_glyph.getUVOffset();

}

void CharacterDrawable::updateUniformData(){
    glUniform2f(glGetUniformLocation(shader.getGLId(), "uv_offset"),
        uv_offset.x, uv_offset.y);
    glUniform3f(glGetUniformLocation(shader.getGLId(), "textColor"),
        text_color.x, text_color.y, text_color.z);
    UIDrawable::updateUniformData();
}
