// ui_window.h
// Trevor Westphal

#ifndef UIWindow_h
#define UIWindow_h


#include "ui_drawable.hpp"
#include "ui_image.hpp"
#include "ui_radiobutton.hpp"
#include "ui_button.hpp"

class UIWindow : public UIDrawable {
public:
    UIWindow();
    UIWindow(Shader shader);

    void loadFromXML(std::string);

    void draw();
    virtual void show();
    virtual void hide();
    void toggleShowing();

    bool isShowing() {return is_showing;}
protected:
    bool is_showing;

    std::vector<UIDrawable*> subelements;

    static UIImage* up_edge_image;
    static UIImage* right_edge_image;
    static UIImage* left_edge_image;
    static UIImage* down_edge_image;

    int edge_inner_offset;
    int edge_outer_offset;
    int edge_thickness;

    static UIImage* up_left_corner_image;
    static UIImage* up_right_corner_image;
    static UIImage* down_left_corner_image;
    static UIImage* down_right_corner_image;

    int corner_inner_offset;
    int corner_outer_offset;
    int corner_width;
    int corner_height;
};

#endif
