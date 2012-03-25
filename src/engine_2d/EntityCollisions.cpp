#include <SFML/Graphics.hpp>
#include "EntityCollisions.hpp"
#include "Entity.hpp"

using namespace sf;

bool EntityCollisions::pixelPerfectTest(const Entity &object1,
    const Entity &object2)
{
    // Get AABBs of the two entities
    sf::IntRect Object1AABB = getAABB(object1),
        Object2AABB = getAABB(object2);

    sf::IntRect intersection;
    if (Object1AABB.intersects(Object2AABB, intersection))
    {
        //We've got an intersection we need to process the pixels
        //In that Rect.
        sf::Vector2i size(intersection.left+intersection.width,
            intersection.top+intersection.height);
        sf::Vector2f o1v;
        sf::Vector2f o2v;
        //Loop through our pixels
        for (int i = intersection.left; i < size.x; i++)
            for (int j = intersection.top; j < size.y; j++)
            {
                o1v = object1.getInverseTransform().transformPoint(sf::Vector2f(i, j)); //Creating Objects each loop :(
                o2v = object2.getInverseTransform().transformPoint(sf::Vector2f(i, j)); //Creating Objects each loop :(
                //If both sprites have opaque pixels at the same point we've got a hit
                if (object1.isPixelPresent(o1v) && object2.isPixelPresent(o2v))
                    return true;
            }
        return false;
    }
    return false;
}

// TODO (Pierre-Yves#1#): [Collisions] getAABB : avoid IntRect/FloatRect conversions (modify Entity::getSize ?)
IntRect EntityCollisions::getAABB(const Entity &entity)
{
    // Just transform the FloatRect global bounds into a IntRect
    FloatRect bounds(entity.getGlobalBounds());
    return IntRect(static_cast<int>(bounds.left),
                   static_cast<int>(bounds.top),
                   static_cast<int>(bounds.width),
                   static_cast<int>(bounds.height));
}
