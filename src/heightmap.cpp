#include "heightmap.hpp"

// From http://www.exploringbinary.com/ten-ways-to-check-if-an-integer-is-a-power-of-two-in-c/
int isPowerOfTwo (unsigned int x){
    return ((x != 0) && ((x & (~x + 1)) == x));
}

Heightmap::Heightmap() {

}

Heightmap::Heightmap(std::string filename, float amplification) : File(filename) {
    this->amplification = amplification;

    // Number of components in the map image, 4 for RGBA
    components = 4;

    texture = Texture(filename);
    texture.setFormat(GL_RGBA);

    updateImage();

    if(!isPowerOfTwo(width) || !isPowerOfTwo(height)){
        Debug::warning("Terrain map size is not base 2."
            " Mesh generation may behave incorrectly.\n");
    }
}

float Heightmap::getMapHeight(int x, int y){
    // Scaling factor for the height map data
    int red = image[(y * width + x)*components + 0];
    int green = image[(y * width + x)*components + 1];
    int blue = image[(y * width + x)*components + 2];

    // Scale the height such that the value is between 0.0 and 1.0
    float map_height = float(red + green + blue) / (3.0f * 255.0);
    map_height *= amplification;
    return map_height;
}

void Heightmap::updateImage(){
    // Get the texture bytes
    texture.setFormat(GL_RGBA);
    image = texture.getBytes();

    // Update the dimensions, shouldn't ever need to be updated but is here for safety
    width = texture.getWidth();
    height = texture.getHeight();
}

Texture& Heightmap::getTexture(){
    return texture;
}

unsigned char* Heightmap::getImage(){
    return image;
}
