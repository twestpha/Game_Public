#include "layered_textures.hpp"

LayeredTextures::LayeredTextures(int size, int width, int height) :  num_layers(size), width(width), height(height) {

    num_splatmaps = (size - 1) / 3;

    texture_layers = std::vector<TextureLayer>(num_layers);
    unique_splatmaps = std::vector<Texture>(num_splatmaps);

    fillSplatmaps();
    fillLayers();

}

void LayeredTextures::addSplatmap(Texture splatmap, int splat_index){
    if (splat_index >= num_splatmaps){
        Debug::error("Splatmap number out of bounds %d. Range is [0, %d]\n", splat_index, num_splatmaps - 1);
    }
    unique_splatmaps[splat_index] = splatmap;
}

void LayeredTextures::addTexture(Texture diffuse, GLuint splat_index, char channel, int layer_number){
    TextureLayer layer(diffuse, splat_index, channel, layer_number);

    if (layer_number >= num_layers){
        Debug::error("Layer number out of bounds %d. Range is [0, %d]\n", layer_number, num_layers - 1);
    } else if (splat_index >= num_splatmaps){
        Debug::error("Splatmap number out of bounds %d. Range is [0, %d]\n", splat_index, num_splatmaps - 1);
    } else {
        texture_layers[layer_number] = layer;
    }
}

Texture& LayeredTextures::getSplatmap(int index){
    if (index >= 0 && index < num_splatmaps){
        return unique_splatmaps[index];
    } else {
        Texture* tex = new Texture(0);
        return *tex;
    }
}

GLuint LayeredTextures::getTexture(GLuint splatmap, char channel){
    TextureLayer layer = getLayer(splatmap, channel);
    return layer.getDiffuse().getGLId();
}

TextureLayer LayeredTextures::getLayer(GLuint splatmap, char channel){
    TextureLayer out_layer;
    for (TextureLayer layer : texture_layers){
        if (layer.getChannelChar() == channel && unique_splatmaps[layer.getSplatmap()].getGLId() == splatmap){
            out_layer = layer;
        }
    }
    return out_layer;
}

TextureLayer LayeredTextures::getLayer(GLuint layer_number){
    if (layer_number >= 0 && layer_number < num_layers){
        return texture_layers[layer_number];
    } else {
        TextureLayer layer;
        return layer;
    }
}

void LayeredTextures::updateUniforms(Shader shader){
    ////////////////////
    // Diffuse
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, texture_layers[0].getDiffuse().getGLId());

    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, texture_layers[1].getDiffuse().getGLId());

    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, texture_layers[2].getDiffuse().getGLId());

    glActiveTexture(GL_TEXTURE13);
    glBindTexture(GL_TEXTURE_2D, texture_layers[3].getDiffuse().getGLId());

    glActiveTexture(GL_TEXTURE14);
    glBindTexture(GL_TEXTURE_2D, texture_layers[4].getDiffuse().getGLId());

    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, texture_layers[5].getDiffuse().getGLId());

    glActiveTexture(GL_TEXTURE16);
    glBindTexture(GL_TEXTURE_2D, texture_layers[6].getDiffuse().getGLId());

    ////////////////////
    // Splatmaps
    glActiveTexture(GL_TEXTURE20);
    glBindTexture(GL_TEXTURE_2D, unique_splatmaps[0].getGLId());

    glActiveTexture(GL_TEXTURE21);
    glBindTexture(GL_TEXTURE_2D, unique_splatmaps[1].getGLId());

    glUniform1i(glGetUniformLocation(shader.getGLId(), "splatmaps[0]"), texture_layers[0].getSplatmap());
    glUniform1i(glGetUniformLocation(shader.getGLId(), "splatmaps[1]"), texture_layers[1].getSplatmap());
    glUniform1i(glGetUniformLocation(shader.getGLId(), "splatmaps[2]"), texture_layers[2].getSplatmap());
    glUniform1i(glGetUniformLocation(shader.getGLId(), "splatmaps[3]"), texture_layers[3].getSplatmap());
    glUniform1i(glGetUniformLocation(shader.getGLId(), "splatmaps[4]"), texture_layers[4].getSplatmap());
    glUniform1i(glGetUniformLocation(shader.getGLId(), "splatmaps[5]"), texture_layers[5].getSplatmap());
    glUniform1i(glGetUniformLocation(shader.getGLId(), "splatmaps[6]"), texture_layers[6].getSplatmap());

    ////////////////////
    // Channels
    glUniform1i(glGetUniformLocation(shader.getGLId(), "channels[0]"),texture_layers[0].getChannel());
    glUniform1i(glGetUniformLocation(shader.getGLId(), "channels[1]"),texture_layers[1].getChannel());
    glUniform1i(glGetUniformLocation(shader.getGLId(), "channels[2]"),texture_layers[2].getChannel());
    glUniform1i(glGetUniformLocation(shader.getGLId(), "channels[3]"),texture_layers[3].getChannel());
    glUniform1i(glGetUniformLocation(shader.getGLId(), "channels[4]"),texture_layers[4].getChannel());
    glUniform1i(glGetUniformLocation(shader.getGLId(), "channels[5]"),texture_layers[5].getChannel());
    glUniform1i(glGetUniformLocation(shader.getGLId(), "channels[6]"),texture_layers[6].getChannel());

}

