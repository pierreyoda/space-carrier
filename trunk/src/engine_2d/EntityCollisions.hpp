#ifndef ENTITYCOLLISIONS_HPP
#define ENTITYCOLLISIONS_HPP

#include <SFML/Graphics/Rect.hpp>

class Entity;

/** \brief Static class providing collision functions between entities.
*
*Most of the code comes from TheNexus's collision class, re-worked and updated by me :
*https://github.com/LaurentGomila/SFML/wiki/SourceSimpleCollisionDetection.
*
*The getAABB function has been fully rewritten : I didn't manage to make it work properly.
*/
struct EntityCollisions
{
    /**  Test for a pixel perfect collision detection between
     * two entities, Rotation and Scale is supported in this test.
     *
     * \param a The first entity.
     * \param b The second entity.
     *
     * \return True if in collision, false otherwise.
     *
     */
    static bool pixelPerfectTest(const Entity &a, const Entity &b);

     /** \brief Generate a Axis-Aligned Bounding Box for broad phase of
     * Pixel Perfect detection.
     *
     * \return IntRect to round off Floating point positions.
     *
     */
    static sf::IntRect getAABB(const Entity &entity);
};

#endif /* ENTITYCOLLISIONS_HPP */
