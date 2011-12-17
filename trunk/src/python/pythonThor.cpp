#include <iostream>
#include "PythonEmbedder.hpp"

using namespace boost::python;

#include <thor/Time.hpp>
#include <thor/Multimedia.hpp>
#include <thor/Events.hpp>
#include <thor/Resources.hpp>
#include <SFML/Graphics.hpp>

// "thin wrappers" : auto overloading macro would be too heavy here
void thor_stopwatch_reset(thor::StopWatch &watch) { watch.Reset(); }
void thor_timer_reset(thor::Timer &timer, sf::Uint32 timeLimit)
{
    timer.Reset(timeLimit);
}

struct Shapes { }; // just for namespace exporting

BOOST_PYTHON_FUNCTION_OVERLOADS(rounded_overloads, thor::Shapes::RoundedRect, 6, 8)
BOOST_PYTHON_FUNCTION_OVERLOADS(rounded_overloads2, thor::Shapes::RoundedRect, 4, 6)
BOOST_PYTHON_FUNCTION_OVERLOADS(polygon_overloads, thor::Shapes::Polygon, 4, 6)

typedef thor::ActionMap<std::string> ActionStrMap;
typedef thor::ActionContext<std::string> ActionContextStr;
typedef thor::EventSystem<ActionContextStr, std::string> EventSystemStr;

struct EventSystemStrWrap : EventSystemStr, wrapper<EventSystemStr>
{
    void TriggerEvent(const ActionContextStr &event)
    {
        this->get_override("TriggerEvent")(event);
    }
    thor::Connection Connect(const std::string &trigger, object function)
    {
        return this->get_override("Connect")(function);
    }
    void ClearConnections(const std::string &identifier)
    {
        this->get_override("ClearConnections")(identifier);
    }
    void ClearAllConnections()
    {
        this->get_override("ClearAllConnections")();
    }
};

void thor_ActionStrMap_InvokeCallbacks(ActionStrMap &actionMap,
    EventSystemStrWrap &pythonEventSystem)
{
    actionMap.InvokeCallbacks(pythonEventSystem);
}
thor::Action &thor_ActionStrMap_getitem(ActionStrMap &actionMap,
    const std::string &id)
{
    return actionMap[id];
}
void thor_ActionStrMap_setitem(ActionStrMap &actionMap,
    const std::string &id, thor::Action &action)
{
    actionMap[id] = action;
}

thor::Action operator&(const thor::Action &lhs, const thor::Action &rhs)
{
    return lhs && rhs;
}
thor::Action operator|(const thor::Action &lhs, const thor::Action &rhs)
{
    return lhs || rhs;
}

struct Resources { }; // just for namespace exporting

