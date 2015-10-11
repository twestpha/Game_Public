#ifndef TexturePainter_h
#define TexturePainter_h

#include "includes/gl.hpp"
#include "includes/glm.hpp"

#include <random>

#include "debug.hpp"

#include "texture_layer.hpp"
#include "texture.hpp"

// Temporary
struct Brush{
    GLubyte* bitmap;
    int width, height;
    enum Mode {PAINT, ERASE};
};

class TexturePainter {
public:
    TexturePainter();
    TexturePainter(Texture texture);

    Texture getTexture();
    void setTexture(Texture texture);

    char getChannel();
    void setChannel(char channel);

    void paint(int x, int y, Brush::Mode mode);

private:

    int getIndex(int x, int y, int width);

    Texture texture;
    GLubyte* texture_bytes;

    char channel;

    Brush brush;

};

#endif
