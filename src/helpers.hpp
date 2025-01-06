#include "pch.hpp"

inline void printVector2i(std::string prefix, sf::Vector2i vec)
{
    printf("%s: %d, %d\n", prefix.c_str(), vec.x, vec.y);
}

inline void printVector2f(std::string prefix, sf::Vector2f vec)
{
    printf("%s: %f, %f\n", prefix.c_str(), vec.x, vec.y);
}

inline void printIntRect(std::string prefix, sf::IntRect rect)
{
    i32 right = rect.position.x + rect.size.x;
    i32 bottom = rect.position.y + rect.size.y;
    printf("%s: %d-%d, %d-%d\n", prefix.c_str(), rect.position.x, right, rect.position.y, bottom);
}
