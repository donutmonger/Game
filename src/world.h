#ifndef World_h
#define World_h

#include <vector>
#include <random>
#include <math.h>

#include "game_view.h"
#include "game_view_edit.h"
#include "debug.h"
#include "window.h"
#include "level.hpp"

using namespace std;

class World{
public:
    World(string, bool);
    ~World();

    void update();

private:
    Level level;
    GameView* game_view;
    RenderStack render_stack;

    bool edit_mode;
};

#endif
