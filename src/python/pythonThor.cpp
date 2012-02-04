#include <iostream>
#include "PythonEmbedder.hpp"

using namespace boost::python;

#include <thor/Time.hpp>
#include <thor/Multimedia.hpp>
#include <thor/Events.hpp>
#include <thor/Resources.hpp>
#include <thor/Geometry.hpp>
#include <thor/Particles.hpp>
#include <thor/Vectors.hpp>
#include <SFML/Graphics.hpp>
#include <tr1/memory>

// A macro to simplify overloaded member function (=method) binding
#define method(CLASS, METHOD, RTYPE, ARGS...) \
    def(BOOST_PP_STRINGIZE(METHOD), \
        (RTYPE(CLASS::*)(ARGS))&CLASS::METHOD)

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
        def("NoDeletePtr", &thor::NoDeletePtr<const Resource>,
            with_custodian_and_ward_postcall<1, 0>()); // keep ResourcePtr as long as original pointer is alive
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

// needed by boost::python
namespace std
{
    namespace tr1
    {
        template<class T>
        T* get_pointer(std::tr1::shared_ptr<T> &p)
        {
            return p.get();
        }
        template<class T>
        const T* get_pointer(const std::tr1::shared_ptr<T> &p)
        {
            return p.get();
        }
    }
} // namespace std::tr1

/*// Emitter::Ptr and Affector::Ptr are std::tr1::shared_ptr
namespace boost
{
    namespace python
    {
        //Make Boost.Python work with std::tr1::shared_ptr<>
        template<typename T_>
        struct pointee<std::tr1::shared_ptr<T_> >
        {
            typedef T_ type;
        };

        //Make Boost.Python work with std::tr1::shared_ptr<const>
        template<typename T_>
        struct pointee<std::tr1::shared_ptr<const T_> >
        {
            typedef T_ type;
        };
    }
}*/

// "Thin wrappers"
void Emitter_SetEmissionZone(thor::Emitter &emitter, thor::Zone &zone)
{
    emitter.SetEmissionZone(thor::Emitter::ZonePtr(&zone));
}
void TargetEmitter_SetTargetZone(thor::TargetEmitter &emitter, thor::Zone &zone)
{
    emitter.SetTargetZone(thor::Emitter::ZonePtr(&zone));
}

sf::Vector2f PolarVector2f_to_Vector2f(const thor::PolarVector2f &vector)
{
    return static_cast<sf::Vector2f>(vector);
}

