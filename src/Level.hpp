#pragma once

#include "pch.hpp"
#include "helpers.hpp"

enum TileType
{
    WALL = 0,
    PATH = 1,
    ROOM = 2,
    CENTER = 3,
    DOOR = 4
};

enum RoomShape
{
    L_SHAPE,
    T_SHAPE,
    RECTANGLE,
    ROOM_SHAPE_COUNT
};

struct Room
{
    std::vector<sf::IntRect> rects;
    RoomShape shape;

    Room(const std::vector<sf::IntRect>& _rects, RoomShape _shape):
        rects(_rects), shape(_shape) {}

    bool overlaps(const Room& other) const {
        for (const auto& rect : rects) {
            sf::IntRect oversized = oversizeIntRect(rect);
            for (const auto& otherRect : other.rects)
                if (oversized.findIntersection(otherRect) != std::nullopt)
                    return true;
        }

        return false;
    }

    bool overlaps(const sf::IntRect& other) const {
        for (const auto& rect : rects) {
            sf::IntRect oversized = oversizeIntRect(rect);
            if (oversized.findIntersection(other) != std::nullopt)
                return true;
        }

        return false;
    }
};

struct Level : public sf::Drawable
{
    Level(const Level&) = delete;
    Level& operator=(const Level&) = delete;
    Level();

    float TILE_WIDTH = 64;
    float TILE_HEIGHT = 32;

    std::vector<std::vector<sf::RectangleShape>> tiles;
    sf::Texture tileset;

    std::vector<std::vector<TileType>> map;
    std::vector<Room> rooms;
    sf::IntRect center;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    void generate(sf::Vector2i mapSize);

    void generateRooms(sf::Vector2i mapSize);
    bool roomCanBePlaced(sf::Vector2i mapSize, Room& room);
    std::vector<sf::IntRect> createRoomShape(const sf::Vector2i& pos, RoomShape shape);

    sf::Vector2f mapToScreen(sf::Vector2i index);
    sf::Vector2i screenToMap(sf::Vector2f vector);
};
