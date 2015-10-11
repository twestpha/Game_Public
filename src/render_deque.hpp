#ifndef RenderDeque_h
#define RenderDeque_h

#include <deque>

#include "screenbuffer.hpp"
#include "framebuffer.hpp"

class RenderDeque {
public:
    RenderDeque();

    void enqueueFramebuffer(Framebuffer& buf, bool clear = true);
    void pushFramebuffer(Framebuffer& buf, bool clear = true);
    void popFramebuffer();

    void drawAllToScreen();

private:

    std::deque<Framebuffer*> framebuffer_deuque;

    Screenbuffer screen;

};

#endif
