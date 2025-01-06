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
    sf::Vector2i point;
    TileType type;

    Tile(const sf::Vector2i _point, TileType _type): point(_point), type(_type) {}
};

struct Room
{
    std::vector<sf::IntRect> rects;
    std::vector<sf::Vector2i> points;
    std::vector<Tile> tiles;
    RoomShape shape;

    Room(const std::vector<sf::IntRect>& _rects, RoomShape _shape):
        rects(_rects),
        shape(_shape)
    {
        for (const auto& rect: this->rects) {
            std::vector<sf::Vector2i> allRectPoints = rectPoints(rect);
            points.insert(points.end(), allRectPoints.begin(), allRectPoints.end());
        }
    }

    TileType determineTileType(const std::unordered_set<sf::Vector2i, VectorHash>& pointSet, const sf::Vector2i& point) {
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
        if (pointSet.find(neighbors[0]) == pointSet.end()) mask |= 1;
        if (pointSet.find(neighbors[1]) == pointSet.end()) mask |= 2;
        if (pointSet.find(neighbors[2]) == pointSet.end()) mask |= 4;
        if (pointSet.find(neighbors[3]) == pointSet.end()) mask |= 8;

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
                if (pointSet.find(neighbors[4]) == pointSet.end()) return WALL_JUNCTION_DOWN_RIGHT;
                if (pointSet.find(neighbors[5]) == pointSet.end()) return WALL_JUNCTION_DOWN_LEFT;
                if (pointSet.find(neighbors[6]) == pointSet.end()) return WALL_JUNCTION_UP_RIGHT;
                if (pointSet.find(neighbors[7]) == pointSet.end()) return WALL_JUNCTION_UP_LEFT;
                break;
        }

        return ROOM;
    }

    void calculateTileTypes() {
        std::unordered_set<sf::Vector2i, VectorHash> pointSet(points.begin(), points.end());
        std::vector<u32> straightWalls;
        for (u32 i = 0; i < points.size(); i++) {
            TileType type = determineTileType(pointSet, points[i]);
            tiles.emplace_back(points[i], type);

            if (type == WALL_LEFT || type == WALL_RIGHT || type == WALL_UP || type == WALL_DOWN)
                straightWalls.push_back(i);
        }

        u32 entrance = straightWalls[rand() % straightWalls.size()];
        switch (tiles[entrance].type) {
            case WALL_LEFT:     tiles[entrance].type = ENTRANCE_LEFT;     break;
            case WALL_RIGHT:    tiles[entrance].type = ENTRANCE_RIGHT;    break;
            case WALL_UP:       tiles[entrance].type = ENTRANCE_UP;       break;
            case WALL_DOWN:     tiles[entrance].type = ENTRANCE_DOWN;     break;
            default:                                                      break;
        }
    }

    bool overlaps(const Room& other) const {
        for (const auto& rect : rects) {
            sf::IntRect oversized = oversizeRect(rect);
            for (const auto& otherRect : other.rects)
                if (oversized.findIntersection(otherRect) != std::nullopt)
                    return true;
        }

        return false;
    }

    bool overlaps(const sf::IntRect& other) const {
        for (const auto& rect : rects) {
            sf::IntRect oversized = oversizeRect(rect);
            if (oversized.findIntersection(other) != std::nullopt)
                return true;
        }

        return false;
    }
};

struct Layer
{
    std::vector<std::vector<sf::RectangleShape>> rects;
    std::vector<std::vector<TileType>> tiles;

    Layer(sf::Vector2i mapSize, const sf::Texture *texture, sf::Vector2f tileSize) {
        tiles = std::vector<std::vector<TileType>>(mapSize.y, std::vector<TileType>(mapSize.x, EMPTY));
        rects = std::vector<std::vector<sf::RectangleShape>>(mapSize.x, std::vector<sf::RectangleShape>(mapSize.y));
        for (i32 y = 0; y < mapSize.y; y++) {
            for (i32 x = 0; x < mapSize.x; x++) {
                rects[y][x] = sf::RectangleShape(tileSize);
                rects[y][x].setTexture(texture);
            }
        }
    }
};

struct Level : public sf::Drawable
{
    Level(const Level&) = delete;
    Level& operator=(const Level&) = delete;
    Level();

    sf::Vector2f TILE_SIZE;
    sf::Vector2f TILESET_SIZE;

    sf::Texture tileset;
    std::vector<Layer> layers;

    sf::IntRect center;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    void generate(sf::Vector2i mapSize);
    sf::IntRect determineTextureRect(TileType type);

    std::vector<Room> generateRooms(sf::Vector2i mapSize);
    bool roomCanBePlaced(sf::Vector2i mapSize, std::vector<Room>& rooms, Room& room);
    std::vector<sf::IntRect> createRoomShape(const sf::Vector2i& pos, RoomShape shape);

    sf::Vector2f mapToScreen(sf::Vector2i index);
    sf::Vector2i screenToMap(sf::Vector2f point);
};
