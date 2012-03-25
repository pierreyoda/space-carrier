#include <SFML/Graphics.hpp>
#include <thor/Math.hpp>
#include "Entity.hpp"

Entity::Entity(const std::string &id, const sf::Vector2f &pos,
    const float &angle) : sf::Transformable(), sf::Drawable(), m_id(id)
{
    setPosition(pos);
    setRotation(angle);
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

sf::FloatRect Entity::getGlobalBounds() const
{
    return getTransform().transformRect(getLocalBounds());
}

bool Entity::isPixelPresent(const sf::Vector2f &pos) const
{
    sf::FloatRect bounds(getLocalBounds());
    int x = static_cast<int>(bounds.left) + static_cast<int>(pos.x),
        y = static_cast<int>(bounds.top) + static_cast<int>(pos.y);
    if (x < 0 || y < 0 || y >= static_cast<int>(m_collisionTable.pixelsPresent.size())
        || x >= static_cast<int>(m_collisionTable.pixelsPresent[y].size()))
            return false;
    return m_collisionTable.pixelsPresent[y][x];
}

bool Entity::isPixelPresent(const float &x, const float &y) const
{
    return isPixelPresent(sf::Vector2f(x, y));
}