BOOST_PYTHON_FUNCTION_OVERLOADS(imgkey_fromsize, thor::Resources::ImageKey::FromSize, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(imgkey_fromfile, thor::Resources::ImageKey::FromFile, 1, 2)
BOOST_PYTHON_FUNCTION_OVERLOADS(texturekey_fromsize, thor::Resources::TextureKey::FromSize, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(texturekey_fromfile, thor::Resources::TextureKey::FromFile, 1, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(texturekey_fromimg, thor::Resources::TextureKey::FromImage, 1, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(fontkey_fromfile, thor::Resources::FontKey::FromFile, 1, 2)

template <typename Resource>
struct ResourcePtr_wrapper
{
    typedef thor::ResourcePtr<Resource> ResourcePtrT;

    static void wrap(const std::string &python_name)
    {
        class_<ResourcePtrT>(python_name.c_str())
            .def(init<const ResourcePtrT&>())
            .def("Swap", &ResourcePtrT::Swap)
            .def("Reset", (void(ResourcePtrT::*)())&ResourcePtrT::Reset)
            .def("valid", &valid)
            .def("get", &get,  return_internal_reference<1, with_custodian_and_ward_postcall<0, 1> >())
        ;
    }

    static Resource *get(const ResourcePtrT &resourcePtr)
    {
        return &*resourcePtr;
    }

    static bool valid(const ResourcePtrT &resourcePtr)
    {
        return (resourcePtr);
    }
};

// TODO (Pierre-Yves#2#): [EXPORT-THOR] thor::Action : operators && (and) and || (or) don't work
void PythonEmbedder::exportThor()
{
    namespace bp = boost::python;
    // Time
    class_<thor::StopWatch>("StopWatch",
        "Pausable clock class that measures elapsed time", no_init)
        .def(bp::init<optional<bool> >())
        .def("GetElapsedTime", &thor::StopWatch::GetElapsedTime)
        .def("IsRunning", &thor::StopWatch::IsRunning)
        .def("Start", &thor::StopWatch::Start)
        .def("Stop", &thor::StopWatch::Stop)
        .def("Reset", &thor::StopWatch::Reset)
        .def("Reset", &thor_stopwatch_reset)
    ; class_<thor::Timer>("Timer",
        "Clock class that has the semantics of a timer")
        .def(bp::init<sf::Uint32, optional<bool> >())
        .def("GetRemainingTime", &thor::Timer::GetRemainingTime)
        .def("IsRunning", &thor::Timer::IsRunning)
        .def("IsExpired", &thor::Timer::IsExpired)
        .def("Start", &thor::Timer::Start)
        .def("Stop", &thor::Timer::Stop)
        .def("Reset", &thor::Timer::Reset)
        .def("Reset", &thor_timer_reset)
    ;
    // Multimedia
    {
    sf::Shape (*rounded1)(float, float, float, float, float, const sf::Color&,
        float, const sf::Color&) = &thor::Shapes::RoundedRect;
    sf::Shape (*rounded2)(sf::Vector2f, sf::Vector2f, float, const sf::Color&,
        float, const sf::Color&) = &thor::Shapes::RoundedRect;
    scope nested = class_<Shapes>("Shapes", "Namespace for predefined shapes",
        no_init);
    def("RoundedRect", rounded1, rounded_overloads());
    def("RoundedRect", rounded2, rounded_overloads2());
    def("Polygon", &thor::Shapes::Polygon, polygon_overloads());
    } ; // end of sf::Shapes scope
    /*class_<thor::ConcaveShape, bases<sf::Drawable> >("ConcaveShape",
        "Concave shape class") // MUST BE IN THE SF MODULE
    ;*/
    def("ToString", (std::string(*)(const sf::Color&))&thor::ToString);
    def("ToString", (std::string(*)(const sf::Vector2i&))&thor::ToString);
    def("ToString", (std::string(*)(const sf::Vector2f&))&thor::ToString);
    def("ToString", (std::string(*)(const sf::Vector2u&))&thor::ToString);
    def("ToString", (std::string(*)(const sf::IntRect&))&thor::ToString);
    def("ToString", (std::string(*)(const sf::FloatRect&))&thor::ToString);
    // Events
    class_<ActionStrMap, boost::noncopyable>("ActionStrMap",
        "Class that associates string identifiers with dynamic actions",
        bp::init<sf::Window&>())
        .def("Update", &ActionStrMap::Update)
        .def("PushEvent", &ActionStrMap::PushEvent)
        .def("ClearEvents", &ActionStrMap::ClearEvents)
        .def("__getitem__", &thor_ActionStrMap_getitem,
            return_value_policy<copy_non_const_reference>())
        .def("__setitem__", &thor_ActionStrMap_setitem,
            with_custodian_and_ward<1,2>()) // to let container keep value
        .def("RemoveAction", &ActionStrMap::RemoveAction)
        .def("ClearActions", &ActionStrMap::ClearActions)
        .def("IsActive", &ActionStrMap::IsActive)
        /*.def("InvokeCallbacks", &ActionStrMap::InvokeCallbacks)
        .def("InvokeCallbacks", &thor_ActionStrMap_InvokeCallbacks)*/
    ; { scope nested = class_<thor::Action>("Action",
        "Class for dynamic actions that are connected with SFML events", no_init)
        .def(bp::init<sf::Keyboard::Key, optional<thor::Action::ActionType> >())
        .def(bp::init<sf::Mouse::Button, optional<thor::Action::ActionType> >())
        .def(bp::init<thor::Joystick, optional<thor::Action::ActionType> >())
        .def(bp::init<sf::Event::EventType>())
        .def(bp::init<thor::Action>()) // copy
        .def(self & thor::Action()) // or
        .def(self | thor::Action()) // and
        ; enum_<thor::Action::ActionType>("ActionType")
            .value("Realtime", thor::Action::Realtime)
            .value("PressOnce", thor::Action::PressOnce)
            .value("ReleaseOnce", thor::Action::ReleaseOnce)
            .value("Once", thor::Action::Once)
            .export_values()
    ; } // end of thor::Action scope
    /*class_<ActionContextStr>("ActionContextStr",
        "Structure containing information about the context in which an action has occurred",
        no_init)
        .add_property("Window",
            make_getter(&ActionContextStr::Window,
                return_value_policy<reference_existing_object>()))
        .add_property("Event",
            make_getter(&ActionContextStr::Event,
                return_value_policy<reference_existing_object>()))
        .def_readonly("ActionId", &ActionContextStr::ActionId)
    ; class_<EventSystemStr, boost::noncopyable>("EventSystemStr",
        "Class for object-oriented handling of user-defined events", no_init)
        .def("TriggerEvent", &EventSystemStr::TriggerEvent)
        //.def("Connect", &EventSystemStr::Connect)
        .def("TriggerEvent", &EventSystemStr::ClearConnections)
        .def("ClearAllConnections", &EventSystemStr::ClearAllConnections)
    ; class_<EventSystemStrWrap, boost::noncopyable>(
        "CustomEventSystemStr", "Makes thor callback system possible in Python")
        .def("TriggerEvent", pure_virtual(&EventSystemStrWrap::TriggerEvent))
        .def("Connect", pure_virtual(&EventSystemStrWrap::Connect))
        .def("ClearConnections", pure_virtual(&EventSystemStrWrap::ClearConnections))
        .def("ClearAllConnections", pure_virtual(&EventSystemStrWrap::ClearAllConnections))
    ; class_<thor::Connection>("Connection",
        "Class that manages the connection between an event and a listener")
        .def("IsConnected", &thor::Connection::IsConnected)
        .def("Invalidate", &thor::Connection::Invalidate)
        .def("Disconnect", &thor::Connection::Disconnect)*/
    ; class_<thor::Joystick>("Joystick",
        "Small wrapper class for joystick number and button number",
        bp::init<unsigned int, unsigned int>())
        .def_readwrite("Id", &thor::Joystick::Id)
        .def_readwrite("Button", &thor::Joystick::Button)
    ; class_<thor::Joy>("Joy",
        "Proxy class that allows the Joy(id).Button(button) syntax",
        bp::init<unsigned int>())
        .def("Button", &thor::Joy::Button)
        .def_readwrite("id", &thor::Joy::id)
    ;
    // Resources
    {
    using namespace thor::Resources;
    scope nested = class_<Resources>("Resources", "thor::Resources namespace",
        no_init)
    ; class_<ImageKey>("ImageKey", no_init)
        .def("FromSize", &ImageKey::FromSize, imgkey_fromsize())
        .def("FromFile", &ImageKey::FromFile, imgkey_fromfile())
        .staticmethod("FromSize")
        .staticmethod("FromFile")
        .def("Swap", &ImageKey::Swap)
        .def(self < other<const ImageKey>())
    ; class_<TextureKey>("TextureKey", no_init)
        .def("FromSize", &TextureKey::FromSize, texturekey_fromsize())
        .def("FromFile", &TextureKey::FromFile, texturekey_fromfile())
        .def("FromImage", &TextureKey::FromImage, texturekey_fromimg())
        .staticmethod("FromSize")
        .staticmethod("FromFile")
        .staticmethod("FromImage")
    ; class_<FontKey>("FontKey", no_init)
        .def("FromFile", &FontKey::FromFile, fontkey_fromfile())
        .staticmethod("FromFile")
        .def("Swap", &FontKey::Swap)
        .def(self < other<const FontKey>())
    ;} // end of thor::Resources scope
    ResourcePtr_wrapper<sf::Texture>::wrap("TextureResourcePtr");
    ResourcePtr_wrapper<sf::Image>::wrap("ImageResourcePtr");
    ResourcePtr_wrapper<sf::Font>::wrap("FontResourcePtr");
}
