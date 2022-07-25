#pragma once
#include <list>
#include "ECS.hpp"

class Game
{
private:
    std::list<System *> systems;

public:
    void init();
    void run();
    ~Game();
};