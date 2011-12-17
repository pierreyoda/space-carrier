#include <SFML/Graphics.hpp>
#include "Entity.hpp"

Entity::Entity(const std::string &id, const sf::Vector2f &pos,
    const float &angle, const float &speed) : m_id(id), m_pos(pos),
    m_angle(angle), m_speed(speed)
{

}

Entity::~Entity()
{

}

float Entity::angle() const
{
    return m_angle;
}

void Entity::setAngle(const float &angle)
{
    m_angle = angle;
}

void Entity::changeAngle(const float &diff)
{
    m_angle += diff;
}

float Entity::speed() const
{
    return m_speed;
}

void Entity::setSpeed(const float &speed)
{
    m_speed = speed;
}

void Entity::changeSpeed(const float &diff)
{
    m_speed += diff;
}

const sf::Vector2f &Entity::pos() const
{
    return m_pos;
}

void Entity::setPos(const sf::Vector2f &pos)
{
    m_pos = pos;

}
void Entity::setPos(const float &x, const float &y)
{
    m_pos.x = x, m_pos.y = y;
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
