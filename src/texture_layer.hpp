#ifndef TextureLayer_h
#define TextureLayer_h


#include "texture.hpp"

class TextureLayer {
public:
    TextureLayer();
    TextureLayer(Texture diffuse, GLuint splatmap, char channel, int layer_number);

    void setDiffuse(Texture diffuse);
    void setSplatmap(GLuint splatmap);
    void setChannel(char channel_char);
    void setChannel(GLuint channel);
    void setLayerNumber(GLuint layer_number);

    Texture getDiffuse();
    GLuint getSplatmap();
    GLuint getChannel();
    GLuint getLayerNumber();
    char getChannelChar();

    string asJsonString();

    static int getIntFromChannelChar(char channel);
    static char getCharFromChannelInt(int channel);

private:
    Texture diffuse;
    GLuint splatmap;
    GLuint channel;
    GLuint layer_number;
    char channel_char;

};

#endif
