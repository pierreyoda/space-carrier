#include <iostream>
#include <vector>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "PythonEmbedder.hpp"

#include "../OiwEngine.hpp"
#include "../engine_2d/Entity.hpp"
#include "../engine_2d/EntityManager.hpp"

// A macro to simplify overloaded member function (=method) binding
#define method(CLASS, METHOD, RTYPE, ARGS...) \
    def(BOOST_PP_STRINGIZE(METHOD), \
        (RTYPE(CLASS::*)(ARGS))&CLASS::METHOD)
// same but with arguments - const version
#define method_const(CLASS, METHOD, RTYPE, ARGS...) \
    def(BOOST_PP_STRINGIZE(METHOD), \
        (RTYPE(CLASS::*)(ARGS)const)&CLASS::METHOD)

using namespace boost::python;
namespace fs = boost::filesystem3;

PythonEmbedder::PythonEmbedder()
{

}

PythonEmbedder::~PythonEmbedder()
{

}

bool PythonEmbedder::init()
{
    try
    {
        Py_SetProgramName(L"Open Internal War");
        Py_Initialize();

        main_module = import("__main__");
        global = main_module.attr("__dict__");

        //PySys_SetPath(fs::current_path().wstring().c_str());
    }
    catch (const error_already_set&)
    {
        if (isPythonException())
            std::cerr << parsePythonException();
        else
            PyErr_Print();
        return false;
    }
    return true;
}

void PythonEmbedder::importModule(const fs::path &path, object &module)
{
    try
    {
        global["modulename"] = path.stem().string();
        global["path"] = path.string();
        exec("import imp\n"
            "newmodule = imp.load_module(modulename, open(path), path, ('py', 'U', imp.PY_SOURCE))\n",
            global, global);
        module = global["newmodule"];
    }
    catch (const error_already_set&)
    {
        if (isPythonException())
            std::cerr << parsePythonException();
        else
            PyErr_Print();
        module = object();
    }
}

bool PythonEmbedder::isPythonException()
{
    return (PyErr_ExceptionMatches(PyExc_BaseException));
}

std::string PythonEmbedder::parsePythonException()
{
    PyObject *exc, *val, *tb;
    PyErr_Fetch(&exc, &val, &tb);
    PyErr_NormalizeException(&exc, &val, &tb);
    handle<> hexc(exc), hval(allow_null(val)), htb(allow_null(tb));
    std::string error;
    if(!hval)
    {
        error += std::string("[ERROR] A Python exception was caught : ")
            + std::string(extract<std::string>(str(hexc))) + "\n";
    }
    else
    {
        error += "\n\t[Intercepted python exception :\n";
        object traceback(import("traceback"));
        object format_exception(traceback.attr("format_exception"));
        list formatted_list(format_exception(hexc,hval,htb));
        for(int count = 0; count < len(formatted_list); ++count)
            error +=
                std::string(extract<std::string>(formatted_list[count].slice(0,-1)))
                + "\n";
        error +="\t]\n";
    }
    return error;
}

/** \brief Python wrapper for State.
Allow Python derived classes to override virtual functions such as State::update.
*/
struct StateWrap : State, wrapper<State>
{
    StateWrap(OiwEngine *engine) : State(engine) { }

    bool init()
    {
        if (override init_ = this->get_override("init"))
            return init_();
        return State::init();
    }
    bool default_init() { return this->State::init(); }

    void update(const sf::Uint32 &elapsedTime)
    {
        this->get_override("update")(elapsedTime);
    }
    void render(sf::RenderTarget &target)
    {
        this->get_override("render")(boost::ref(target));
    }
    void handleEvent(const sf::Event &Event)
    {
        this->get_override("handleEvent")(boost::ref(Event));
    }
};

// TODO (Pierre-Yves#1#): [ENGINE] Entity::update : on delete Python won't delete the object immediately ; find a way to remove dependency
/** \brief Python wrapper for Entity.
Allow Python derived classes to override virtual functions such as Entity::render.
*/
struct EntityWrap : Entity, wrapper<Entity>
{
    EntityWrap(const std::string &id, const sf::Vector2f &pos = sf::Vector2f(),
        const float &angle = 0.f, const float &speed = 0.f) :
        Entity(id, pos, angle, speed)
    { }

    void render(sf::RenderTarget &target)
    {
        this->get_override("render")(boost::ref(target));
    }

    bool update(const sf::Uint32 &elapsedTime)
    {
        return this->get_override("update")(elapsedTime);
    }

    sf::Vector2f getSize() const
    {
        return this->get_override("getSize")();
    }

    sf::Vector2f transformToLocal(const sf::Vector2f &point) const
    {
        return this->get_override("transformToLocal")(point);
    }
    sf::Vector2f transformToGlobal(const sf::Vector2f &point) const
    {
        return this->get_override("transformToGlobal")(point);
    }

    sf::IntRect getSubRect() const
    {
        if (override getSubRect_ = this->get_override("getSubRect"))
            return getSubRect_();
        return Entity::getSubRect();
    }
    sf::IntRect default_getSubRect() const
    {
        return this->Entity::getSubRect();
    }

