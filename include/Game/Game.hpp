<<<<<<< HEAD
#pragma once

#include <memory>
#include <vector>

#include "Debug/debug.hpp"

#include "Game/ObjectState.hpp"
#include "Game/Maps/Map.hpp"
#include "Game/Entities/Entitiy.hpp"

#include "Game/Maps/MapFactory.hpp"

class Game {
public:
    Game();
    void update();
    
    void setMap(const std::string name);
    Map& getMap();

    void addEntity(std::shared_ptr<Entity>);

    std::vector<const ObjectState*> getStates();
private:
    std::vector<std::shared_ptr<Entity>> entities_;
    std::shared_ptr<Map> map_;
=======
// Game.hpp
#pragma once

class Game {
    public:
    Game();
    ~Game();

    

    
    private:

>>>>>>> origin/main
};