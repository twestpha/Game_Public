// mesh_loader.cpp
// implementation of the model class
// Trevor Westphal

#include <vector>  // vector
#include <fstream> // fgets
#include <stdio.h> // sscanf
#include <map>     // map
#include <array>   // array

#include "mesh_loader.hpp"

MeshLoader::MeshLoader(string filename){
    flat_shading = false;
    loadMeshFromDAE(filename);
}

std::vector<GLuint> getIntsFromString(std::string input, char delim){
    std::vector<GLuint> result;
    std::string tmp;
    std::string::iterator i;
    result.clear();

    for(i = input.begin(); i <= input.end(); ++i) {
        if((const char)*i != delim  && i != input.end()) {
            tmp += *i;
        } else {
            result.push_back(std::stoi(tmp));
            tmp = "";
        }
    }

    return result;
}

std::vector<glm::vec3> breakStringIntoVec3s(std::string input){
    std::vector<glm::vec3> result;
    std::string tmp;
    std::string::iterator i;
    char delim = ' ';

    glm::vec3 current_vector;
    int count = 0;
    for(i = input.begin(); i <= input.end(); ++i) {
        if((const char)*i != delim  && i != input.end()) {
            tmp += *i;
        } else {
            count++;
            float value = std::stof(tmp);
            if (count == 1){
                current_vector.x = value;
            } else if (count == 2){
                current_vector.y = value;
            } else if (count == 3){
                current_vector.z = value;
                result.push_back(current_vector);
                current_vector = glm::vec3();
                count = 0;
            }
            tmp = "";
        }
    }

    return result;
}

std::vector<glm::vec2> breakStringIntoVec2s(std::string input){
    std::vector<glm::vec2> result;
    std::string tmp;
    std::string::iterator i;
    char delim = ' ';

    glm::vec2 current_vector;
    int count = 0;
    for(i = input.begin(); i <= input.end(); ++i) {
        if((const char)*i != delim  && i != input.end()) {
            tmp += *i;
        } else {
            count++;
            float value = std::stof(tmp);
            if (count == 1){
                current_vector.x = value;
            } else if (count == 2){
                current_vector.y = value;
                result.push_back(current_vector);
                current_vector = glm::vec2();
                count = 0;
            }
            tmp = "";
        }
    }

    return result;
}

bool isAllThrees(std::string input){
    bool result = true;
    for (int i = 0; i < input.length(); ++i){
        char current_char = input[i];
        result &= (current_char == '3') || (current_char == ' ');
    }
    return result;
}

void MeshLoader::loadMeshFromDAE(string filename){
    float start_time = GameClock::getInstance()->getCurrentTime();
    this->filename = filename;

    // Load the document into a pugixml object
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename.c_str());

    // Get the node that contains geometry data for each mesh.
    pugi::xml_node mesh_list_node = doc.child("COLLADA").child("library_geometries");

    // For now we don't really support multiple mesh loading from one file.
    // Just look at the first geometry node.
    pugi::xml_node geometry_node = mesh_list_node.first_child();

    // Create the vectors that will hold vertex and face data
    std::vector<Vertex> vertices;
    std::vector<GLuint> elements;
    bool successful = getVerticesAndElements(geometry_node, vertices, elements);

    if (successful){
        // At this point the tangents and binormals still need to be
        // calculated.
        calculateTangentsAndBinormals(vertices, elements);

        writeFinalArrays(vertices, elements);

        float delta_time = GameClock::getInstance()->getCurrentTime() - start_time;
        Debug::info("Collada mesh loaded from '%s' in %.5f seconds.\n", filename.c_str(), delta_time);
    }
}

