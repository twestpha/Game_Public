#ifndef LayeredTextures_h
#define LayeredTextures_h

#include "includes/gl.hpp"
#include "includes/glm.hpp"

#include <vector>
#include <string>

#include "texture_layer.hpp"
#include "shader.hpp"

using namespace std;

class LayeredTextures {
public:
    LayeredTextures(int size, int width, int height);

    void addSplatmap(Texture splatmap, int id);
    void addTexture(Texture diffuse, GLuint splatmap, char channel, int layer_number);

    void updateUniforms(Shader shader);
    void setTextureLocations(Shader shader);

    void swapLayers(GLuint layer1, GLuint layer2);

    bool needsSplatmaps();

    Texture& getSplatmap(int index);
    GLuint getTexture(GLuint splatmap, char channel);

    TextureLayer getLayer(GLuint splatmap, char channel);
    TextureLayer getLayer(GLuint layer_number);

    int getNumLayers();

    string asJsonString();

private:

    void fillSplatmaps();
    void fillLayers();

    std::vector<Texture> unique_splatmaps;
    std::vector<TextureLayer> texture_layers;

    int num_layers;
    int num_splatmaps;

    int width;
    int height;

};

#endif
