/*
Space Carrier
Copyright (C) 2011-2012  Pierre-Yves Diallo (Pierreyoda)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software

Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
Website: https://sourceforge.net/projects/openinternalwar/<br />
E-mail: pierreyoda33@gmail.com
*/
/**
* \mainpage Space Carrier
* Space Carrier is a top view action game where you control a mothership able to launch fighters.
* It mainly uses SFML2, boost and Thor libraries, and features python dynamic scripting.
*
* Copyright (C) 2011-2012  Pierre-Yves Diallo (Pierreyoda).
*
* Website: ...
* E-mail: pierreyoda33@gmail.com
*
* License : GPL (v3) - See main.cpp or gpl3.txt<br />
* \endcode
*/

#include <iostream>
#include "python/PythonEmbedder.hpp"
#include "SpaceCarrier.hpp"

using namespace std;
using namespace boost::python;
namespace fs = boost::filesystem3;

BOOST_PYTHON_MODULE(sf)
{
    PythonEmbedder::exportSfWindow();
    PythonEmbedder::exportSf();
}
BOOST_PYTHON_MODULE(thor)
{
    PythonEmbedder::exportThor();
}
BOOST_PYTHON_MODULE(OiwEngine)
{
    PythonEmbedder::exportOiwEngine();
}
BOOST_PYTHON_MODULE(SpaceCarrier)
{
    PythonEmbedder::exportSpaceCarrier();
}

using namespace std;

int main(int argc, char **argv)
{
    // Python stuff
    PyImport_AppendInittab("sf", &PyInit_sf);
    PyImport_AppendInittab("thor", &PyInit_thor);
    PyImport_AppendInittab("OiwEngine", &PyInit_OiwEngine);
    PyImport_AppendInittab("SpaceCarrier", &PyInit_SpaceCarrier);
    // running game
    SpaceCarrier game;
    game.launch();

    return 0;
}