void MeshLoader::writeFinalArrays(std::vector<Vertex>& vertices, std::vector<GLuint>& elements){
    final_vertices.clear();
    final_faces.clear();

    for (int i = 0; i < vertices.size(); ++i){
        Vertex vertex = vertices[i];
        final_vertices.push_back(vertex.position.x);
        final_vertices.push_back(vertex.position.y);
        final_vertices.push_back(vertex.position.z);
        final_vertices.push_back(vertex.normal.x);
        final_vertices.push_back(vertex.normal.y);
        final_vertices.push_back(vertex.normal.z);
        final_vertices.push_back(vertex.tangent.x);
        final_vertices.push_back(vertex.tangent.y);
        final_vertices.push_back(vertex.tangent.z);
        final_vertices.push_back(vertex.binormal.x);
        final_vertices.push_back(vertex.binormal.y);
        final_vertices.push_back(vertex.binormal.z);
        final_vertices.push_back(vertex.texcoord.x);
        final_vertices.push_back(vertex.texcoord.y);
    }

    // Elements are already ordered correctly.
    // Could look for duplicates but, eh...
    final_faces = elements;
}

void MeshLoader::calculateTangentsAndBinormals(std::vector<Vertex>& vertices, std::vector<GLuint>& elements) {
    // For every face declaration calculate the
    // face tangents and binormals.

    for (int i = 0; i < elements.size(); i += 3){
        int A = elements[i];
        int B = elements[i + 1];
        int C = elements[i + 2];

        glm::vec3 P0 = vertices[A].position;
        glm::vec3 P1 = vertices[B].position;
        glm::vec3 P2 = vertices[C].position;

        glm::vec2 UV0 = vertices[A].texcoord;
        glm::vec2 UV1 = vertices[B].texcoord;
        glm::vec2 UV2 = vertices[C].texcoord;

        glm::vec3 p10 = P1 - P0;
        glm::vec3 p20 = P2 - P0;
        float u10 = UV1.x - UV0.x;
        float v10 = UV1.y - UV0.y;
        float u20 = UV2.x - UV0.x;
        float v20 = UV2.y - UV0.y;

        float divisor = 1.0f / (u10 * v20 - v10 * u20);
        glm::vec3 tangent = (p10 * v20 - p20 * v10) * divisor;
        glm::vec3 binormal = (p20 * u10 - p10 * u20) * divisor;

        glm::vec3 normal = vertices[A].normal;

        if (glm::dot(glm::cross(normal, tangent), binormal) < 0.0f){
            tangent = tangent * -1.0f;
        }

        if (flat_shading){
            tangent = glm::normalize(tangent);
            binormal = glm::normalize(binormal);

            vertices[A].tangent = tangent;
            vertices[A].binormal = binormal;

            vertices[B].tangent = tangent;
            vertices[B].binormal = binormal;

            vertices[C].tangent = tangent;
            vertices[C].binormal = binormal;

        } else {
            // These normals be fuckity.
            int A_unique_index = vertex_to_unique[A];
            int B_unique_index = vertex_to_unique[B];
            int C_unique_index = vertex_to_unique[C];

            unique_vertices[A_unique_index].tangent  += tangent;
            unique_vertices[A_unique_index].binormal += binormal;

            unique_vertices[B_unique_index].tangent  += tangent;
            unique_vertices[B_unique_index].binormal += binormal;

            unique_vertices[C_unique_index].tangent  += tangent;
            unique_vertices[C_unique_index].binormal += binormal;
        }

    }

    if (!flat_shading){
        for (int i = 0; i < unique_vertices.size(); ++i){
            glm::vec3 normal = unique_vertices[i].normal;
            glm::vec3 tangent = unique_vertices[i].tangent;
            tangent = tangent - normal * glm::dot(normal, tangent);
            unique_vertices[i].tangent = glm::normalize(tangent);
            unique_vertices[i].binormal = glm::normalize(glm::cross(normal, tangent));

        }

        for (int i = 0; i < vertices.size(); ++i){
            // Find which unique vertex this corresponds to
            int unique_vertex_index = vertex_to_unique[i];

            // Set this vertex's tangent and bitangent to be the same as
            // the unique vertex's
            vertices[i].tangent = unique_vertices[unique_vertex_index].tangent;
            vertices[i].binormal = unique_vertices[unique_vertex_index].binormal;

        }
    }
}

