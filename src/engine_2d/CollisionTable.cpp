#include <cmath>
#include <SFML/Graphics.hpp>
#include "CollisionTable.hpp"

using namespace sf;

CollisionTable::CollisionTable() : pixelsPresent()
{

}

CollisionTable::CollisionTable(const sf::Image &image,
    const sf::Uint8 &alphaLimit) : pixelsPresent()
{
    for (unsigned int i = 0; i < image.getHeight(); i++)
    {
        pixelsPresent.push_back(std::vector<bool>());
        for (unsigned int j = 0; j < image.getWidth(); j++)
            pixelsPresent[i].push_back(image.getPixel(j, i).a > alphaLimit);
    }
}

CollisionTable::~CollisionTable()
{

}
