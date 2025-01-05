#include "pch.hpp"

inline void printVector2i(std::string prefix, sf::Vector2i vec)
{
    printf("%s: %d, %d\n", prefix.c_str(), vec.x, vec.y);
}

inline void printIntRect(std::string prefix, sf::IntRect rect)
{
    i32 right = rect.position.x + rect.size.x;
    i32 bottom = rect.position.y + rect.size.y;
    printf("%s: %d-%d, %d-%d\n", prefix.c_str(), rect.position.x, right, rect.position.y, bottom);
}

inline i32 rectRight(sf::IntRect rect)
{
    return rect.position.x + rect.size.x;
}

inline i32 rectBottom(sf::IntRect rect)
{
    return rect.position.y + rect.size.y;
}

inline std::vector<sf::Vector2i> rectPoints(sf::IntRect rect)
{
    std::vector<sf::Vector2i> points(rect.size.x * rect.size.y);
    for (i32 y = 0; y < rect.size.y; y++) {
        for (i32 x = 0; x < rect.size.x; x++) {
            points[y * rect.size.x + x] = sf::Vector2i(rect.position.x + x, rect.position.y + y);
        }
    }

    return points;
}

inline sf::IntRect oversizeRect(sf::IntRect rect)
{
    return sf::IntRect({ rect.position.x - 2, rect.position.y - 2 }, { rect.size.x + 4, rect.size.y + 4 });
}