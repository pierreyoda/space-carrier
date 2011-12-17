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

    /** \brief Helper function to get the minimum from a list of values.
    *
    *std::min can be directly used instead with 'initializer lists' (C++11).
    */
    static float minValue(float a, float b, float c, float d);

    /** \brief Helper function to get the maximum from a list of values.
    *
    *std::max can be directly used instead with 'initializer lists' (C++11).
    */
    static float maxValue(float a, float b, float c, float d);

    /** \brief Move the whole rectangle by the given offset.
    *
    * \param rect Rectangle to move.
    * \param offsetX X offset.
    * \param offsetY Y offset.
    *
    */
    template<typename T>
    static void rectOffset(sf::Rect<T> &rect, const T &offsetX, const T &offsetY);
};

#endif /* ENTITYCOLLISIONS_HPP */
