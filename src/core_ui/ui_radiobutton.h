// ui_radiobutton.h
// Trevor Westphal

#ifndef UIRadioButton_h
#define UIRadioButton_h

#include "ui_drawable.h"
#include "texture_loader.h"
#include "ui_image.h"
#include "window.h"
#include "mouse.h"
#include "text_renderer.h"

class UIRadioButton : public UIDrawable {
public:
    UIRadioButton(GLuint);
    void loadFromXML(std::string);
    void draw();


protected:
    std::string functionName;

    bool constraintsAreValid(bool, bool, bool, bool, bool, bool);

    void toggleRadioButton();

    // Extensible for child classes
    virtual void onClick(){;}

    bool radioButtonOn;
    bool has_clicked;

    int icon_width;
    int icon_height;

    static UIImage* on_icon;
    static UIImage* off_icon;
    static UIImage* hover_icon;
    UIImage* current_icon;

    TextRenderer* text_renderer;
    std::string radio_text;
};

#endif
