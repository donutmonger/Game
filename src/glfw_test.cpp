#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <OpenGL/OpenGL.h>

#include <SOIL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <thread>
#include <stdio.h>
#include <math.h>
#include <cstdlib>
#include <vector>
#include <time.h>

#include <random>

#include "model.h"
#include "camera.h"
#include "drawable.h"

//////////////////////////
// Function prototypes
GLFWwindow* initializeGLFWWindow(int, int);

//////////////////////////
// Implementation
int main() {
    srand(time(NULL));

    float width = 1366.0f;
    float height = 768.0f;
    GLFWwindow* window = initializeGLFWWindow(width, height);

    // Zooming stuff
    float field_of_view = 45.0f;
    float last_field_of_view = 45.0f;
    bool zoom_pressed = false;
    float start_time = 0;

    Camera camera(0.0f, 0.0f, 25.0f);
    glm::mat4 view_matrix = glm::mat4();
    glm::mat4 proj_matrix = glm::mat4();

    std::vector<Drawable> drawables;

    GLuint vertex_shader = ShaderLoader::loadVertexShader("shaders/vertex_shader.glsl");
    GLuint fragment_shader = ShaderLoader::loadFragmentShader("shaders/fragment_shader.glsl");
    GLuint shader_program = ShaderLoader::combineShaderProgram(vertex_shader, fragment_shader);

    // Load models with textures. Textures should
    // load independently but for now you have to
    // keep track of the texture number even when
    // loading a new model
    Model cube1 = Model();
    cube1.useTexture("res/diamond2.png", 0);
    cube1.attachShader(shader_program);
    Model cube2 = Model();
    cube2.useTexture("res/plank.png", 1);
    cube2.attachShader(shader_program);

    for (int i = -10; i < 10; ++i){
        for (int j = -10; j < 10; ++j){
            if (rand() % 2){
                drawables.push_back(Drawable(&cube1, glm::vec3(i, 1, j)));
            }
        }
    }
    for (int i = -10; i < 10; ++i){
        for (int j = -10; j < 10; ++j){
            drawables.push_back(Drawable(&cube2, glm::vec3(i, 0, j)));
        }
    }
    // Display loop
    while(!glfwWindowShouldClose(window)) {
        // Swap display/rendering buffers
        glfwSwapBuffers(window);
        
        // Handle events
        glfwPollEvents();
        // Check to see if escape is pressed. If so, close the window
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        // Camera controls
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
            camera.moveZ(-1);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
            camera.moveZ(1);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
            camera.moveX(1);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
            camera.moveX(-1);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
            camera.moveY(-1);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
            camera.moveY(1);
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
            camera.rotateY(-1.0f);
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
            camera.rotateY(1.0f);
        }

        // Zoom code, should be changed for cleanliness and it doesn't respond
        // well to high frequency clicks
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS){
            if (!zoom_pressed) {
                start_time = (float)glfwGetTime();
                zoom_pressed = true;
            }
        } else {
            if (zoom_pressed){
                start_time = (float)glfwGetTime();
                zoom_pressed = false;
            }
        }
        if (zoom_pressed) { 
            float delta_t = (float)glfwGetTime() - start_time;
            if (delta_t < 0.75){
                field_of_view = (1.5 / delta_t) + 22.5;
                field_of_view = fmin(field_of_view, last_field_of_view);
                last_field_of_view = field_of_view;
            }
        } else {
            float delta_t = (float)glfwGetTime() - start_time;
            if (delta_t < 0.75){
                field_of_view = -(1.5 / delta_t) + 45.0f;
                field_of_view = fmax(field_of_view, last_field_of_view);
                last_field_of_view = field_of_view;
            }
        }
        // End of bad zoom code


        proj_matrix = glm::perspective(field_of_view, width / height, 1.0f, 100.0f);
        view_matrix = camera.getViewMatrix();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i = 0; i < drawables.size(); ++i){
            drawables[i].draw(&view_matrix, &proj_matrix);
        }
    }

    // Shut down GLFW before exiting the program
    glfwTerminate();

    // Nothing went wrong!
    return 0;
}

GLFWwindow* initializeGLFWWindow(int width, int height){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Windowed
    // GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr); 
    // Fullscreen 
    GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL", glfwGetPrimaryMonitor(), nullptr);

    // Hide the mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // Create the OpenGL context in the window
    glfwMakeContextCurrent(window);

    // Set up GLEW so that we can use abstracted OpenGL functions
    glewExperimental = GL_TRUE;
    glewInit();

    // Print various info about OpenGL
    printf("\n");
    printf("Renderer:       %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    printf("GLSL version:   %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("\n");

    // Set up the correct depth rendering 
    glEnable(GL_DEPTH_TEST);

    return window;   
}