#include "render_deque.hpp"

RenderDeque::RenderDeque() : screen() {

}

void RenderDeque::enqueueFramebuffer(Framebuffer& buf, bool clear) {
    buf.setAsRenderTarget(clear);
    framebuffer_deuque.push_front(&buf);
}

void RenderDeque::pushFramebuffer(Framebuffer& buf, bool clear) {
    buf.setAsRenderTarget(clear);
    framebuffer_deuque.push_front(&buf);
}

void RenderDeque::popFramebuffer() {
    framebuffer_deuque.pop_front();

    // Set the new top one as the render target
    if (!framebuffer_deuque.empty()){
        framebuffer_deuque.front()->setAsRenderTarget();
    }
}

void RenderDeque::drawAllToScreen() {
    // Goes through the render deque and draws each framebuffer to the screen.

    // Render the things to the screen
    screen.setAsRenderTarget();

    bool is_first = true;
    while(!framebuffer_deuque.empty()){
        if (is_first){
            // Temporarily disable blending for the first framebuffer. This ensures any alpha stuff in the first framebuffer doesn't blend with the default black background
            glDisable(GL_BLEND);
            framebuffer_deuque.back()->draw();
            glEnable(GL_BLEND);
            is_first = false;
        } else {
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            framebuffer_deuque.back()->draw();
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        }

        // Regular pop without setting the next top one as the render target
        framebuffer_deuque.pop_back();
    }
}
