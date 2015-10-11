#include "screenbuffer.hpp"

Screenbuffer::Screenbuffer(){
    framebuffer = 0;
    width = Window::getInstance()->getWidth();
    height = Window::getInstance()->getHeight();
}
