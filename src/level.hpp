#ifndef Level_h
#define Level_h

#include "game_map.hpp"
#include "unit_manager.hpp"
#include "resource_loader.hpp"

using namespace std;

class Level {
public:
    Level(string filename, RenderDeque& render_stack);

    GameMap& getGameMap();
    UnitManager& getUnitManager();
    ResourceLoader& getResourceLoader();

    string asJsonString();

private:
    UnitHolder unit_holder;
    ResourceLoader resource_loader;
    GameMap game_map;
    UnitManager unit_manager;
};


#endif
