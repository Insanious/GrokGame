#include "Level.hpp"

Level::Level()
{
    if (!tileset.loadFromFile("resources/tileset_cave_1.png")) {
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
    map = std::vector<std::vector<TileType>>(mapSize.y, std::vector<TileType>(mapSize.x, WALL));
    rooms.clear();

    center = sf::IntRect({ mapSize.x / 2, mapSize.y / 2 }, { mapSize.x / 4, mapSize.y / 4 });
    for (i32 y = center.position.y; y < rectBottom(center); y++)
        for (i32 x = center.position.x; x < rectRight(center); x++)
            map[y][x] = CENTER;

    generateRooms(mapSize);

    tiles = std::vector<std::vector<sf::RectangleShape>>(mapSize.x, std::vector<sf::RectangleShape>(mapSize.y));
    sf::Vector2f size({ TILE_WIDTH, TILE_HEIGHT });
    for (i32 y = 0; y < mapSize.y; y++) {
        for (i32 x = 0; x < mapSize.x; x++) {
            sf::RectangleShape tile(size);
            tile.setPosition(mapToScreen({x, y}));
            tile.setTexture(&tileset);

            sf::Vector2i wall({ 0, 0 });
            sf::Vector2i path({ 64, 32 });
            sf::Vector2i room({ 64, 64 });
            sf::Vector2i center({ 64, 128 });
            sf::Vector2i door({ 64, 128 });
            sf::IntRect rect;
            switch (map[y][x]) {
                case WALL:      rect = sf::IntRect(wall, sf::Vector2i(size));   break;
                case PATH:      rect = sf::IntRect(path, sf::Vector2i(size));   break;
                case ROOM:      rect = sf::IntRect(room, sf::Vector2i(size));   break;
                case CENTER:    rect = sf::IntRect(center, sf::Vector2i(size)); break;
                case DOOR:      rect = sf::IntRect(door, sf::Vector2i(size));   break;
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
    while (rooms.size() < 12 || attempts++ >= maxAttempts) {
        sf::Vector2i pos({ 1 + (rand() % mapSize.x), 1 + (rand() % mapSize.y) });
        RoomShape shape = static_cast<RoomShape>(rand() % ROOM_SHAPE_COUNT);

        auto rects = createRoomShape(pos, shape);
        Room room(rects, shape);
        if (roomCanBePlaced(mapSize, room)) {
            for (const auto& rect : rects)
                for (const auto& point : pointsIntRect(rect))
                    map[point.y][point.x] = ROOM;

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
        (float)(index.x - index.y) * TILE_WIDTH / 2,
        (float)(index.x + index.y) * TILE_HEIGHT / 2
    );
}

sf::Vector2i Level::screenToMap(sf::Vector2f screen)
{
    return sf::Vector2i(
        (int)(screen.x / TILE_WIDTH + screen.y / TILE_HEIGHT),
        (int)(screen.y / TILE_HEIGHT - screen.x / TILE_WIDTH)
    );
}
