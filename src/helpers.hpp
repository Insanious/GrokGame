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

inline sf::Vector2i bottomRightIntRect(sf::IntRect rect)
{
    return { rect.position.x + rect.size.x, rect.position.y + rect.size.y };
}

inline i32 rectRight(sf::IntRect rect)
{
    return rect.position.x + rect.size.x;
}

inline i32 rectBottom(sf::IntRect rect)
{
    return rect.position.y + rect.size.y;
}

inline std::vector<sf::Vector2i> pointsIntRect(sf::IntRect rect)
{
    std::vector<sf::Vector2i> points(rect.size.x * rect.size.y);
    sf::Vector2i bottomRight = bottomRightIntRect(rect);
    for (i32 y = rect.position.y; y < bottomRight.y; y++)
        for (i32 x = rect.position.x; x < bottomRight.x; x++)
            points.emplace_back(x, y);

    return points;
}

inline sf::IntRect oversizeIntRect(sf::IntRect rect)
{
    return sf::IntRect({ rect.position.x - 2, rect.position.y - 2 }, { rect.size.x + 4, rect.size.y + 4 });
}