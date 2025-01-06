#pragma once

#include "pch.hpp"

struct Player
{
    sf::Sprite sprite;
    sf::Time animationTime;
    u32 animations;
    sf::Clock animation;

    Player() = delete;
    Player(sf::Texture& texture, sf::IntRect rect):
        sprite(texture, rect),
        animationTime(sf::seconds(0.1)),
        animations(6)
    {}
};

struct PlayerManager : public sf::Drawable
{
    std::vector<Player*> players;

    PlayerManager();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void update();

    void addPlayer(sf::Texture& texture, sf::Vector2f pos, sf::Vector2i size);
};
