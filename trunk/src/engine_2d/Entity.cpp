#include <SFML/Graphics.hpp>
#include <thor/Math.hpp>
#include "Entity.hpp"

Entity::Entity(const std::string &id, const sf::Vector2f &pos,
    const float &angle) : sf::Drawable(), sf::Transformable(), m_id(id)
{
    SetPosition(pos);
    SetRotation(angle);
}

Entity::~Entity()
{

}

const CollisionTable &Entity::collisionTable() const
{
    return m_collisionTable;
}

void Entity::setCollisionTable(const CollisionTable &table)
{
    m_collisionTable = table;
}

const std::string &Entity::id() const
{
    return m_id;
}

sf::IntRect Entity::getSubRect() const
{
    return sf::IntRect(sf::Vector2i(), static_cast<sf::Vector2i>(getSize()));
}

bool Entity::isPixelPresent(const sf::Vector2f &pos) const
{
    sf::IntRect subrect = getSubRect();
    int x = subrect.Left + static_cast<int>(pos.x),
        y = subrect.Top + static_cast<int>(pos.y);
    if (x < 0 || y < 0 || y >= static_cast<int>(m_collisionTable.pixelsPresent.size())
        || x >= static_cast<int>(m_collisionTable.pixelsPresent[y].size()))
            return false;
    return m_collisionTable.pixelsPresent[y][x];
}

bool Entity::isPixelPresent(const float &x, const float &y) const
{
    return isPixelPresent(sf::Vector2f(x, y));
}
