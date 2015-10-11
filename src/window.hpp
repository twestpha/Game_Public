#ifndef Window_h
#define Window_h

#include "includes/gl.hpp"
#include "includes/sdl.hpp"
#include "includes/glm.hpp"

#include <SOIL.h>
#include <ctime>
#include <string>

#include "debug.hpp"
#include "profile.hpp"

class Window{
public:
    static Window* getInstance();

    void initializeWindow();
    void display();
    void close();
    void takeScreenshot();

    void requestClose();
    bool shouldClose();

    void centerMouse();

    void setWidth(int w);
    void setHeight(int h);
    void setFullscreen(bool f);
    void setVsync(bool);

    int getWidth();
    int getHeight();

    float getWidthScale(){return width_scale;}
    float getHeightScale(){return height_scale;}

private:

    int width;
    int height;
    bool fullscreen;

    float width_scale;
    float height_scale;

    int requested_width;
    int requested_height;

    bool should_close;

    SDL_Window* sdl_window;
    SDL_GLContext gl_context;

    static Window* instance;
    Window();

};

#endif
