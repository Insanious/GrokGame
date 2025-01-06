#include "Level.hpp"

Level::Level(sf::Vector2i _mapSize, sf::Vector2f _tileSize, sf::Vector2f _tilesetSize):
    mapSize(_mapSize),
    tileSize(_tileSize),
    tilesetSize(_tilesetSize)
{}

void Level::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const auto& layer: layers) {
        for (u32 y = 0; y < layer.tiles.size(); y++) {
            for (u32 x = 0; x < layer.tiles[y].size(); x++) {
                if (layer.tiles[y][x].type == EMPTY)
                    continue;

                target.draw(layer.tiles[y][x].sprite, states);
            }
        }
    }
}

void Level::generate(sf::Texture& tileset) {
    layers.clear();
    Tile* tile = nullptr;
    Layer groundLayer(mapSize, tileset);

    sf::Vector2i centerSize({ mapSize.x / 4 + 1, mapSize.y / 4 + 1 });
    center = sf::IntRect({ mapSize.x / 2 - centerSize.x + 1, mapSize.y / 2 - centerSize.y + 1 }, centerSize);

    for (i32 y = center.position.y; y < center.position.y + center.size.y; y++)
        for (i32 x = center.position.x; x < center.position.x + center.size.x; x++)
            groundLayer.tiles[y][x].type = CENTER;

    for (i32 y = 0; y < mapSize.y; y++) {
        for (i32 x = 0; x < mapSize.x; x++) {
            tile = &groundLayer.tiles[y][x];
            if (tile->type == EMPTY)
                tile->type = SPACE;

            tile->sprite.setPosition(mapToScreen({ x, y }));
            tile->sprite.setTextureRect(determineTextureRect(tile->type));
        }
    }
    layers.push_back(groundLayer);

    Layer roomLayer(mapSize, tileset);
    std::vector<Room> rooms = generateRooms(tileset);
    for (const auto& room: rooms)
        for (const auto& tile: room.tiles)
            roomLayer.tiles[tile.point.y][tile.point.x] = tile;

    for (i32 y = 0; y < mapSize.y; y++) {
        for (i32 x = 0; x < mapSize.x; x++) {
            tile = &roomLayer.tiles[y][x];
            tile->sprite.setPosition(mapToScreen({ x, y }));
            tile->sprite.setTextureRect(determineTextureRect(tile->type));
        }
    }

    layers.push_back(roomLayer);
}

sf::IntRect Level::determineTextureRect(TileType type) {
    sf::Vector2i size(tilesetSize);

    switch (type) {
        case EMPTY:                     return sf::IntRect({ size.x * 2, size.y * 23 }, size);
        case SPACE:                     return sf::IntRect({ size.x * 3, size.y * 0 }, size);
        case CENTER:                    return sf::IntRect({ size.x * 8, size.y * 0 }, size);
        case ROOM:                      return sf::IntRect({ size.x * 0, size.y * 0 }, size);
        case WALL_LEFT:                 return sf::IntRect({ size.x * 3, size.y * 3 }, size);
        case WALL_RIGHT:                return sf::IntRect({ size.x * 0, size.y * 3 }, size);
        case WALL_UP:                   return sf::IntRect({ size.x * 1, size.y * 3 }, size);
        case WALL_DOWN:                 return sf::IntRect({ size.x * 2, size.y * 3 }, size);
        case WALL_CORNER_DOWN_LEFT:     return sf::IntRect({ size.x * 6, size.y * 10 }, size);
        case WALL_CORNER_DOWN_RIGHT:    return sf::IntRect({ size.x * 4, size.y * 10 }, size);
        case WALL_CORNER_UP_LEFT:       return sf::IntRect({ size.x * 7, size.y * 10 }, size);
        case WALL_CORNER_UP_RIGHT:      return sf::IntRect({ size.x * 5, size.y * 10 }, size);
        case WALL_JUNCTION_DOWN_RIGHT:  return sf::IntRect({ size.x * 1, size.y * 4 }, size);
        case WALL_JUNCTION_DOWN_LEFT:   return sf::IntRect({ size.x * 2, size.y * 4 }, size);
        case WALL_JUNCTION_UP_RIGHT:    return sf::IntRect({ size.x * 0, size.y * 4 }, size);
        case WALL_JUNCTION_UP_LEFT:     return sf::IntRect({ size.x * 3, size.y * 4 }, size);
        case ENTRANCE_LEFT:             return sf::IntRect({ size.x * 10, size.y * 7 }, size);
        case ENTRANCE_RIGHT:            return sf::IntRect({ size.x * 10, size.y * 1 }, size);
        case ENTRANCE_UP:               return sf::IntRect({ size.x * 11, size.y * 7 }, size);
        case ENTRANCE_DOWN:             return sf::IntRect({ size.x * 11, size.y * 1 }, size);
        default:                        return sf::IntRect({ size.x * 2, size.y * 23 }, size);
    }
}

