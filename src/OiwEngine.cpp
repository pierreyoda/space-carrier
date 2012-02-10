#include <iostream>
#include "OiwEngine.hpp"

using namespace sf;

OiwEngine::OiwEngine(const Vector2i &windowSize, const std::string &windowTitle) :
    App(VideoMode(windowSize.x, windowSize.y, 32), windowTitle),
    m_currentState(0)
{
    App.SetFramerateLimit(60);
    App.EnableVerticalSync(true);
    textureMgr.SetLoadingFailureStrategy(thor::Resources::ThrowException);
}

OiwEngine::~OiwEngine()
{

}

bool OiwEngine::run()
{
    sf::Clock frameClock;
    while (App.IsOpen())
    {
        // Updating state
        if (m_currentState != 0)
            m_currentState->update(frameClock.Restart());

        // Transmitting events to the state
        Event Event;
        while (App.PollEvent(Event))
        {
            if (Event.Type == Event::Closed)
                quit();
            else if (m_currentState != 0)
                m_currentState->handleEvent(Event);
        }

        // Rendering it
        App.Clear();

        if (m_currentState != 0)
            m_currentState->render(App);

        App.Display();
    }
    return true;
}

void OiwEngine::quit()
{
    App.Close();
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
