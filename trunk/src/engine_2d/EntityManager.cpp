#include "EntityManager.hpp"
#include <SFML/Graphics.hpp>
#include "Entity.hpp"

using namespace std;

EntityManager::EntityManager()
{

}

EntityManager::~EntityManager()
{

}

void EntityManager::renderAll(sf::RenderTarget &target)
{
    for (list<Entity*>::iterator it = m_entities.begin();
        it != m_entities.end(); it++)
        (*it)->render(target); // no NULL-check : addEntity is secured, and updateAll already handles that case
}

void EntityManager::updateAll(const sf::Uint32 &elapsedTime)
{
    for (list<Entity*>::iterator it = m_entities.begin();
        it != m_entities.end(); it++)
        if ((*it) == 0 || !(*it)->update(elapsedTime)) // must be erased
            m_entities.erase(it++);
}

bool EntityManager::addEntity(Entity *entity)
{
    if (entity == 0) // null Entity pointer
        return false;
    if (entity->id().empty()) // empty ID
        return false;
    const Entity *prev = findEntity(entity->id());
    if (prev != 0) // ID already used
        return false;
    m_entities.push_back(entity);
    return true;
}

bool EntityManager::removeEntity(const string &id)
{
    for (list<Entity*>::iterator it = m_entities.begin();
        it != m_entities.end(); it++)
        if ((*it)->id() == id)
        {
            m_entities.erase(it);
            return true;
        }
    return false;
}

Entity *EntityManager::findEntity(const string &id)
{
     if (id.empty())
        return 0;
    for (list<Entity*>::iterator it = m_entities.begin();
        it != m_entities.end(); it++)
        if ((*it)->id() == id)
            return (*it);
    return 0;
}