    /** \brief Position setter. Accepts : sf::Vector2f ; (x,y) tuple where both x and y can be none.
    *That allows syntax : "entity.pos = None, y" (implicit tuple) or "entity.pos = (x, )".
    */
    void setPos(object value)
    {
        float x = pos().x, y = pos().y;
        if (PyTuple_Check(value.ptr()))
        {
            object x_obj(value[0]);
            if (!x_obj.is_none())
                x = extract<float>(x_obj);
            if (len(value) == 1) // only x param
                Entity::setPos(x, 0);
            else
            {
                object y_obj(value[1]);
                if (!y_obj.is_none())
                    y = extract<float>(y_obj);
                Entity::setPos(x, y);
            }
        }
        else if (extract<sf::Vector2f>(value).check())
            m_pos = extract<sf::Vector2f>(value);
        else // incorrect type
        {
            std::string message = std::string("setting ") + m_id.c_str() +
                ".pos :  provided object is not a sf.Vector2f or a tuple";
            PyErr_SetString(PyExc_TypeError, message.c_str());
        }
    }
};

thor::ResourcePtr<sf::Texture> engine_loadtexture(OiwEngine &engine, const thor::Resources::TextureKey &key)
{
    thor::ResourcePtr<sf::Texture> img = engine.getTextureMgr().Acquire(key);
    /*if (!img) // load failed
    {
        PyErr_SetString(PyExc_ValueError, "Failed to load texture.");
        throw_error_already_set();
    }*/
    return img;
}

thor::ResourcePtr<sf::Font> engine_loadfont(OiwEngine &engine, const thor::Resources::FontKey &key)
{
    thor::ResourcePtr<sf::Font> img = engine.getFontMgr().Acquire(key);
   /* if (!img) // load failed
        PyErr_SetString(PyExc_ValueError, "Failed to load font.");
        throw_error_already_set();
    */
    return img;
}

void PythonEmbedder::exportOiwEngine()
{
    namespace bp = boost::python;
    class_<OiwEngine, boost::noncopyable>("OiwEngine",
            "God-class' of Open Internal War's engine", no_init)
        .def("quit", &OiwEngine::quit)
        .def("addState",&OiwEngine::addState, with_custodian_and_ward<1, 2>()) // keep 'state' alive as long as 'engine' is alive
        .def("setCurrentState",&OiwEngine::setCurrentState)
        .def("getRenderWindow",&OiwEngine::getRenderWindow,
             return_value_policy<reference_existing_object>())
        .def("loadTexture", &engine_loadtexture)
        .def("loadFont", &engine_loadfont)
    ; class_<StateWrap, boost::noncopyable>("State",
            "Abstract class to define game states, such as 'Splash Scren', 'Game Menu' or 'InGame'",
            bp::init<OiwEngine*>())
        .def("init", &State::init, &StateWrap::default_init)
        .def("update", pure_virtual(&State::update))
        .def("render", pure_virtual(&State::render))
        .def("handleEvent", pure_virtual(&State::handleEvent))
    ; class_<EntityWrap, boost::noncopyable>("Entity",
        "An abstract class representing a game entity",
        bp::init<const std::string&, optional<const sf::Vector2f&,
            const float&, const float&> >())
        .def("render", pure_virtual(&Entity::render))
        .def("update", pure_virtual(&Entity::update))
        .def("getSize", pure_virtual(&Entity::getSize))
        .def("transformToLocal", pure_virtual(&Entity::transformToLocal))
        .def("transformToGlobal", pure_virtual(&Entity::transformToGlobal))
        .def("getSubRect", &Entity::getSubRect, &EntityWrap::default_getSubRect)
        .add_property("pos", make_function(&Entity::pos,
            return_value_policy<copy_const_reference>()),
            (void(EntityWrap::*)(object))&EntityWrap::setPos)
        .add_property("angle", &Entity::angle, &Entity::setAngle)
        .add_property("speed", &Entity::speed, &Entity::setSpeed)
        .add_property("id", make_function(&Entity::id,
            return_value_policy<copy_const_reference>()))
        .add_property("collision_table", make_function(&Entity::collisionTable,
            return_value_policy<copy_const_reference>()),
            &Entity::setCollisionTable)
    ; class_<EntityManager, boost::noncopyable>("EntityManager",
        "Tool class to handle all game entities and their interactions")
        .def("addEntity", &EntityManager::addEntity,
            with_custodian_and_ward<1, 2>()) // keep entity as long as manager is alive
        .def("renderAll", &EntityManager::renderAll)
        .def("updateAll", &EntityManager::updateAll)
    ; class_<std::vector<bool> >("bool_vec")
        .def(vector_indexing_suite<std::vector<bool> >())
    ; class_<std::vector< std::vector<bool> > >("bool_vec2")
        .def(vector_indexing_suite<std::vector< std::vector<bool> > >())
    ; class_<CollisionTable>("CollisionTable",
        "Stores Entity's collision data")
        .def(bp::init<const sf::Image&, optional<const sf::Uint8&> >())
        .def_readwrite("pixelsPresent", &CollisionTable::pixelsPresent)
    ; class_<EntityCollisions>("EntityCollisions",
        "Static class providing collision functions between entities", no_init)
        .def("pixelPerfectTest", &EntityCollisions::pixelPerfectTest)
        .def("getAABB", &EntityCollisions::getAABB)
        .staticmethod("pixelPerfectTest")
        .staticmethod("getAABB")
    ;
}

void PythonEmbedder::exportSpaceCarrier()
{
// Note : for now all game code is in Python script (prototyping)
}
