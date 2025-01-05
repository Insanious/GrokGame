#include "Level.hpp"

Level::Level():
    TILE_SIZE(32, 16),
    TILESET_SIZE(32, 32)
{
    if (!tileset.loadFromFile("resources/tileset_isometric_pack_1bit_white.png")) {
        printf("failed to load\n");
        exit(0);
    }
}

void Level::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (u32 i = 0; i < tiles.size(); i++) {
        for (u32 j = 0; j < tiles[i].size(); j++) {
            target.draw(tiles[i][j], states);
        }
    }
}

void Level::generate(sf::Vector2i mapSize)
{
    map = std::vector<std::vector<TileType>>(mapSize.y, std::vector<TileType>(mapSize.x, SPACE));
    rooms.clear();

    sf::Vector2i centerSize({ mapSize.x / 4, mapSize.y / 4 });
    center = sf::IntRect({ mapSize.x / 2 - centerSize.x + 1, mapSize.y / 2 - centerSize.y + 1 }, centerSize);
    for (i32 y = center.position.y; y < rectBottom(center); y++)
        for (i32 x = center.position.x; x < rectRight(center); x++)
            map[y][x] = CENTER;

    generateRooms(mapSize);

    tiles = std::vector<std::vector<sf::RectangleShape>>(mapSize.x, std::vector<sf::RectangleShape>(mapSize.y));
    for (i32 y = 0; y < mapSize.y; y++) {
        for (i32 x = 0; x < mapSize.x; x++) {
            sf::RectangleShape tile(TILESET_SIZE);
            tile.setPosition(mapToScreen({x, y}));
            tile.setTexture(&tileset);

            sf::Vector2i size({ (i32)TILESET_SIZE.x, (i32)TILESET_SIZE.y });
            sf::IntRect rect({ 0, 0 }, size);
            switch (map[y][x]) {
                case SPACE:                     rect.position = { size.x * 3, size.y * 0 };     break;
                case CENTER:                    rect.position = { size.x * 1, size.y * 0 };     break;
                case ROOM:                      rect.position = { size.x * 0, size.y * 0 };     break;
                case WALL_LEFT:                 rect.position = { size.x * 3, size.y * 3 };     break;
                case WALL_RIGHT:                rect.position = { size.x * 0, size.y * 3 };     break;
                case WALL_UP:                   rect.position = { size.x * 1, size.y * 3 };     break;
                case WALL_DOWN:                 rect.position = { size.x * 2, size.y * 3 };     break;
                case WALL_CORNER_DOWN_LEFT:     rect.position = { size.x * 6, size.y * 10 };    break;
                case WALL_CORNER_DOWN_RIGHT:    rect.position = { size.x * 4, size.y * 10 };    break;
                case WALL_CORNER_UP_LEFT:       rect.position = { size.x * 7, size.y * 10 };    break;
                case WALL_CORNER_UP_RIGHT:      rect.position = { size.x * 5, size.y * 10 };    break;
                case WALL_JUNCTION_DOWN_RIGHT:  rect.position = { size.x * 1, size.y * 4 };     break;
                case WALL_JUNCTION_DOWN_LEFT:   rect.position = { size.x * 2, size.y * 4 };     break;
                case WALL_JUNCTION_UP_RIGHT:    rect.position = { size.x * 0, size.y * 4 };     break;
                case WALL_JUNCTION_UP_LEFT:     rect.position = { size.x * 3, size.y * 4 };     break;
            }
            tile.setTextureRect(rect);
            tiles[y][x] = tile;
        }
    }
}

void Level::generateRooms(sf::Vector2i mapSize)
{
    i32 maxAttempts = 200;
    i32 attempts = 0;
    while (attempts++ < maxAttempts && rooms.size() < 12) {
        sf::Vector2i pos({ 1 + (rand() % mapSize.x), 1 + (rand() % mapSize.y) });
        RoomShape shape = static_cast<RoomShape>(rand() % ROOM_SHAPE_COUNT);

        auto rects = createRoomShape(pos, shape);
        Room room(rects, shape);
        if (roomCanBePlaced(mapSize, room)) {
            room.calculatePoints();
            for (const auto& tile : room.tiles)
                map[tile.point.y][tile.point.x] = tile.type;

            rooms.push_back(room);
        }
    }
}

std::vector<sf::IntRect> Level::createRoomShape(const sf::Vector2i& pos, RoomShape shape)
{
    switch (shape) {
        case L_SHAPE: {
            sf::Vector2i baseSize(4, 8);
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
                    bottom = sf::IntRect({ pos.x - baseSize.x, pos.y + baseSize.y }, flippedSize);
                    break;
                case 3: // double flip L (7)
                    top = sf::IntRect(pos, flippedSize);
                    bottom = sf::IntRect({ pos.x + baseSize.x, pos.y + baseSize.x }, baseSize);
                    break;
            }

            return std::vector<sf::IntRect>({ top, bottom });
        }

        case T_SHAPE: {
            sf::Vector2i baseSize(8, 4);
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
            sf::Vector2i baseSize(12, 6);
            sf::Vector2i flippedSize(baseSize.y, baseSize.x);
            sf::IntRect rect(pos, rand() % 2 == 1 ? baseSize : flippedSize);
            return std::vector<sf::IntRect>({ rect });
        }

        case ROOM_SHAPE_COUNT: break;
    }

    return std::vector<sf::IntRect>();
}

bool Level::roomCanBePlaced(sf::Vector2i mapSize, Room& room)
{
    for (const auto& rect : room.rects)
        if (rect.position.x <= 0 || rectRight(rect) >= mapSize.x - 1 || rect.position.y <= 0 || rectBottom(rect) >= mapSize.y - 1)
            return false;

    for(const auto& other: rooms)
        if(room.overlaps(other))
            return false;

    if (room.overlaps(center))
        return false;

    return true;
}

sf::Vector2f Level::mapToScreen(sf::Vector2i index)
{
    return sf::Vector2f(
        (index.x - index.y) * TILE_SIZE.x / 2,
        (index.x + index.y) * TILE_SIZE.y / 2
    );
}

sf::Vector2i Level::screenToMap(sf::Vector2f point)
{
    return sf::Vector2i(
        (int)(point.x / TILE_SIZE.x + point.y / TILE_SIZE.y),
        (int)(point.y / TILE_SIZE.y - point.x / TILE_SIZE.x)
    );
}
