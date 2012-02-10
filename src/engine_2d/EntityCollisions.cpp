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

IntRect EntityCollisions::getAABB(const Entity &entity)
{
    // Store the sprite subrect
    IntRect subrect = entity.getSubRect();
    // Bail out early if the sprite isn't rotated
    if (entity.GetRotation() == 0.0f)
        return IntRect(static_cast<int>(entity.GetPosition().x),
                       static_cast<int>(entity.GetPosition().y),
                       static_cast<int>(subrect.Width),
                       static_cast<int>(subrect.Height));
    // Get the local position of each of the points
    float left = subrect.Left,
        top = subrect.Top,
        right = subrect.Left + subrect.Width,
        bottom = subrect.Top + subrect.Height;
    // Calculate sprite corners as global coordinates
    Vector2f A(entity.GetTransform().TransformPoint(Vector2f(left, top))),
        B(entity.GetTransform().TransformPoint(Vector2f(right, top))),
        C(entity.GetTransform().TransformPoint(Vector2f(left, bottom))),
        D(entity.GetTransform().TransformPoint(Vector2f(right, bottom)));

    // Round off to int and set the four corners of our Rect
    int Left = static_cast<int> (minValue(A.x, B.x, C.x, D.x));
    int Top = static_cast<int> (minValue(A.y, B.y, C.y, D.y));
    int Right = static_cast<int> (maxValue(A.x, B.x, C.x, D.x));
    int Bottom = static_cast<int> (maxValue(A.y, B.y, C.y, D.y));

    return IntRect(Left, Top, Right-Left, Bottom-Top);
}

float EntityCollisions::minValue(float a, float b, float c, float d)
{
    return std::min(std::min(a, b), std::min(c, d));
}

float EntityCollisions::maxValue(float a, float b, float c, float d)
{
    return std::max(std::max(a, b), std::max(c, d));
}

template <typename T>
void EntityCollisions::rectOffset(sf::Rect<T> &rect, const T &offsetX, const T &offsetY)
{
    rect.Left += offsetX, rect.Top += offsetY;
}
