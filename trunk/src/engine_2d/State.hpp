#ifndef STATE_HPP
#define STATE_HPP

#include <SFML/Graphics.hpp>

class OiwEngine;

/** \brief Abstract class to define game states, such as 'Splash Scren', 'Game Menu' or 'InGame'.
*
 */
class State
{
    public:
        virtual ~State();

        /** \brief Init the state. Default behavior : check validity of OiwEngine pointer.
        *
        * \see State::m_engine
        * \return True if successful, false otherwise.
        *
        */
        inline virtual bool init() { return (m_engine != 0); }

        /** \brief Update the state.
        *
        * \param elapsedTime Time elapsed since the last update (i.e. the last frame).
        *
        */
        virtual void update(const sf::Uint32 &elapsedTime) = 0;

        /** \brief Render the state.
        *
        * \param target Rendering target.
        *
        */
        virtual void render(sf::RenderTarget &target) = 0;

        /** \brief Handle a user event.
        *
        * \param Event Event to handle.
        *
        */
        virtual void handleEvent(const sf::Event &Event) = 0;

    protected:
        /** \brief Constructor.
        *
        * \param engine Pointer to the engine.
        *
        */
        State(OiwEngine *engine);

        OiwEngine *m_engine;

        State(const State&); // no copy
        State &operator=(const State&); // no assignment
};

#endif /* STATE_HPP */