std::vector<Room> Level::generateRooms(sf::Texture &tileset) {
    std::vector<Room> rooms;
    std::vector<sf::IntRect> rects;

    i32 maxAttempts = 200;
    i32 attempts = 0;
    while (attempts++ < maxAttempts && rooms.size() < 12) {
        sf::Vector2i pos({
            1 + (rand() % mapSize.x),
            1 + (rand() % mapSize.y)
        });
        RoomShape shape = static_cast<RoomShape>(rand() % ROOM_SHAPE_COUNT);

        auto roomRects = createRoomShape(pos, shape);
        if (roomCanBePlaced(roomRects, rects)) {
            rooms.emplace_back(roomRects, tileset);
            rects.insert(rects.end(), roomRects.begin(), roomRects.end());
        }
    }

    return rooms;
}

std::vector<sf::IntRect> Level::createRoomShape(const sf::Vector2i& pos, RoomShape shape) {
    switch (shape) {
        case L_SHAPE: {
            sf::Vector2i baseSize(5, 9);
            sf::Vector2i flippedSize(baseSize.y, baseSize.x);
            sf::IntRect top, bottom;
            switch (rand() % 4)
            {
                case 0: // normal L
                    top = sf::IntRect(pos, baseSize);
                    bottom = sf::IntRect({ pos.x, pos.y + baseSize.y }, flippedSize);
                    break;
                case 1: // vertical flip L (F)
                    top = sf::IntRect(pos, flippedSize);
                    bottom = sf::IntRect({ pos.x, pos.y + baseSize.x }, baseSize);
                    break;
                case 2: // horizontal flip L (J)
                    top = sf::IntRect(pos, baseSize);
                    bottom = sf::IntRect({ pos.x - baseSize.x + 1, pos.y + baseSize.y }, flippedSize);
                    break;
                case 3: // double flip L (7)
                    top = sf::IntRect(pos, flippedSize);
                    bottom = sf::IntRect({ pos.x + baseSize.x - 1, pos.y + baseSize.x }, baseSize);
                    break;
            }

            return std::vector<sf::IntRect>({ top, bottom });
        }

        case T_SHAPE: {
            sf::Vector2i baseSize(9, 5);
            sf::Vector2i flippedSize(baseSize.y, baseSize.x);
            sf::IntRect top, bottom;
            switch (rand() % 4)
            {
                case 0: // normal T
                    top = sf::IntRect(pos, baseSize);
                    bottom = sf::IntRect({ pos.x + 2, pos.y + baseSize.y }, flippedSize);
                    break;
                case 1: // flipped T
                    top = sf::IntRect(pos, baseSize);
                    bottom = sf::IntRect({ pos.x + 2, pos.y - baseSize.x }, flippedSize);
                    break;
                case 2: // left T
                    top = sf::IntRect(pos, flippedSize);
                    bottom = sf::IntRect({ pos.x + baseSize.y, pos.y + 2 }, baseSize);
                    break;
                case 3: // left T
                    top = sf::IntRect(pos, flippedSize);
                    bottom = sf::IntRect({ pos.x - baseSize.x, pos.y + 2 }, baseSize);
                    break;
            }

            return std::vector<sf::IntRect>({ top, bottom });
        }

        case RECTANGLE: {
            sf::Vector2i baseSize(13, 7);
            sf::Vector2i flippedSize(baseSize.y, baseSize.x);
            sf::IntRect rect(pos, rand() % 2 == 1 ? baseSize : flippedSize);
            return std::vector<sf::IntRect>({ rect });
        }

        case ROOM_SHAPE_COUNT: break;
    }

    return std::vector<sf::IntRect>();
}

bool Level::roomCanBePlaced(std::vector<sf::IntRect>& rects, std::vector<sf::IntRect>& others) {
    for (const auto& rect : rects) {
        if (outOfBounds(rect))
            return false;

        sf::IntRect oversized({ rect.position.x - 2, rect.position.y - 2 }, { rect.size.x + 4, rect.size.y + 4 });
        if (oversized.findIntersection(center) != std::nullopt)
            return false;

        for(const auto& other: others)
            if (oversized.findIntersection(other) != std::nullopt)
                return false;
    }

    return true;
}

sf::Sprite* Level::getSprite(sf::Vector2i index) {
    sf::Vector2i adjusted({ index.x + 1, index.y + 1 });
    if (outOfBounds(adjusted))
        return nullptr;

    for (i32 i = layers.size() - 1; i >= 0; i--)
        if (layers[i].tiles[adjusted.y][adjusted.x].type != EMPTY)
            return &layers[i].tiles[adjusted.y][adjusted.x].sprite;

    return nullptr;
}

bool Level::outOfBounds(sf::Vector2i index) {
    return index.x < 0 || index.x >= mapSize.x - 1 || index.y < 0 || index.y >= mapSize.y - 1;
}

bool Level::outOfBounds(sf::IntRect rect) {
    return rect.position.x < 0
        || rect.position.x + rect.size.x >= mapSize.x - 1
        || rect.position.y < 0
        || rect.position.y + rect.size.y >= mapSize.y - 1;
}

sf::Vector2f Level::mapToScreen(sf::Vector2i index) {
    return sf::Vector2f(
        (index.x - index.y) * tileSize.x / 2,
        (index.x + index.y) * tileSize.y / 2
    );
}

sf::Vector2i Level::screenToMap(sf::Vector2f point) {
    return sf::Vector2i(
        (point.x / tileSize.x + point.y / tileSize.y),
        (point.y / tileSize.y - point.x / tileSize.x)
    );
}
