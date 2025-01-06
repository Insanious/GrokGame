#pragma once

#include "pch.hpp"
#include "Level.hpp"
#include "PlayerManager.hpp"

struct Game
{
    sf::RenderWindow window;
    sf::View view;
    sf::Texture tileset;
    sf::Texture playerTexture;
    Level level;
    PlayerManager playerManager;
    sf::Sprite pointer;
    sf::Sprite* highlighted;

    Game() = delete;
    Game(u32 x, u32 y);

    void run(int framesPerSeconds=60);

    void update(sf::Time dt);
    void draw();

};
