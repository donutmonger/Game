#ifndef Terrain_h
#define Terrain_h

#include <SOIL.h>
#include <string>
#include <vector>

#include "mesh.h"
#include "drawable.h"

struct Heightmap {
    unsigned char* image;
    int width;
    int height;
};

class Terrain : public Drawable {
public:
    Terrain (GLuint s, std::string h) : Terrain(s, h, 10.0f) {;}
    Terrain (GLuint, std::string, float);


    int getDepth();
    int getWidth();

    GLfloat getHeight(GLfloat, GLfloat);
    GLfloat getHeightInterpolated(GLfloat, GLfloat);
    glm::vec3 getNormal(GLfloat, GLfloat);

    bool isOnTerrain(GLfloat, GLfloat, GLfloat);

private:

    void updateUniformData();

    Mesh* generateMesh(Heightmap&);
    float getMapHeight(Heightmap&, int, int);

    int getIndex(int, int);

    GLuint width;
    GLuint depth;

    float start_x;
    float start_z;

    float amplification;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;



};

#endif
