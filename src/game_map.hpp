#ifndef GameMap_h
#define GameMap_h

#include <vector>
#include <functional>
#include <fstream>
#include <iostream>

#include "doodad.h"
#include "terrain.h"
#include "includes/json.hpp"
#include "shadowbuffer.h"

#include "render_deque.hpp"
#include "profile.h"
#include "particles/emitter_factory.hpp"
#include "unit_holder.hpp"
#include "resource_loader.hpp"

using namespace std;

class GameMap {
public:
    GameMap(string map_filename, UnitHolder& unit_holder, RenderDeque& render_stack, ResourceLoader& resource_loader);

    void render();
    void renderToShadowMap();
    void renderToDepthMap();

    glm::vec3 calculateWorldPosition(glm::vec2 screen_pos);
    void addDrawable(Drawable& drawable);

    void setTempDrawable(Drawable& drawable);
    void placeTempDrawable();

    Shadowbuffer& getShadowbuffer();
    Shadowbuffer& getDepthbuffer();

    Camera& getCamera();
    Terrain& getGround();


private:

    void renderAllNoShader();
    void renderAllWithShader(Shader& shader, Framebuffer& buf);

    void load(ifstream& map);

    void initializeGlobalUniforms();
    void updateGlobalUniforms();

    glm::vec3 getIntersection(glm::vec3 line, float plane_height);
    glm::vec3 calculateRay(glm::vec2 screen_point);
    std::tuple<float, float, glm::vec3> findMapPoint(glm::vec3 ray, int steps, float bottom, float top);
    glm::vec3 findMapPointInit(glm::vec3 ray, int steps);

    vector<Doodad> doodads;
    vector<Emitter*> emitters;

    Camera camera;
    Terrain ground;
    UnitHolder* unit_holder;
    RenderDeque* render_stack;
    ResourceLoader* resource_loader;

    // Everything that will be drawn
    vector<Drawable*> drawables;

    // Temporary drawable for adding new things through drawable placer
    bool has_temp_drawable;
    Drawable* temp_drawable;

    Shadowbuffer shadowbuffer;
    Shadowbuffer depthbuffer;

    Shader shadow_shader;
    Shader depth_shader;

    GLuint camera_ubo;
    GLuint shadow_ubo;


};

#endif
