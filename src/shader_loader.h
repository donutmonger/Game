#ifndef ShaderLoader_h
#define ShaderLoader_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>
// MAC
// #include <OpenGL/OpenGL.h>
// LINUX
#include <GL/gl.h>

#include <stdio.h>

class ShaderLoader {
public:
    static GLuint loadVertexShader(const char*);
    static GLuint loadFragmentShader(const char*);
    static GLuint combineShaderProgram(GLuint, GLuint);

private:
    static const char* GLSLParse(const char*);

};

#endif