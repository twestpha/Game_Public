#ifndef Shadowbuffer_h
#define Shadowbuffer_h

#include "framebuffer.hpp"
#include "core_ui/ui_window.hpp"
#include "profile.hpp"

class Shadowbuffer : public Framebuffer {
public:
    Shadowbuffer() : Shadowbuffer(1.0) {;}
    Shadowbuffer(float);

    void setAsRenderTarget(bool clear = true);
};

#endif
