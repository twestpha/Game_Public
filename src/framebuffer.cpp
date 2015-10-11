#include "framebuffer.hpp"

Framebuffer::Framebuffer(){
    this->window = Window::getInstance();

    // Create frame buffer
    glGenFramebuffers(1, &framebuffer);

    setupFramebufferTexture(GL_RGBA, 1);

    setupDepthStencilBuffer();

    Shader framebuffer_shader;
    // Load framebuffer shader
    if (Profile::getInstance()->getFxaaLevel()){
        framebuffer_shader = Shader("shaders/flat_drawable_noflip.vs",
            "shaders/framebuffer_fxaa.fs");
    } else {
        framebuffer_shader = Shader("shaders/flat_drawable_noflip.vs",
            "shaders/flat_drawable.fs");
    }

    addShaderPass(framebuffer_shader);

    // Create the window to draw the framebuffer onto
    framebuffer_window = new FlatDrawable(framebuffer_shader, 1.0, 1.0, glm::vec2(0.0, 0.0));
    framebuffer_window->attachTexture(framebuffer_texture);

}

void Framebuffer::setAsRenderTarget(bool clear){
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, width, height);

    if (clear) {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

void Framebuffer::draw(){
    // If this is an empty framebuffer then don't draw it
    if (framebuffer_window != NULL){
        // Do a draw pass for each of the shaders.
        for (GLuint shader : shaders){
            framebuffer_window->setShader(shader);
            framebuffer_window->draw();
        }
    }
}

void Framebuffer::addShaderPass(Shader shader){
    // Add a shader to the drawing passes. The pass
    // is done in the same order as the shaders were
    // added in
    shaders.push_back(shader.getGLId());
}

void Framebuffer::setupFramebufferTexture(GLuint format, float up_sample) {
    // Create texture for framebuffer (should probably be a constructor in texture)
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    this->width = up_sample * Window::getInstance()->getWidth();
    this->height = up_sample * Window::getInstance()->getHeight();

    glTexImage2D(GL_TEXTURE_2D, 0, format, this->width, this->height, 0, format,
        GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    framebuffer_texture = Texture(texture_id);

    //  Bind framebuffer and link texture to it
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        framebuffer_texture.getGLId(), 0);
}

void Framebuffer::setupDepthStencilBuffer() {
    // Add the depth buffer to the framebuffer
    GLuint rboDepthStencil;
    glGenRenderbuffers(1, &rboDepthStencil);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, rboDepthStencil);
}
