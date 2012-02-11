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
    if (Object1AABB.Intersects(Object2AABB, intersection))
    {
        //We've got an intersection we need to process the pixels
        //In that Rect.
        sf::Vector2i size(intersection.Left+intersection.Width,
            intersection.Top+intersection.Height);
        sf::Vector2f o1v;
        sf::Vector2f o2v;
        //Loop through our pixels
        for (int i = intersection.Left; i < size.x; i++)
            for (int j = intersection.Top; j < size.y; j++)
            {
                o1v = object1.GetInverseTransform().TransformPoint(sf::Vector2f(i, j)); //Creating Objects each loop :(
                o2v = object2.GetInverseTransform().TransformPoint(sf::Vector2f(i, j)); //Creating Objects each loop :(
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
    return IntRect(static_cast<int>(bounds.Left),
                   static_cast<int>(bounds.Top),
                   static_cast<int>(bounds.Width),
                   static_cast<int>(bounds.Height));
}
