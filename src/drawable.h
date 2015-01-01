#ifndef Drawable_h
#define Drawable_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#if defined __APPLE__ && __MACH__
    #include <OpenGL/OpenGL.h>
// #elif defined __gnu_linux__
#endif

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.h"
#include "camera.h"
#include "texture_set.h"

class Drawable {
public:
    Drawable () {;}
    Drawable(Mesh*, GLuint);
    Drawable(Mesh*, GLuint, glm::vec3, GLfloat);
    Drawable(Mesh*, GLuint, glm::vec3, glm::vec3, GLfloat);

    void draw(Camera*, glm::mat4*);
    void draw(glm::mat4*, glm::mat4*);

    virtual void attachTextureSet(TextureSet) = 0;

    void setPosition(glm::vec3 p) {position = p;}
    void setRotation(glm::vec3 r) {rotation = r;}
    void setScale(GLfloat s) {scale = s;}

    glm::vec3 getPosition() {return position;}
    glm::vec3 getRotation() {return rotation;}
    GLfloat getScale() {return scale;}

protected:
    void load(Mesh*, GLuint, glm::vec3, glm::vec3, GLfloat);
    void updateModelMatrix();

    virtual void bindTextures() = 0;
    virtual void updateUniformData() = 0;

    Mesh* mesh;

    GLuint shader_program;

    GLfloat scale;

    glm::vec3 position;
    glm::vec3 rotation;

    glm::mat4 model_matrix;

    TextureSet* texture_set;
};

#endif
