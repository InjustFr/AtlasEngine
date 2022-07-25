#include "Game.hpp"

void Game::init()
{
}

void Game::run()
{
    auto it = systems.begin();
    while (it != systems.end())
    {
        (*it)->run(1 / 60);
    }
}

Game::~Game()
{
}