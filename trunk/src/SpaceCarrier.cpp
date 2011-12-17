#include "SpaceCarrier.hpp"
#include "python/PythonEmbedder.hpp"

using namespace std;
namespace fs = boost::filesystem3;

SpaceCarrier::SpaceCarrier() : engine(sf::Vector2i(800, 600), "Space Carrier - v. pre-dev")
{

}

SpaceCarrier::~SpaceCarrier()
{
    pythonEmbedder.Release();
}

void SpaceCarrier::launch()
{
    namespace bp = boost::python;

    bool sucess = false;
    try
    {
        pythonEmbedder.Reset(new PythonEmbedder());
        if (pythonEmbedder->init())
        {
            fs::path file_path = fs::current_path() / "game.py";
            /*if (!fs::is_regular_file(file_path)) // non-existent or invalid
                throw string("cannot load script file '" + file_path.string()
                    + "' because it is invalid or does not exist.");*/
            bp::object script;
            pythonEmbedder->importModule(file_path, script);
            /*script = bp::exec_file(bp::str(file_path.string()), pythonEmbedder->mainModule(),
                pythonEmbedder->mainModule()); */ // CAUSES WINDOWS CRASH...
            if (!script.is_none() && (sucess =
                script.attr("SpaceCarrier_initScript")(boost::ref(engine))))
                engine.run();
        }
    }
    catch (const string &error)
    {
        cerr << "[ERROR] An exception has occured :\n" << error
            << endl << endl;
        sucess = false;
    }
    catch (const exception &error)
    {
        cerr << "[ERROR] An exception has occured :\n" << error.what()
            << endl << endl;
        sucess = false;
    }
    catch (const bp::error_already_set&)
    {
        /*
        // needed when loading 'script' package directly from C++
        std::cerr << "Print simple traceback (0) or try to translate the Python error (1)?\n";
        int simple_print;
        do
        {
            std::cin >> simple_print;
        } while (simple_print != 0 && simple_print != 1);

        if (simple_print == 1 && pythonEmbedder->isPythonException())
            cerr << pythonEmbedder->parsePythonException() << endl;
        else
            PyErr_Print();
        //*/
        //*
        if (pythonEmbedder->isPythonException())
            cerr << pythonEmbedder->parsePythonException() << endl;
        else
            PyErr_Print();
        sucess = false;
        //*/
    }
    if (!sucess)
    {
        cerr << "Error while running Space Carrier, game will now exit." << endl << endl;
        exit(1);
    }
}
