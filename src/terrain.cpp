#include "terrain.hpp"

// Terrain dimensions:
//
//            height
//            _|_
//        ---|   |-----------
//       /  /     \        /   h
//      /  |       |      /   t
//     /                 /   p
//    /                 /   e
//   /                 /   d
//   ------------------
//        width

Terrain::Terrain(const Json::Value& terrain_json, ResourceLoader& resource_loader){

    string texture_path = resource_loader.getTexturePath();

    // Basic terrain loading
    string heightmap_filename = texture_path + terrain_json["heightmap"].asString();
    float amplification = terrain_json["amplification"].asFloat();
    int tile_size = terrain_json["tile_size"].asInt();
    Shader& shader_ref = resource_loader.loadShader("shaders/terrain.vs", "shaders/terrain.fs");

    initializer(shader_ref, heightmap_filename, amplification, tile_size);

    // Do the textures

    // Splatmaps
    for(const Json::Value& splatmap_json : terrain_json["splatmaps"]){
        string filename = texture_path + splatmap_json["filename"].asString();
        int id = splatmap_json["id"].asInt();
        Texture splatmap(filename);
        layered_textures->addSplatmap(splatmap, id);
    }

    // Layers
    for (const Json::Value& layer_json : terrain_json["texture_layers"]){
        int layer_number = layer_json["layer_number"].asInt();
        int splatmap_number = layer_json["splatmap"].asInt();
        char splatmap_channel = layer_json["channel"].asString().at(0);
        string diff_filename = texture_path + layer_json["textures"]["diff"].asString();
        Texture diffuse(diff_filename);

        addDiffuse(diffuse, splatmap_number, layer_number, splatmap_channel);

    }


}

Terrain::Terrain(string heightmap_filename, float amplification){
    Shader* shader = new Shader("shaders/terrain.vs", "shaders/terrain.fs");

    initializer(*shader, heightmap_filename, amplification, 16);

}

Terrain::Terrain(Shader& shader, string heightmap_filename, float amplification) : Drawable() {

    initializer(shader, heightmap_filename, amplification, 16);

}

Drawable* Terrain::clone() {
    return new Terrain(*this);
}

void Terrain::initializer(Shader& shader, string heightmap_filename, float amplification, int tile_size){

    this->amplification = amplification;
    this->tile_size = tile_size;

    // This is where generate the new mesh and override the one passed in by
    // the constructor. This is to save space in the game files, so we don't have a terrain mesh

    max_height = amplification;
    // After loading in the heightmap to memory, we can make a terrain mesh
    // based on the data
    float start_time = GameClock::getInstance()->getCurrentTime();
    heightmap = Heightmap(heightmap_filename, amplification);
    mesh = generateMesh(heightmap);
    float delta_time = GameClock::getInstance()->getCurrentTime() - start_time;
    Debug::info("Took %f seconds to generate the terrain mesh.\n", delta_time);

    // Once we have a mesh, we can load the drawable data required for this
    // child class.
    Drawable::load(*mesh, shader, glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);

    generatePathingArray();

    splatmap_painter = new TexturePainter(0);

    // Debugging the allowed areas
    // printPathing();

    // Initialize the layered textures
    layered_textures = new LayeredTextures(7, width, depth);

}

void Terrain::generatePathingArray(){
    // Could do bit-packing here but it really doesn't matter
    pathing_array = new bool*[depth];
    for(int i = 0; i < depth; ++i){
        pathing_array[i] = new bool[width];
    }

    // Iterate through the pathing array, filling in all the places where we can't go
    for(int z = 0; z < depth; ++z){
        for(int x = 0; x < width; ++x){
            pathing_array[z][x] = (getSteepness(GLfloat(x) + start_x, GLfloat(z) + start_z) < 0.8f);

            if(pathing_array[z][x]){
                // printf("  ");
            } else {
                // printf("██");
            }
        }
        // printf("\n");
    }
}

void Terrain::paintSplatmap(glm::vec3 mouse_position){
    int x_offset = mouse_position.x - start_x;
    int y_offset = mouse_position.z - start_z;
    splatmap_painter->paint(x_offset, y_offset, Brush::Mode::PAINT);
}

void Terrain::eraseSplatmap(glm::vec3 mouse_position){
    int x_offset = mouse_position.x - start_x;
    int y_offset = mouse_position.z - start_z;
    splatmap_painter->paint(x_offset, y_offset, Brush::Mode::ERASE);
}

bool Terrain::canPath(int x, int z){
    x -= int(start_x);
    z -= int(start_z);

    if(x < 0 || z < 0 || x > width - 1 || z > depth - 1){
        return false;
    }

    return pathing_array[z][x];
}

