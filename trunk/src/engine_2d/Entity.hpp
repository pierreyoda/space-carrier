#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "CollisionTable.hpp"

/** \brief An abstract class representing a game entity.
*An entity is defined mainly by a position, an angle and a string identifier (or ID).
*/
class Entity
{
    public:
        virtual ~Entity();

        /** \brief Get the position of the entity.
        *
        * \return Entity's position.
        *
        */
        const sf::Vector2f &pos() const;
        /** \brief Set the position of the entity.
        *
        * \param pos Entity's new vector position.
        *
        */
        void setPos(const sf::Vector2f &pos);
        /** \brief Set the position of the entity.
        *
        * \param x Entity's new x position.
        * \param y Entity's new y position.
        *
        */
        void setPos(const float &x, const float &y);

        /** \brief Get the angle of the entity.
        *
        * \return Entity's angle with the abscissa (in radians).
        *
        */
        float angle() const;
        /** \brief Set the angle of the entity.
        *
        * \param angle Entity's new angle (in radians).
        *
        */
        void setAngle(const float &angle);
        /** Change the angle of the entity.
        *
        * \param diff Difference (in radians).
        *
        */
        void changeAngle(const float &diff);

        /** \brief Get the speed of the entity.
        *
        * \return Entity's speed.
        *
        */
        float speed() const;
        /** \brief Set the speed of the entity.
        *
        * \param speed Entity's new speed.
        *
        */
        void setSpeed(const float &speed);
            /** Change the speed of the entity.
        *
        * \param diff Difference.
        *
        */
        void changeSpeed(const float &diff);

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
        virtual void render(sf::RenderTarget&) = 0;

        /** \brief Update the entity.
        *
        * \param elapsedTime Time elapsed since the last update (i.e. the last frame).
        *
        * \return False if must be deleted, true otherwise.
        *
        */
        virtual bool update(const sf::Uint32&) = 0;

        /** \brief Get the entity size.
        *
        * \return Entity's size.
        *
        */
        virtual sf::Vector2f getSize() const = 0;

        /** \brief Transform a point in object local coordinates.
        *
        */
        virtual sf::Vector2f transformToLocal(const sf::Vector2f&) const = 0;

        /** \brief Transform a local point in global coordinates.
        *
        */
        virtual sf::Vector2f transformToGlobal(const sf::Vector2f&) const = 0;

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
        * \param angle Angle with the abscissa.
        * \param speed Speed, 0 by default.
        *
        */
        Entity(const std::string &id, const sf::Vector2f &pos = sf::Vector2f(),
            const float &angle = 0.f, const float &speed = 0.f);

        const std::string m_id;
        sf::Vector2f m_pos;
        float m_angle, m_speed;
        CollisionTable m_collisionTable;
};

#endif /* ENTITY_HPP */