bool MeshLoader::getVerticesAndElements(pugi::xml_node geometry_node, std::vector<Vertex>& vertices, std::vector<GLuint>& elements){
    bool success = true;

    vertices.clear();
    elements.clear();

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    std::vector<GLuint> faces;

    std::string mesh_id = std::string(geometry_node.attribute("id").as_string());

    std::string mesh_vertex_source_id = mesh_id + "-positions";
    std::string mesh_normal_source_id = mesh_id + "-normals";
    std::string mesh_uv_source_id = mesh_id + "-map";

    // The first child in a geometry node contains all the data
    // relevant to the mesh.
    pugi::xml_node mesh_node = geometry_node.first_child();

    for (pugi::xml_node mesh_data_node : mesh_node.children()){
        // If it is a source node
        if (strcmp(mesh_data_node.name(), "source") == 0){
            std::string source_id = std::string(mesh_data_node.attribute("id").as_string());
            // Get the count and stride for error checking.
            pugi::xml_node accessor = mesh_data_node.child("technique_common").child("accessor");
            int stride = accessor.attribute("stride").as_int();

            if (source_id == mesh_vertex_source_id){
                // Check if the array width is correct for the positions
                if (stride != 3){
                    Debug::error("Invalid array width for vertex array in %s",
                        filename.c_str());
                } else {
                    std::string vertex_array_string = mesh_data_node.child_value("float_array");
                    positions = breakStringIntoVec3s(vertex_array_string);
                }

            } else if (source_id == mesh_normal_source_id){
                // Check if the array width is correct for the normals
                if (stride != 3){
                    Debug::error("Invalid array width for normal array in %s",
                        filename.c_str());
                } else {
                    std::string normal_array_string = mesh_data_node.child_value("float_array");
                    normals = breakStringIntoVec3s(normal_array_string);
                }

            } else if (source_id == mesh_uv_source_id){
                // Check if the array width is correct for the texture
                // coordinates
                if (stride != 2){
                    Debug::error("Invalid array width for uv array in %s",
                        filename.c_str());
                } else {
                    std::string uv_array_string = mesh_data_node.child_value("float_array");
                    texcoords = breakStringIntoVec2s(uv_array_string);
                }

            } else {
                Debug::warning("Unknown source id: %s.\n", source_id.c_str());
            }

        } else if (strcmp(mesh_data_node.name(), "polylist") == 0){
            // Check that the vertex counts are all three (triangles)
            std::string vcount_str = mesh_data_node.child_value("vcount");
            if (isAllThrees(vcount_str)){
                // Get the list of faces
                std::string faces_str = mesh_data_node.child_value("p");
                faces = getIntsFromString(faces_str, ' ');
            } else {
                Debug::error("The faces in mesh '%s' are not triangulated.\n",
                    filename.c_str());
            }
        }
    }

    bool loaded_correctly = !positions.empty() && !normals.empty() &&
    !texcoords.empty() && !faces.empty();

    if (loaded_correctly) {
        for (int i = 0; i < faces.size(); i += 3){
            // See if this combination of position, normal, uv, has
            // been used before. If not, make a new vertex and add
            // the corresponding number to the actual faces.
            int position_index = faces[i];
            int normal_index = faces[i + 1];
            int texcoord_index = faces[i + 2];
            Vertex vertex;
            vertex.position = positions[position_index];
            vertex.normal = normals[normal_index];

            glm::vec2 adjusted_texcoord = texcoords[texcoord_index];
            adjusted_texcoord.y = 1.0f - adjusted_texcoord.y;
            vertex.texcoord = adjusted_texcoord;

            vertices.push_back(vertex);
            int vertex_index = vertices.size() - 1;
            elements.push_back(vertex_index);


            std::vector<Vertex>::iterator it = std::find(unique_vertices.begin(),
                unique_vertices.end(), vertex);
            bool is_unique = (it == unique_vertices.end());
            if (is_unique){
                unique_vertices.push_back(vertex);
                int index = unique_vertices.size() - 1;
                vertex_to_unique[vertex_index] = index;
            } else {
                int index = it - unique_vertices.begin();
                vertex_to_unique[vertex_index] = index;
            }

        }

    } else {
        Debug::error("Failed to load mesh data from '%s'.\n", filename.c_str());
        success = false;
    }

    return success;

}
