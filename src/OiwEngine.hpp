#ifndef OIWENGINE_HPP
#define OIWENGINE_HPP

#include <map>
#include <Thor/Events.hpp>
#include <Thor/Resources.hpp>
#include "engine_2d/State.hpp"

typedef thor::ResourceManager<sf::Texture> TextureManager;
typedef thor::ResourceManager<sf::Font> FontManager;

/** \brief OIW Engine 'god class'.
*
* Open Internal War engine is an evolution of the Open Advanced War engine, from the same author.
*It uses Thor Engine instead of implementing animations, resource managers...
*/
class OiwEngine
{
    public:
        OiwEngine(const sf::Vector2i &windowSize,
            const std::string &windowTitle);
        ~OiwEngine();

        bool run();
        /** \brief Close the RenderWindow and quit the engine.
        *
        */
        void quit();

        /** \brief Add a State to the engine.
        *Note : ownership does not belong to OiwEngine, user must delete the state himself.
        *
        * \param state Pointer to the state.
        * \param id State's string identifier, example : 'Main Menu', 'SplashScreen'.
        * \param setCurrent Set the added state as current?
        *
        * \return True if successful, false otherwise.
        *
        */
        bool addState(State *state, const std::string &id, const bool &setCurrent);
        /** \brief Set the current engine State.
        *
        * \param id State's string identifier, specified when state was added.
        *
        * \return True if successful, false otherwise.
        *
        */
        bool setCurrentState(const std::string &id);

        FontManager &getFontMgr() { return fontMgr; }
        TextureManager &getTextureMgr() { return textureMgr; }
        sf::RenderWindow &getRenderWindow() { return App; }

    private:
        OiwEngine(const OiwEngine &copy) { } // non copyable

        State *findState(const std::string &id);

        sf::RenderWindow App;
        TextureManager textureMgr;
        FontManager fontMgr;
        std::map<std::string, State*> m_states;
        State *m_currentState;
        bool m_running;
};

#endif /* OIWENGINE_HPP */
