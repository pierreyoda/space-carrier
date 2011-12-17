#ifndef ENTITYMANAGER_HPP
#define ENTITYMANAGER_HPP

#include <list>
#include <string>
#include <SFML/Config.hpp>
#include "EntityCollisions.hpp"

struct Entity;
namespace sf
{
    class RenderTarget;
}

/** \brief Tool class to handle all game entities and their interactions.
*
*/
class EntityManager
{
    public:
        EntityManager();
        ~EntityManager();

        /** \brief Add an entity to manage.
        *Note : ownership does not belong to EntityManager, user must delete the entity himself.
        *
        * \param entity Pointer to the Entity.
        *
        * \return True if successful, false otherwise.
        *
        */
        bool addEntity(Entity *entity);

        /** \brief Remove an entity from the list.
        *Warning : entity won't be deleted, user must do it himself.
        *
        * \param Entity's ID.
        *
        * \return True if successful, false otherwise.
        *
        */
        bool removeEntity(const std::string &id);

        /** \brief Render all entities to the specified target.
        *
        * \param target Target where the entities will be rendered.
        *
        */
        void renderAll(sf::RenderTarget &target);

        /** \brief Update all entities.
        *
        * \param elapsedTime Time elapsed since the last update (i.e. the last frame).
        *
        */
        void updateAll(const sf::Uint32 &elapsedTime);

    private:
        EntityManager(const EntityManager &copy) { } // non copyable

        Entity *findEntity(const std::string &id);

        std::list<Entity*> m_entities;
};

#endif /* ENTITYMANAGER_HPP */
