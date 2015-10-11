#ifndef Terrain_h
#define Terrain_h

#include <SOIL.h>
#include <string>
#include <vector>
#include <cmath>
#include <unordered_map>
#include "includes/json.hpp"

#include "mesh.hpp"
#include "drawable.hpp"
#include "heightmap.hpp"
#include "vertex.hpp"
#include "terrain_mesh.hpp"
#include "game_clock.hpp"
#include "layered_textures.hpp"
#include "texture_layer.hpp"
#include "texture_painter.hpp"
#include "resource_loader.hpp"
#include "jsonable.hpp"

using namespace std;

class Terrain : public Drawable, public Jsonable {
public:
    Terrain() {;}
    Terrain(const Json::Value&, ResourceLoader& resource_loader);
    Terrain(string heightmap_filename, float amplification);
    Terrain (Shader& shader, string h) : Terrain(shader, h, 10.0f) {;}
    Terrain (Shader& shader, string, float);
    Drawable* clone();

    int getDepth() {return depth;}
    int getWidth() {return width;}

    GLfloat getHeight(GLfloat, GLfloat);
    GLfloat getHeightInterpolated(GLfloat, GLfloat);
    glm::vec3 getNormal(GLfloat, GLfloat);
    float getSteepness(GLfloat, GLfloat);
    float getMaxHeight(){return max_height;}

    void addSplatmap(Texture splat);
    void addDiffuse(Texture diff, GLuint splat, int layer_num, char channel);

    void printPathing();

    bool isOnTerrain(GLfloat, GLfloat, GLfloat);

    bool canPath(int, int);

    void paintSplatmap(glm::vec3 position);
    void eraseSplatmap(glm::vec3 position);

    TextureLayer getCurrentLayer();
    void setPaintLayer(GLuint layer);

    string asJsonString();

    void saveData(string name);

    LayeredTextures* getLayeredTextures();
    TexturePainter* getTexturePainter();

private:
    void initializer(Shader&, string, float, int tile_size);
    void updateUniformData();

    GLubyte* renderHeightmapAsImage();

    void initializeBaseMesh(Heightmap&);
    Mesh* generateMesh(Heightmap&);
    void generatePathingArray();
    int getIndex(int x, int y);
    int getIndex(int x, int y, int width);

    virtual void bindTextures();
    virtual void setTextureLocations();

    bool** pathing_array;

    vector<TerrainVertex> vertices;

    int width;
    int depth;
    int start_x;
    int start_z;
    float max_height;
    float amplification;
    int tile_size;

    LayeredTextures* layered_textures;

    TexturePainter* splatmap_painter;

    Heightmap heightmap;

};

#endif
