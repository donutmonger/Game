#define GLEW_STATIC

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

#include <stdio.h>
#include <cstdlib>
#include <random>
#include <iostream>
#include <vector>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include "world.h"
#include "debug.h"
#include "profile.h"
#include "window.h"

void renderEverything(GLFWwindow*);
void inputConsole();
GLFWwindow* initializeGLFWWindow(int, int, bool);

int main(int argc, char* argv[]) {
    // Make the randomizer random
    srand(time(NULL));

    // Parse command line arguments
    float width;
    float height;
    bool fullscreen = false;
    bool interactive = false;
    bool debug = false;
    char argument;
    bool has_map;
    std::string map_filename;

    while ((argument = getopt(argc, argv, "wfidm:x:")) != -1){
        // printf("Read command line option:\n");
        // printf("  argument = %c\n", argument);
        // printf("  optopt   = %c\n", optopt);
        // printf("  optarg   = %s\n\n", optarg);

        if (argument == 'f'){
            fullscreen = true;
        } else if (argument == 'w'){
            fullscreen = false;
        } else if (argument == 'i'){
            interactive = true;
        } else if (argument == 'x'){
            Profile::fxaa = true;
            std::string fxaa_level_str(optarg);
            Profile::fxaa_level = std::stoi(fxaa_level_str);
        } else if (argument == 'd'){
            debug = true;
        } else if (argument == 'm'){
            has_map = true;
            map_filename = std::string(optarg);
        } else {
            printf("\nCommand line options:\n");
            printf("\t-f\n");
            printf("\t\tRun in fullscreen mode.\n");
            printf("\t-w\n");
            printf("\t\tRun in windowed mode.\n");
            printf("\t-i\n");
            printf("\t\tRun in interactive mode.\n");
            printf("\t-d\n");
            printf("\t\tShow the debug log.\n\n");
            printf("\t-m <map_filename>\n");
            printf("\t\tLoad the world with map <map_filename>.\n\n");
            printf("\t-x <level>\n");
            printf("\t\tTurn on FXAA with level <level>.\n\n");

            return 1;
        }
    }

    Debug::is_on = debug;

    glfwInit();
    if (fullscreen){
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

        width  = mode->width;
        height = mode->height;

        Debug::info("Loading in fullscreen mode with resolution: %d by %d\n",
            (int)width, (int)height);
    } else {
        width = 1600;
        height = 900;
        Debug::info("Loading in windowed mode with resolution: %d by %d\n",
            (int)width, (int)height);
    }

    // Create the window
    Window window = Window(width, height, fullscreen);
    GLFWwindow* glfw_window = window.getGLFWWindow();

    // Create the world
    World* world;
    if (has_map){
        world = new World(glfw_window, map_filename.c_str());
    } else {
        world = new World(glfw_window);
    }

    // Display loop
    while(!glfwWindowShouldClose(glfw_window)) {
        // Swap display/rendering buffers
        glfwSwapBuffers(glfw_window);

        // Handle events
        glfwPollEvents();
        // Check to see if escape is pressed. If so, close the glfw_window
        if (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
            glfwSetWindowShouldClose(glfw_window, GL_TRUE);
        }
        world->update();
    }

    // Kill glfw to end the program
    glfwTerminate();

    delete world;
    world = NULL;

    // Add a line break before going back to the terminal prompt.
    printf("\n");

    // Nothing went wrong!
    return 0;
}
