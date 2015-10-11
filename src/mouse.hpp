#ifndef Mouse_h
#define Mouse_h

#include <vector>



#include "core_ui/ui_window.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "terrain.hpp"

class Mouse : public UIDrawable {
public:
    static Mouse* getInstance();

    enum class cursorType { CURSOR, SELECTION, COMMAND, UP, RIGHT, DOWN, LEFT, UP_LEFT, UP_RIGHT, DOWN_RIGHT, DOWN_LEFT};
    enum Button { LEFT, RIGHT, MIDDLE };

    void draw();
    void setCursorSprite(cursorType);
    void setHovering();
    bool isHovering();
    bool isPressed(Button);

    glm::vec2 getGLPosition();
    glm::vec2 getScreenPosition();

private:

    bool hovering;

    static Mouse* instance;
    Mouse();

    cursorType current_type;

    std::vector<GLuint> mouse_sprites;

    glm::mat3 mouse_projection;
    glm::mat3 inv_mouse_projection;
};

#endif
