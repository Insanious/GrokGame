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

            sf::Vector2i room({ 0, 0 });
            sf::Vector2i center({ 32, 0 });
            sf::Vector2i space({ 96, 0 });
            sf::IntRect rect;
            switch (map[y][x]) {
                case SPACE:      rect = sf::IntRect(space, sf::Vector2i(TILESET_SIZE));   break;
                case ROOM:      rect = sf::IntRect(room, sf::Vector2i(TILESET_SIZE));   break;
                case CENTER:    rect = sf::IntRect(center, sf::Vector2i(TILESET_SIZE)); break;
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
            rooms.push_back(room);
            for (const auto& rect : rects)
                for (const auto& point : rectPoints(rect))
                    map[point.y][point.x] = ROOM;
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