void Terrain::printPathing(){
    Debug::info("Pathing array:\n");
    for (int x = 0; x < width - 1; ++x){
        for (int z = 0; z < depth - 1; ++ z){
            printf(" %d", pathing_array[z][x]);
        }
        printf("\n");
    }
}

GLfloat Terrain::getHeight(GLfloat x_pos, GLfloat z_pos){
    // Returns the map height for a specified x and z position.
    // This will be useful for moving units around the terrain.

    // Just do an integer conversion to get the vertex index.
    int x = x_pos;
    int z = z_pos;

    // Offset the positions by the starting positions of the mesh
    x = x - start_x;
    z = z - start_z;

    // Calculate the index of the current point in the vertex array.
    int i = getIndex(x, z);

    if (i < 0 || i > vertices.size() - 1){
        return 0.0f;
    } else {
        return vertices[i].position.y;
    }
}

GLfloat Terrain::getHeightInterpolated(GLfloat x_pos, GLfloat z_pos){
    // Interpolates in the current square. Because the positions get floored
    // We can say that the unit is always "at" the top left.

    //        o--------o
    //        |1      2|
    //        |        |
    //        |3      4|
    //        o--------o
    //     x
    //   o-->
    // z |
    //   v

    // Get the heights for each point
    GLfloat height_1 = getHeight(x_pos, z_pos);
    GLfloat height_2 = getHeight(x_pos + 1, z_pos);
    GLfloat height_3 = getHeight(x_pos, z_pos + 1);

    // Get the change in height from point 2 to 1 and from 3 to 1
    GLfloat delta_height_2 = height_2 - height_1;
    GLfloat delta_height_3 = height_3 - height_1;

    // Find the fractional component of the
    // x and z position to know how much to
    // weight each height.
    double intpart;
    GLfloat x_mult = modf(x_pos, &intpart);
    GLfloat z_mult = modf(z_pos, &intpart);

    // Calculate the height by adding the initial height to the
    // weighted combination of the other two points.
    GLfloat interpolated_height = height_1 + (x_mult * delta_height_2) +
        (z_mult * delta_height_3);

    return interpolated_height;
}

glm::vec3 Terrain::getNormal(GLfloat x_pos, GLfloat z_pos){
    // Returns the normal vector at the specified x and y position.
    // This is good for knowing how a unit can move across a segment
    // of terrain. For example, if the normal is too steep, the unit
    // won't be able to move on that segment.
    // Just do an integer conversion to get the vertex index.
    // Later this should be interpolated using the normal.
    int x = x_pos;
    int z = z_pos;

    // Offset the positions by the starting positions of the mesh
    x = x - start_x;
    z = z - start_z;

    // Calculate the index of the current point in the vertex array.
    int i = getIndex(x, z);

    if (i < 0 || i > vertices.size() - 1){
        return glm::vec3(0.0f, 0.0f, 0.0f);
    } else {
        return vertices[i].normal;
    }
}

GLfloat Terrain::getSteepness(GLfloat x_pos, GLfloat z_pos){
    glm::vec3 normal = getNormal(x_pos, z_pos);
    GLfloat cosTheta = glm::dot(glm::vec3(0.0, 1.0, 0.0), normal);

    GLfloat steepness = acos(cosTheta);

    return steepness;
}

bool Terrain::isOnTerrain(GLfloat x_pos, GLfloat z_pos, GLfloat tolerance){
    bool is_on_terrain = (x_pos >= (-getWidth() / 2.0) + tolerance) &&
        (x_pos <= (getWidth() / 2.0) - tolerance) &&
        (z_pos >= (-getDepth() / 2.0) + tolerance) &&
        (z_pos <= (getDepth() / 2.0) - tolerance);
    return is_on_terrain;
}

