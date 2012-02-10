#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Time.hpp>
#include "CollisionTable.hpp"

/** \brief An abstract class representing a game entity.
*An entity is defined mainly by a position, an angle (in degrees) and a string identifier (or ID).
*Inherits from both sf::Drawable and sf::Transformable.
*/
class Entity : public sf::Transformable, public sf::Drawable
{
    public:
        virtual ~Entity();

        /** \brief Get the identifier.
        *
        * \return Entity's ID.
        *
        */
        const std::string &id() const;

        /** \brief Get the Entity's collision table.
        *
        * \return Entity's collision table.
        */
        const CollisionTable &collisionTable() const;

        /** \brief Set the Entity's collision table.
        *
        *Warning : costly operation (vector copying), should be done once at creation.
        *
        * \param table New collision table.
        */
        void setCollisionTable(const CollisionTable &table);

        /** \brief Render the entity to the specified target.
        *
        * \param target Target where the entity will be rendered.
        *
        */
        virtual void Draw(sf::RenderTarget&, sf::RenderStates) const = 0;

        /** \brief Update the entity.
        *
        * \param elapsedTime Time elapsed since the last update (i.e. the last frame).
        *
        * \return False if must be deleted, true otherwise.
        *
        */
        virtual bool update(const sf::Time&) = 0;

        /** \brief Get the entity size.
        *
        * \return Entity's size.
        *
        */
        virtual sf::Vector2f getSize() const = 0;

        /** \brief Get the entity's drawing subrect if existing. By default : compute with entity's size.
        *
        * \return Entity's subrect.
        *
        */
        sf::IntRect getSubRect() const;

        /** \brief Compute if a pixel present here, using the internal collision table.
        *Will not work if image flipped!
        *
        *Warning : no out-of-range check is made.
        *
        * \param pos Pixel position (in local coordinates).
        *
        * \return True if pixel present, false otherwise.
        *
        */
        bool isPixelPresent(const sf::Vector2f &pos) const;
        /** \brief Compute if a pixel present here, using the internal collision table.
        *Will not work if image flipped!
        *
        *Warning : no out-of-range check is made.
        *
        * \param x Pixel X position (in local coordinates).
        * \param y Pixel Y position (in local coordinates).
        *
        * \return True if pixel present, false otherwise.
        *
        */
        bool isPixelPresent(const float &x, const float &y) const;

    protected:
        /** \brief Constructor. Only @a id is mandatory.
        *
        * \param id Identifier (a string).
        * \param pos Position of the entity.
        * \param angle Angle with the abscissa (in degrees).
        *
        */
        Entity(const std::string &id, const sf::Vector2f &pos = sf::Vector2f(),
            const float &angle = 0.f);

        const std::string m_id;
        CollisionTable m_collisionTable;
};

#endif /* ENTITY_HPP */
