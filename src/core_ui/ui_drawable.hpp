#ifndef UIDrawable_h
#define UIDrawable_h

#include <string>   // std::string
#include <vector>   // std::vector

#include "pugixml.hpp" // PUGI xml library

#include "includes/gl.hpp"
#include "includes/glm.hpp"
#include "function_helper.hpp"
#include "flat_drawable.hpp"
#include "window.hpp"
#include "texture.hpp"

class UIDrawable : public FlatDrawable {
public:
    enum PositioningMode { Regular, CenterHorizontal, CenterVertical, Center };

    UIDrawable();
    UIDrawable(Texture texture);
    UIDrawable(Texture texture, PositioningMode mode);
    UIDrawable(Shader shader, Texture texture);

    void draw();
    void attachTexture(Texture);

    glm::vec2 getGLPosition();

    void setPixelCoordinates(int, int, int, int);
    void setGLPosition(glm::vec2);
    void setOutline(bool o) {outline = o;}
    void setGLCoordinates(glm::vec2, glm::vec2);

    void setParent(UIDrawable*);

    void setPositioningMode(PositioningMode mode);

    int getXPixels(){ return x_pixels;}
    int getYPixels(){ return y_pixels;}
    int getWidthPixels(){ return width_pixels;}
    int getHeightPixels(){ return height_pixels;}

    virtual void loadFromXML(std::string){;}
    virtual void receiveNotification(UIDrawable*);

protected:
    void load(Texture texture, PositioningMode mode);

    void parseConstraints(pugi::xml_node);
    virtual bool constraintsAreValid(bool, bool, bool, bool, bool, bool);
    virtual void didLoadXML(){;}

    int parseAnchor(const char*, bool);

    bool XOR(bool, bool);

    void updateDimensions();

    int window_width;
    int window_height;

    int width_pixels;
    int height_pixels;

    int x_pixels;
    int y_pixels;

    bool outline;
    glm::mat3 inv_mesh_projection;

    PositioningMode pos_mode;

    UIDrawable* parent;
};

#endif