void Terrain::initializeBaseMesh(Heightmap& heightmap){
    // This generates the mesh that will be used for the
    // top level terrain data, like height and normals.
    // This is just the basic layout of the mesh though
    // because the actual mesh that gets drawn needs to
    // have more accurate normals and texture coordinates
    // for actually drawing things.
    vertices = vector<TerrainVertex>(width * depth);
    for (int x = 0; x < width; ++x){
        for (int z = 0; z < depth; ++z){
            TerrainVertex current;
            float height = heightmap.getMapHeight(x, z);
            current.position = glm::vec3(x + start_x, height, z + start_z);

            float u = x / (float)width;
            float v = z / (float)depth;
            current.splatcoord = glm::vec2(u, v);

            int index = getIndex(x, z);
            vertices[index] = current;
        }
    }

    // Make the edge loops to simplify the normal
    // calculations.
    vector<GLuint> faces;
    for (int x = 0; x < width - 1; ++x){
        for (int z = 0; z < depth - 1; ++z){
            faces.push_back(getIndex(x, z));
            faces.push_back(getIndex(x + 1, z));
            faces.push_back(getIndex(x, z + 1));

            faces.push_back(getIndex(x + 1, z));
            faces.push_back(getIndex(x + 1, z + 1));
            faces.push_back(getIndex(x, z + 1));
        }
    }

    // Dumb normal calculations without hard edge detection
    // These are sufficient for gameplay terrain data (pathing).
    for (int i = 0; i < faces.size(); i += 3){
        TerrainVertex* a = &vertices[faces[i]];
        TerrainVertex* b = &vertices[faces[i + 1]];
        TerrainVertex* c = &vertices[faces[i + 2]];

        glm::vec3 edge1 = b->position - a->position;
        glm::vec3 edge2 = c->position - a->position;

        glm::vec3 normal   = glm::cross(edge2, edge1);
        glm::vec3 tangent  = edge1;
        glm::vec3 binormal = edge2;

        a->normal += normal;
        b->normal += normal;
        c->normal += normal;

        a->tangent += tangent;
        b->tangent += tangent;
        c->tangent += tangent;

        a->binormal += binormal;
        b->binormal += binormal;
        c->binormal += binormal;

    }

    // Normalize the normals
    for (int i = 0; i < vertices.size(); ++i){
        TerrainVertex* current = &vertices[i];
        current->normal = glm::normalize(current->normal);
        current->tangent = glm::normalize(current->tangent);
        current->binormal = glm::normalize(current->binormal);

    }
}

Mesh* Terrain::generateMesh(Heightmap& heightmap){
    width = heightmap.getWidth();
    depth = heightmap.getHeight();

    start_x = -width / 2;
    start_z = -depth / 2;

    // Generate the mesh for gameplay data
    initializeBaseMesh(heightmap);

    // Now make it look nice!

    // The number of terrain tiles before the
    // texture repeats.
    int texture_size = tile_size;

    // This should probably definitely be rewritten because
    // it will be hard to do the normal fix afterwards. Also
    // its so many loops!
    vector<GLuint> faces;
    vector<TerrainVertex> textured_vertices;
    unordered_map<int, bool> valid_vertices;
    for (int x = 0; x < width; x += texture_size){
        for (int z = 0; z < depth; z += texture_size){
            int start_index = textured_vertices.size();

            // Create the big tile
            for (int i = 0; i <= texture_size; ++i){
                for (int j = 0; j <= texture_size; ++j){
                    int index = getIndex(x + i, z + j);
                    float u = i / (float)(texture_size);
                    float v = j / (float)(texture_size);

                    if (index < vertices.size()){
                        TerrainVertex current = vertices[index];
                        current.texcoord = glm::vec2(u, v);
                        textured_vertices.push_back(current);
                    } else {
                        TerrainVertex current;
                        textured_vertices.push_back(current);
                    }

                    int textured_index = textured_vertices.size() - 1;
                    if (((x + i) >= width) || ((z + j) >= depth)){
                        valid_vertices[textured_index] = false;
                    } else {
                        valid_vertices[textured_index] = true;
                    }
                }
            }

            for (int i = 0; i < texture_size; ++i){
                for (int j = 0; j < texture_size; ++j){
                    int block_width = texture_size + 1;

                    int upper_left  = start_index + getIndex(i, j, block_width);
                    int upper_right = start_index + getIndex(i + 1, j, block_width);
                    int lower_left  = start_index + getIndex(i, j + 1, block_width);
                    int lower_right = start_index + getIndex(i + 1, j + 1, block_width);

                    bool is_valid = true;
                    is_valid &= valid_vertices[upper_left];
                    is_valid &= valid_vertices[upper_right];
                    is_valid &= valid_vertices[lower_left];
                    is_valid &= valid_vertices[lower_right];

                    if (is_valid){
                        faces.push_back(upper_left);
                        faces.push_back(upper_right);
                        faces.push_back(lower_left);

                        faces.push_back(upper_right);
                        faces.push_back(lower_right);
                        faces.push_back(lower_left);
                    }

                }
            }

        }
    }

    return new TerrainMesh(textured_vertices, faces);
}

int Terrain::getIndex(int x, int z){
    // For a given x,z coordinate this will return the
    // index for that element in our linear arrays: vertices,
    // and normals.
    return x + ((width) * z);
}

int Terrain::getIndex(int x, int z, int width){
    // For a given x,z coordinate this will return the
    // index for that element in our linear arrays: vertices,
    // and normals.
    return x + ((width) * z);
}

