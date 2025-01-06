#pragma once

#include "pch.hpp"
#include "helpers.hpp"

enum TileType
{
    EMPTY = 0,
    SPACE,
    CENTER,
    ROOM,
    WALL_LEFT,
    WALL_RIGHT,
    WALL_UP,
    WALL_DOWN,
    WALL_CORNER_DOWN_LEFT,
    WALL_CORNER_DOWN_RIGHT,
    WALL_CORNER_UP_LEFT,
    WALL_CORNER_UP_RIGHT,
    WALL_JUNCTION_DOWN_RIGHT,
    WALL_JUNCTION_DOWN_LEFT,
    WALL_JUNCTION_UP_RIGHT,
    WALL_JUNCTION_UP_LEFT,
    ENTRANCE_LEFT,
    ENTRANCE_RIGHT,
    ENTRANCE_UP,
    ENTRANCE_DOWN,
};

enum RoomShape
{
    L_SHAPE = 0,
    T_SHAPE,
    RECTANGLE,
    ROOM_SHAPE_COUNT
};

struct VectorHash {
    std::size_t operator()(const sf::Vector2i& vec) const {
        return std::hash<int>()(vec.x) ^ std::hash<int>()(vec.y);
    }
};

struct Tile
{
    sf::Sprite sprite;
    sf::Vector2i point;
    TileType type;

    Tile(sf::Texture &tileset):
        sprite(tileset),
        type(EMPTY)
    {}

    Tile(sf::Texture &tileset, sf::Vector2i _point, TileType _type):
        sprite(tileset),
        point(_point),
        type(_type)
    {}
};

struct Room
{
    std::vector<Tile> tiles;

    Room(const std::vector<sf::IntRect>& rects, sf::Texture &tileset) {
        std::unordered_set<sf::Vector2i, VectorHash> points;
        for (const auto& rect: rects)
            for (i32 y = 0; y < rect.size.y; y++)
                for (i32 x = 0; x < rect.size.x; x++)
                    points.emplace(rect.position.x + x, rect.position.y + y);

        std::vector<sf::Vector2i> straightWalls;
        for (auto& point: points) {
            TileType type = determineTileType(points, point);
            tiles.emplace_back(tileset, point, type);

            if (type == WALL_LEFT || type == WALL_RIGHT || type == WALL_UP || type == WALL_DOWN)
                straightWalls.push_back(point);
        }

        sf::Vector2i entrancePoint = straightWalls[rand() % straightWalls.size()];
        Tile* entrance = nullptr;
        for (u32 i = 0; entrance == nullptr && i < tiles.size(); i++)
            if (tiles[i].point == entrancePoint)
                entrance = &tiles[i];

        switch (entrance->type) {
            case WALL_LEFT:     entrance->type = ENTRANCE_LEFT;     break;
            case WALL_RIGHT:    entrance->type = ENTRANCE_RIGHT;    break;
            case WALL_UP:       entrance->type = ENTRANCE_UP;       break;
            case WALL_DOWN:     entrance->type = ENTRANCE_DOWN;     break;
            default:                                                break;
        }
    }

    TileType determineTileType(std::unordered_set<sf::Vector2i, VectorHash>& points, const sf::Vector2i& point) {
        sf::Vector2i neighbors[8] = {
            { point.x + 1, point.y },     // Right
            { point.x - 1, point.y },     // Left
            { point.x,     point.y + 1 }, // Down
            { point.x,     point.y - 1 }, // Up
            { point.x + 1, point.y + 1 }, // Down-Right
            { point.x - 1, point.y + 1 }, // Down-Left
            { point.x + 1, point.y - 1 }, // Up-Right
            { point.x - 1, point.y - 1 }  // Up-Left
        };

        int mask = 0;
        if (points.find(neighbors[0]) == points.end()) mask |= 1;
        if (points.find(neighbors[1]) == points.end()) mask |= 2;
        if (points.find(neighbors[2]) == points.end()) mask |= 4;
        if (points.find(neighbors[3]) == points.end()) mask |= 8;

        switch (mask) {
            case 5:     return WALL_CORNER_DOWN_RIGHT;  // 0101
            case 6:     return WALL_CORNER_DOWN_LEFT;   // 0110
            case 9:     return WALL_CORNER_UP_RIGHT;    // 1001
            case 10:    return WALL_CORNER_UP_LEFT;     // 1010
            case 1:     return WALL_RIGHT;
            case 2:     return WALL_LEFT;
            case 4:     return WALL_DOWN;
            case 8:     return WALL_UP;
            case 0:
                if (points.find(neighbors[4]) == points.end()) return WALL_JUNCTION_DOWN_RIGHT;
                if (points.find(neighbors[5]) == points.end()) return WALL_JUNCTION_DOWN_LEFT;
                if (points.find(neighbors[6]) == points.end()) return WALL_JUNCTION_UP_RIGHT;
                if (points.find(neighbors[7]) == points.end()) return WALL_JUNCTION_UP_LEFT;
                break;
        }

        return ROOM;
    }
};

struct Layer
{
    std::vector<std::vector<Tile>> tiles;

    Layer(sf::Vector2i mapSize, sf::Texture &tileset) {
        tiles.resize(mapSize.y);
        for (i32 y = 0; y < mapSize.y; y++) {
            tiles[y].reserve(mapSize.x);
            for (i32 x = 0; x < mapSize.x; x++)
                tiles[y].emplace_back(tileset);
        }
    }
};

struct Level : public sf::Drawable
{
    Level(const Level&) = delete;
    Level& operator=(const Level&) = delete;
    Level();
    Level(sf::Vector2i _mapSize, sf::Vector2f tileSize, sf::Vector2f tilesetSize);

    sf::Vector2i mapSize;
    sf::Vector2f tileSize;
    sf::Vector2f tilesetSize;

    std::vector<Layer> layers;

    sf::IntRect center;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    void generate(sf::Texture& tileset);
    sf::IntRect determineTextureRect(TileType type);

    std::vector<Room> generateRooms(sf::Texture &tileset);
    std::vector<sf::IntRect> createRoomShape(const sf::Vector2i& pos, RoomShape shape);
    bool roomCanBePlaced(std::vector<sf::IntRect>& rects, std::vector<sf::IntRect>& others);

    sf::Sprite* getSprite(sf::Vector2i index);

    bool outOfBounds(sf::Vector2i index);
    bool outOfBounds(sf::IntRect rect);
    sf::Vector2f mapToScreen(sf::Vector2i index);
    sf::Vector2i screenToMap(sf::Vector2f point);
};