// TODO (Pierre-Yves#1#): [EXPORT-THOR] Find a way to bind CreateGradient (from a list of colors and transitions...)
// TODO (Pierre-Yves#2#): [EXPORT-THOR] thor::Action : operators && (and) and || (or) don't work
// TODO (Pierre-Yves#5#): [EXPORT-THOR] make thor::Emitter and thor::Affector derivable (--> Wrapper)
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
    class_<thor::ColorGradient>("ColorGradient",
        "Class to implement color gradients", bp::init<const sf::Color&>())
        .def("GetColor", &thor::ColorGradient::GetColor)
    ; def("ToString", (std::string(*)(const sf::Color&))&thor::ToString);

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
    ; } // end of thor::Resources scope
    ResourcePtr_wrapper<sf::Texture>::wrap("TextureResourcePtr");
    ResourcePtr_wrapper<sf::Image>::wrap("ImageResourcePtr");
    ResourcePtr_wrapper<sf::Font>::wrap("FontResourcePtr");

    // Geometry
    class_<thor::Zone, boost::noncopyable>("Zone",
        "Abstract base class for geometric zones", no_init)
        .def("GetRandomPoint", &thor::Zone::GetRandomPoint)
        .def("Clone", &thor::Zone::Clone,
             return_value_policy<manage_new_object>())
        .method(thor::Zone, SetPosition, void, sf::Vector2f)
        .method(thor::Zone, SetPosition, void, float, float)
        .method(thor::Zone, Move, void, sf::Vector2f)
        .method(thor::Zone, Move, void, float, float)
        .def("GetPosition", &thor::Zone::GetPosition)
        .def("SetRotation", &thor::Zone::SetRotation)
        .def("Rotate", &thor::Zone::Rotate)
        .def("GetRotation", &thor::Zone::GetRotation)
    ; class_<thor::Circle, bases<thor::Zone> >("Circle", "Geometric circle class",
        bp::init<sf::Vector2f, float>())
        .def("SetRadius", &thor::Circle::SetRadius)
        .def("GetRadius", &thor::Circle::GetRadius)
    ; class_<thor::Point, bases<thor::Zone> >("Point", "Geometric point class",
        bp::init<sf::Vector2f>())
        .def(bp::init<float, float>())
    ; class_<thor::Rectangle, bases<thor::Zone> >("Rectangle",
        "Geometric rectangle class",
        bp::init< const sf::FloatRect&, optional<float> >())
        .def(bp::init< sf::Vector2f, sf::Vector2f, optional<float> >())
        .def(bp::init< float, float, float, float, optional<float> >())
        .def("SetSize", &thor::Rectangle::SetSize)
        .def("GetSize", &thor::Rectangle::GetSize)
    ;
    // Particles
    {
    using namespace thor;
        // ParticleSystem scope
        {
        scope nested = class_<ParticleSystem, boost::noncopyable>("ParticleSystem",
            "Class for simple particle systems",
                bp::init<ResourcePtr<const sf::Texture>,
                optional<const sf::IntRect&> >())
            .def("Swap", &ParticleSystem::Swap)
            .def("AddAffector", (void(ParticleSystem::*)(Affector::Ptr))
                 &ParticleSystem::AddAffector, with_custodian_and_ward<1, 2>()) // keep affector as long as manager is alive
            .def("RemoveAffector", &ParticleSystem::RemoveAffector)
            .def("ClearAffectors", &ParticleSystem::ClearAffectors)
            .def("ContainsAffector", &ParticleSystem::ContainsAffector)
            .def("AddEmitter", (void(ParticleSystem::*)(Emitter::Ptr))
                 &ParticleSystem::AddEmitter, with_custodian_and_ward<1, 2>()) // keep affector as long as manager is alive
            .def("RemoveEmitter", &ParticleSystem::RemoveEmitter)
            .def("ClearEmitters", &ParticleSystem::ClearEmitters)
            .def("ContainsEmitter", &ParticleSystem::ContainsEmitter)
            .def("Update", &ParticleSystem::Update)
            .def("Draw", &ParticleSystem::Draw)
            .def("ClearParticles", &ParticleSystem::ClearParticles)
            .def("SetGlowing", &ParticleSystem::SetGlowing)
            .def("IsGlowing", &ParticleSystem::IsGlowing)
        ; class_<ParticleSystem::TimeLimit>("TimeLimit",
            "Functor for emitter/affector removal triggered by time limit",
            bp::init<float>())
    ; } // end of ParticleSystem scope
    ResourcePtr_wrapper<const sf::Texture>::wrap("TextureResourcePtrConst");
    class_<Emitter, boost::noncopyable, std::tr1::shared_ptr<Emitter> >("Emitter",
        "Abstract base class for particle emitters", no_init)
        .def("SetEmissionZone", &Emitter_SetEmissionZone,
             with_custodian_and_ward<1, 2>()) // keep zone as long as emitter is alive
        .def("GetEmissionZone", (Zone&(Emitter::*)())&Emitter::GetEmissionZone,
            return_internal_reference<1>())
        .def("GetEmissionZone", (const Zone&(Emitter::*)()const)
             &Emitter::GetEmissionZone, return_internal_reference<1>())
        .def("SetEmissionRate", &Emitter::SetEmissionRate)
        .def("GetEmissionRate", &Emitter::GetEmissionRate)
        .def("SetParticleScale", &Emitter::SetParticleScale)
        .def("GetParticleScale", &Emitter::GetParticleScale)
        .def("SetParticleColor", &Emitter::SetParticleColor)
        .def("GetParticleColor", &Emitter::GetParticleColor,
             return_internal_reference<1>())
        .def("SetParticleLifetime", &Emitter::SetParticleLifetime)
        .def("GetParticledLifetime", &Emitter::GetParticleLifetime)
    ; class_<DirectionalEmitter, bases<Emitter>, std::tr1::shared_ptr<DirectionalEmitter> >(
        "DirectionalEmitter", "Class that emits particles in a given direction",
        bp::init<float, float>())
        .def("Create", &DirectionalEmitter::Create)
        .staticmethod("Create")
        .def("SetParticleVelocity", &DirectionalEmitter::SetParticleVelocity)
        .def("GetParticleVelocity", &DirectionalEmitter::GetParticleVelocity)
        .def("SetEmissionAngle", &DirectionalEmitter::SetEmissionAngle)
        .def("GetEmissionAngle", &DirectionalEmitter::GetEmissionAngle)
    ; class_<TargetEmitter, bases<Emitter>, std::tr1::shared_ptr<TargetEmitter> >(
        "TargetEmitter", "Emits particles towards a specified target zone",
        bp::init<float, float>())
        .def("Create", &TargetEmitter::Create)
        .staticmethod("Create")
        .def("SetTargetZone", &TargetEmitter_SetTargetZone,
             with_custodian_and_ward<1, 2>()) // keep zone as long as emitter is alive
        .def("GetTargetZone", (Zone&(TargetEmitter::*)())&TargetEmitter::GetTargetZone,
            return_internal_reference<1>())
        .def("GetTargetZone", (const Zone&(TargetEmitter::*)()const)
             &TargetEmitter::GetTargetZone, return_internal_reference<1>())
        .def("SetParticleSpeed", &TargetEmitter::SetParticleSpeed)
        .def("GetParticleSpeed", &TargetEmitter::GetParticleSpeed)
    ; implicitly_convertible< std::tr1::shared_ptr<DirectionalEmitter>, std::tr1::shared_ptr<Emitter> >();
    implicitly_convertible< std::tr1::shared_ptr<TargetEmitter>, std::tr1::shared_ptr<Emitter> >();

    class_<Affector, boost::noncopyable, std::tr1::shared_ptr<Affector> >(
        "Affector", "Abstract base class for particle affectors", no_init)
    ; class_<ColorAffector, std::tr1::shared_ptr<ColorAffector> >("ColorAffector",
        "Applies a color gradient to particles", bp::init<const ColorGradient&>())
        .def("Create", &ColorAffector::Create)
        .staticmethod("Create")
        .def("SetGradient", &ColorAffector::SetGradient,
             with_custodian_and_ward<1, 2>()) // keep gradient as long as affector is alive
        .def("GetGradient", &ColorAffector::GetGradient,
             return_internal_reference<1>())
    ; class_<FadeInAffector, std::tr1::shared_ptr<FadeInAffector> >("FadeInAffector",
        "Fades particles in over time", bp::init< optional<float> >())
        .def("Create", &FadeInAffector::Create)
        .staticmethod("Create")
        .def("SetTimeRatio", &FadeInAffector::SetTimeRatio)
        .def("GetTimeRatio", &FadeInAffector::GetTimeRatio)
    ; class_<FadeOutAffector, std::tr1::shared_ptr<FadeOutAffector> >("FadeOutAffector",
        "Fades particles out over time", bp::init< optional<float> >())
        .def("Create", &FadeOutAffector::Create)
        .staticmethod("Create")
        .def("SetTimeRatio", &FadeOutAffector::SetTimeRatio)
        .def("GetTimeRatio", &FadeOutAffector::GetTimeRatio)
    ; class_<ForceAffector, std::tr1::shared_ptr<ForceAffector> >("ForceAffector",
        "Applies a translational acceleration to particles over time",
        bp::init<sf::Vector2f>())
        .def("Create", &ForceAffector::Create)
        .staticmethod("Create")
        .def("SetAcceleration", &ForceAffector::SetAcceleration)
        .def("GetAcceleration", &ForceAffector::GetAcceleration)
    ; class_<ScaleAffector, std::tr1::shared_ptr<ScaleAffector> >("ScaleAffector",
        "Scales particles over time", bp::init<sf::Vector2f>())
        .def("Create", &ScaleAffector::Create)
        .staticmethod("Create")
        .def("SetScaleFactor", &ScaleAffector::SetScaleFactor)
        .def("GetScaleFactor", &ScaleAffector::GetScaleFactor)
    ; class_<TorqueAffector , std::tr1::shared_ptr<TorqueAffector > >("TorqueAffector ",
        "Applies a rotational acceleration to particles over time",
        bp::init<float>())
        .def("Create", &TorqueAffector ::Create)
        .staticmethod("Create")
        .def("SetAngularAcceleration", &TorqueAffector::SetAngularAcceleration)
        .def("GetAngularAcceleration", &TorqueAffector::GetAngularAcceleration)
    ; implicitly_convertible< std::tr1::shared_ptr<ColorAffector>, std::tr1::shared_ptr<Affector> >();
    implicitly_convertible< std::tr1::shared_ptr<FadeInAffector>, std::tr1::shared_ptr<Affector> >();
    implicitly_convertible< std::tr1::shared_ptr<FadeOutAffector>, std::tr1::shared_ptr<Affector> >();
    implicitly_convertible< std::tr1::shared_ptr<ForceAffector>, std::tr1::shared_ptr<Affector> >();
    implicitly_convertible< std::tr1::shared_ptr<ScaleAffector>, std::tr1::shared_ptr<Affector> >();
    implicitly_convertible< std::tr1::shared_ptr<TorqueAffector>, std::tr1::shared_ptr<Affector> >();
    }
    // Vectors
    class_<thor::PolarVector2f>("PolarVector2f",
        "Vector in polar coordinate system")
        .def(bp::init<float, float>())
        .def(bp::init<const sf::Vector2f&>())
        .def_readwrite("r", &thor::PolarVector2f::r)
        .def_readwrite("phi", &thor::PolarVector2f::phi)
        .def("to_vec2f", &PolarVector2f_to_Vector2f)
    ; def("Lenght", (float(*)(const thor::PolarVector2f&))&thor::Length);
    def("PolarAngle", (float(*)(const thor::PolarVector2f&))&thor::PolarAngle);
}
