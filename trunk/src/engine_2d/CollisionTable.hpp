#ifndef COLLISIONTABLE_HPP
#define COLLISIONTABLE_HPP

#include <vector>

namespace sf
{
    class Image;
}

typedef std::vector< std::vector<bool> > vector_bool2;

/** \brief Stores Entity's collision data.
*
*That consists of an array of bool (with false = no pixel there) used by collision algorithms.
*
* \author TheNexus
* \author Re-worked and updated algorithms + class by Pierreyoda (me).
*/
struct CollisionTable
{
    /** \brief Default constructor.
    *
    */
    CollisionTable();

    /** \brief Build presence pixel array from a sf::Image.
    *
    * \param image Image from which the array will be built.
    * \param alphaLimit How opaque a pixel needs to be in order to be taken in account. 127 by default.
    *
    */
    CollisionTable(const sf::Image &image, const sf::Uint8 &alphaLimit = 127);

    ~CollisionTable();

    /** \brief Presence pixel array.
    *
    */
    vector_bool2 pixelsPresent;
};

#endif /* COLLISIONTABLE_HPP */
