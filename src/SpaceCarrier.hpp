#ifndef SPACECARRIER_HPP
#define SPACECARRIER_HPP

#include "OiwEngine.hpp"
#include <Thor/SmartPtr/ScopedPtr.hpp>

class PythonEmbedder;

/** \brief Main class of Space Carrier game.
*
*/
class SpaceCarrier
{
    public:
        SpaceCarrier();
        ~SpaceCarrier();

        /** \brief Launch the game.
        *
        */
        void launch();

    private:
        OiwEngine engine;
        thor::ScopedPtr<PythonEmbedder> pythonEmbedder;
};

#endif /* SPACECARRIER_HPP */
