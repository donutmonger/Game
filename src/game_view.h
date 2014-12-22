#ifndef GameView_h
#define GameView_h

#include "mesh.h"
#include "drawable.h"
#include "camera.h"
#include "level.h"
#include "flat_mesh.h"
#include "flat_drawable.h"
#include "text_renderer.h"
#include "ui_drawable.h"

class GameView {
public:
    
    GameView(GLFWwindow*, Level*);

    void update();
private:
    void handleInputs();

    GLFWwindow* window;
    Level* level;

    GLuint framebuffer;
    GLuint framebuffer_texture;

    FlatDrawable* framebuffer_window;

    glm::mat3 mouse_projection;

    UIDrawable* mouse;

};

#endif