void LayeredTextures::setTextureLocations(Shader shader){
    ////////////////////
    // Diffuse
    glUniform1i(glGetUniformLocation(shader.getGLId(), "diffuse_textures[0]"), 10);
    glUniform1i(glGetUniformLocation(shader.getGLId(), "diffuse_textures[1]"), 11);
    glUniform1i(glGetUniformLocation(shader.getGLId(), "diffuse_textures[2]"), 12);
    glUniform1i(glGetUniformLocation(shader.getGLId(), "diffuse_textures[3]"), 13);
    glUniform1i(glGetUniformLocation(shader.getGLId(), "diffuse_textures[4]"), 14);
    glUniform1i(glGetUniformLocation(shader.getGLId(), "diffuse_textures[5]"), 15);
    glUniform1i(glGetUniformLocation(shader.getGLId(), "diffuse_textures[6]"), 16);

    ////////////////////
    // Splatmaps
    glUniform1i(glGetUniformLocation(shader.getGLId(), "unique_splatmaps[0]"), 20);
    glUniform1i(glGetUniformLocation(shader.getGLId(), "unique_splatmaps[1]"), 21);
}

void LayeredTextures::swapLayers(GLuint layer1, GLuint layer2){
    if (layer1 == 0 || layer2 == 0){
        Debug::error("Cannot swap with the base layer (layer 0).\n");
        return;
    }
    TextureLayer temp = texture_layers[layer2];
    texture_layers[layer2] = texture_layers[layer1];
    texture_layers[layer1] = temp;

    texture_layers[layer1].setLayerNumber(layer1);
    texture_layers[layer2].setLayerNumber(layer2);
}

string LayeredTextures::asJsonString() {
    // Example:
    //     "splatmaps": [
    //         {
    //             "id": 0,
    //             "filename": "all_splat.png"
    //         },
    //         {
    //             "id": 1,
    //             "filename": "second_splat.png"
    //         }
    //     ],
    //     "texture_layers": [
    //         {
    //             "layer_number": 0,
    //             "splatmap": 0,
    //             "channel": "r",
    //             "textures": {
    //                 "diff": "stylized_grass.png"
    //             }
    //         },
    //         {
    //             "layer_number": 1,
    //             "splatmap": 0,
    //             "channel": "r",
    //             "textures": {
    //                 "diff": "stone_tile.png"
    //             }
    //         },
    //         {
    //             "layer_number": 2,
    //             "splatmap": 0,
    //             "channel": "g",
    //             "textures": {
    //                 "diff": "stone.png"
    //             }
    //         }
    //         ...
    //     ]

    string json_string = "";

    // Splatmaps
    json_string += "\"splatmaps\": [\n";
    int id = 0;
    for (int id = 0; id < unique_splatmaps.size(); ++id) {
        Texture& splatmap = unique_splatmaps[id];
        json_string += "{\n";
        json_string += "\"id\": " + to_string(id) + ",\n";
        json_string += splatmap.asJsonString("filename");
        splatmap.save();
        if (id == (unique_splatmaps.size() - 1)){
            json_string += "}\n";
        } else {
            json_string += "},\n";
        }
    }
    json_string += "],\n";

    // Texture layers
    json_string += "\"texture_layers\": [\n";
    for (TextureLayer& layer : texture_layers) {
        json_string += layer.asJsonString();
        if (&layer == &(texture_layers.back())){
            json_string += "\n";
        } else {
            json_string += ",\n";
        }
    }
    json_string += "]";

    return json_string;
}

bool LayeredTextures::needsSplatmaps(){
    return num_splatmaps > unique_splatmaps.size();
}

int LayeredTextures::getNumLayers(){
    return num_layers;
}

void LayeredTextures::fillLayers() {
    for (int layer_num = 0; layer_num < texture_layers.size(); ++layer_num) {
        TextureLayer& layer = texture_layers[layer_num];

        // Set the layer to a random color
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        Texture random_diff(glm::vec4(r, g, b, 1));
        layer.setDiffuse(random_diff);

        // Set the layer attributes correctly
        layer.setLayerNumber(layer_num);
        if (layer_num <= 3) {
            layer.setSplatmap(0);
        } else {
            layer.setSplatmap(1);
        }
        if (layer_num == 0) {
            layer.setChannel('r');
        } else if (layer_num % 3 == 1) {
            layer.setChannel('r');
        } else if (layer_num % 3 == 2) {
            layer.setChannel('g');
        } else if (layer_num % 3 == 0) {
            layer.setChannel('b');
       }
    }
}

void LayeredTextures::fillSplatmaps() {
    for (int i = 0; i < num_splatmaps; ++i){
        Texture blank_splat(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), width, height);
        addSplatmap(blank_splat, i);
    }
}
