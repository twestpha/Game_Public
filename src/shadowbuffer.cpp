#include "shadowbuffer.hpp"


Shadowbuffer::Shadowbuffer(float up_sample){
    // Create frame buffer
    glGenFramebuffers(1, &framebuffer);

    setupFramebufferTexture(GL_RGBA, up_sample);
    setupDepthStencilBuffer();

    // Create the window to draw the framebuffer onto
    framebuffer_window = new FlatDrawable(0.5, 0.5, glm::vec2(0.5, -0.5));
    framebuffer_window->attachTexture(framebuffer_texture);
}

void Shadowbuffer::setAsRenderTarget(bool clear){
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, width, height);

    if (clear) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}