void Terrain::updateUniformData(){
    // Set the scale, this is not really going to be a thing, probably
    // ^ It's definitely a thing
    GLuint scale_loc = glGetUniformLocation(shader->getGLId(), "scale");
    GLuint time_loc = glGetUniformLocation(shader->getGLId(), "time");

    glUniform1f(scale_loc, scale);
    glUniform1f(time_loc, GameClock::getInstance()->getCurrentTime());

}

void Terrain::bindTextures(){
    // Put each texture into the correct location for this Drawable. GL_TEXTURE0-3
    // correspond to the uniforms set in attachTextureSet(). This is where we actually
    // tell the graphics card which textures to use.

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specular.getGLId());

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, emissive.getGLId());

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, normal.getGLId());

    layered_textures->updateUniforms(shader->getGLId());

}

void Terrain::setTextureLocations(){
    // Try to set the texture locations
    glUniform1i(glGetUniformLocation(shader->getGLId(), "specular_texture"), 1);
    glUniform1i(glGetUniformLocation(shader->getGLId(), "emissive_texture"), 2);
    glUniform1i(glGetUniformLocation(shader->getGLId(), "normal_map"), 3);
    glUniform1i(glGetUniformLocation(shader->getGLId(), "shadow_map"), 4);

    layered_textures->setTextureLocations(shader->getGLId());

}

void Terrain::addSplatmap(Texture splat){
    // // Check the dimensions of the splatmap and ensure that they are the
    // // same as the heightmap's.
    // GLuint splat_width = splat.getWidth();
    // GLuint splat_height = splat.getHeight();
    // if (splat_width == width && splat_height == depth){
    //     layered_textures->addSplatmap(splat);
    // } else {
    //     Debug::warning("Splatmap dimensions do not agree with heightmap dimensions.\n");
    //     Texture blank_splat(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), width, depth);
    //     layered_textures->addSplatmap(blank_splat);
    // }
}

void Terrain::addDiffuse(Texture diff, GLuint splat, int layer_num, char channel) {

    layered_textures->addTexture(diff, splat, channel, layer_num);

    // Pretty sure this dont do anything
    Drawable::setDiffuse(diff);
}

LayeredTextures* Terrain::getLayeredTextures(){
    return layered_textures;
}

TexturePainter* Terrain::getTexturePainter(){
    return splatmap_painter;
}

TextureLayer Terrain::getCurrentLayer(){
    TextureLayer layer = layered_textures->getLayer(splatmap_painter->getTexture().getGLId(), splatmap_painter->getChannel());
    return layer;
}

void Terrain::setPaintLayer(GLuint layer){
    TextureLayer texture_layer = layered_textures->getLayer(layer);

    // Sanity check
    if (texture_layer.getLayerNumber() == layer && layer != 0){
        char channel = TextureLayer::getCharFromChannelInt(texture_layer.getChannel());
        GLuint splatmap = layered_textures->getSplatmap(texture_layer.getSplatmap()).getGLId();

        splatmap_painter->setChannel(channel);
        splatmap_painter->setTexture(splatmap);
    } else if (layer == 0) {
        Debug::error("Cannot use base layer as paint layer.\n");
    } else {
        Debug::error("Cannot set layer to %d.\n", layer);
    }

}

string Terrain::asJsonString() {
    // Outputs the formatted json string for this terrain object
    // Example:
        // "terrain": {
        //     "heightmap": "heightmap.png",
        //     "amplification": 10.0,
        //     "tile_size": 8,
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
        //         },
        //         ...
        //     ]
        // }

    string json_string = "\"terrain\": {\n";

    // Basic terrain data
    json_string += "\"heightmap\": \"" + heightmap.getFilename() + "\",\n";
    json_string += "\"amplification\": " + to_string(amplification) + ",\n";
    json_string += "\"tile_size\": " + to_string(tile_size) + ",\n";

    // Splatmaps and Texture Layers are both handled by LayeredTextures
    json_string += layered_textures->asJsonString() + "\n";

    json_string += "}";

    return json_string;
}

void Terrain::saveData(string name){
    Texture& texture = heightmap.getTexture();
    string heightmap_name = name + "_heightmap.bmp";
    texture.setFormat(GL_RGBA);
    texture.saveAs(heightmap_name);

}

GLubyte* Terrain::renderHeightmapAsImage(){
    GLubyte* data = new GLubyte[width * depth];
    for (int i = 0; i < width * depth; i++){
        int height = (vertices[i].position.y / amplification) * 255;
        data[i] = height;
    }
    return data;
}
