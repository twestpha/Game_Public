#include "text_renderer.hpp"

TextRenderer::TextRenderer(std::string font_filename, GLint point){
    int width = Window::getInstance()->getWidth();
    int height = Window::getInstance()->getHeight();

    this->point = point;

    Shader text_shader("shaders/text.vs",
        "shaders/text.fs");

    FontSheet* font_sheet = new FontSheet(font_filename, point);
    character_box = new CharacterDrawable(text_shader, font_sheet, point);

    // From http://upload.wikimedia.org/wikipedia/commons/9/95/Xterm_color_chart.png
    // at bottom of image
    colors["30"] = glm::vec3(0.0, 0.0, 0.0);
    colors["31"] = glm::vec3(0.8, 0.0, 0.0);
    colors["32"] = glm::vec3(0.0, 0.8, 0.0);
    colors["33"] = glm::vec3(0.98, 0.91, 0.28);
    colors["34"] = glm::vec3(0.0, 0.0, 0.92);
    colors["35"] = glm::vec3(0.8, 0.0, 0.8);
    colors["36"] = glm::vec3(0.0, 0.8, 0.8);
    colors["37"] = glm::vec3(0.9, 0.9, 0.9);

}

void TextRenderer::drawString(std::string to_draw){
    if (to_draw.empty()){
        return;
    }

    // Check if the string starts with an ansi color code
    // The format is \033[1;CCm for color code CC
    // And reset is just \033[0m
    bool is_color_code = to_draw[0] == '\033';
    bool is_reset_code = to_draw.substr(0, 4) == "\033[0m";
    bool is_newline = to_draw[0] == '\n';
    int offset = 0;
    if (is_reset_code){
        character_box->setTextColor(1.0, 1.0, 1.0);
        offset = 3;
    } else if (is_color_code){
        // Remove the color code from the string and
        // set the character color.
        // for now... could fix but ehhhh
        std::string color_code = to_draw.substr(4, 2);
        glm::vec3 color = colors[color_code];
        character_box->setTextColor(color);
        offset = 6;
    } else if (!is_newline) {
        character_box->setCharacter(to_draw[0]);
        character_box->moveToNext();
        character_box->draw();
    }

    drawString(to_draw.substr(1 + offset, to_draw.size() - 1));

}

void TextRenderer::drawStringInitial(int x, int y, std::string to_draw){
    character_box->setPixelPosition(x, y);
    drawString(to_draw);
}

void TextRenderer::print(int x, int y, const char* format, ...){
    // Some real hacky C shit from
    // http://stackoverflow.com/questions/5876646/how-to-overload-printf-or-cout
    char* buffer = new char[1024];

    va_list argument_list;
    va_start(argument_list, format);
    vsprintf(buffer, format, argument_list);
    va_end(argument_list);

    drawStringInitial(x, y, std::string(buffer));

    delete[] buffer;
    buffer = NULL;
}

CharacterDrawable* TextRenderer::getCharacterBox(){
    return character_box;
}
