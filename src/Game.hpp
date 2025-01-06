#pragma once

#include "pch.hpp"
#include "Level.hpp"

struct Game
{
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game(u32 x, u32 y);

    void run(int framesPerSeconds=60);

    void update(sf::Time dt);
    void draw();

    sf::RenderWindow window;
    sf::View view;

    sf::Texture tileset;
    Level level;
    sf::RectangleShape pointer;
    sf::RectangleShape* highlighted;
};