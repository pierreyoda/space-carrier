#ifndef SPACECARRIER_HPP
#define SPACECARRIER_HPP

#include "OiwEngine.hpp"
#include <boost/smart_ptr/scoped_ptr.hpp>

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
        boost::scoped_ptr<PythonEmbedder> pythonEmbedder;
};

#endif /* SPACECARRIER_HPP */
