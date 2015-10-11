// ui_button.h
// Trevor Westphal

#ifndef UIButton_h
#define UIButton_h

#include "ui_drawable.hpp"

#include "ui_image.hpp"
#include "window.hpp"
#include "mouse.hpp"
#include "text_renderer.hpp"

class UIButton : public UIDrawable {
public:
    UIButton(Shader);
    void loadFromXML(std::string);
    void draw();

protected:

    virtual void onClick(){;}

    bool constraintsAreValid(bool, bool, bool, bool, bool, bool);

    bool has_clicked;

    std::string functionName;

    TextRenderer* text_renderer;
    std::string button_text;

    int button_text_width;

    static UIImage* center_image;
    static UIImage* left_image;
    static UIImage* right_image;

    static UIImage* center_hover_image;
    static UIImage* left_hover_image;
    static UIImage* right_hover_image;
    // Needs hovering and click images
};

#endif
