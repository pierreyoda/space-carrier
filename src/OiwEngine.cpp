#include <iostream>
#include "OiwEngine.hpp"

using namespace sf;

OiwEngine::OiwEngine(const Vector2i &windowSize, const std::string &windowTitle) :
    App(VideoMode(windowSize.x, windowSize.y, 32), windowTitle),
    m_currentState(0)
{
    App.setFramerateLimit(60);
    App.setVerticalSyncEnabled(true);
    textureMgr.setLoadingFailureStrategy(thor::Resources::ThrowException);
}

OiwEngine::~OiwEngine()
{

}

bool OiwEngine::run()
{
    sf::Clock frameClock;
    while (App.isOpen())
    {
        // Updating state
        if (m_currentState != 0)
            m_currentState->update(frameClock.restart());

        // Transmitting events to the state
        Event event;
        while (App.pollEvent(event))
        {
            if (event.type == Event::Closed)
                quit();
            else if (m_currentState != 0)
                m_currentState->handleEvent(event);
        }

        // Rendering it
        App.clear();

        if (m_currentState != 0)
            m_currentState->render(App);

        App.display();
    }
    return true;
}

void OiwEngine::quit()
{
    App.close();
    m_currentState = 0;
}

bool OiwEngine::addState(State *state, const std::string &id,
    const bool &setCurrent)
{
    if (state == 0) // null State pointer
        return false;
    if (id.empty()) // empty ID
        return false;
    const State *prev = findState(id);
    if (prev != 0) // ID already used
        return false;
    m_states[id] = state;
    if (setCurrent)
        m_currentState = state;
    return true;
}

 bool OiwEngine::setCurrentState(const std::string &name)
 {
    State *state = findState(name);
    if (state != 0) // found
    {
        m_currentState = state;
        return true;
    }
    return false;
 }

State *OiwEngine::findState(const std::string &id)
{
     if (id.empty())
        return 0;
    std::map<std::string, State*>::iterator result = m_states.find(id);
    if (result != m_states.end())
        return result->second;
    return 0;
}
