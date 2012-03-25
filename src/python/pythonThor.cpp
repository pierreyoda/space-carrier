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
#include <Aurora/SmartPtr/CopiedPtr.hpp>
#include <SFML/Graphics.hpp>
#include <tr1/memory>

// "thin wrappers" : auto overloading macro would be too heavy here
void thor_stopwatch_reset(thor::StopWatch &watch) { watch.reset(); }
void thor_timer_reset(thor::Timer &timer, sf::Time timeLimit)
{
    timer.reset(timeLimit);
}

typedef thor::ActionMap<std::string> ActionStrMap;
typedef thor::ActionContext<std::string> ActionContextStr;
typedef thor::EventSystem<ActionContextStr, std::string> EventSystemStr;

struct EventSystemStrWrap : EventSystemStr, wrapper<EventSystemStr>
{
    void TriggerEvent(const ActionContextStr &event)
    {
        this->get_override("triggerEvent")(event);
    }
    thor::Connection Connect(const std::string &trigger, object function)
    {
        return this->get_override("connect")(function);
    }
    void ClearConnections(const std::string &identifier)
    {
        this->get_override("clearConnections")(identifier);
    }
    void ClearAllConnections()
    {
        this->get_override("clearAllConnections")();
    }
};

void thor_ActionStrMap_invokeCallbacks(ActionStrMap &actionMap,
    EventSystemStrWrap &pythonEventSystem)
{
    actionMap.invokeCallbacks(pythonEventSystem);
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

BOOST_PYTHON_FUNCTION_OVERLOADS(imgkey_fromsize, thor::Resources::ImageKey::fromSize, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(imgkey_fromfile, thor::Resources::ImageKey::fromFile, 1, 2)
BOOST_PYTHON_FUNCTION_OVERLOADS(texturekey_fromsize, thor::Resources::TextureKey::fromSize, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(texturekey_fromfile, thor::Resources::TextureKey::fromFile, 1, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(texturekey_fromimg, thor::Resources::TextureKey::fromImage, 1, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(fontkey_fromfile, thor::Resources::FontKey::fromFile, 1, 2)

template <typename Resource>
struct ResourcePtr_wrapper
{
    typedef thor::ResourcePtr<Resource> ResourcePtrT;

    static void wrap(const std::string &python_name)
    {
        class_<ResourcePtrT>(python_name.c_str())
            .def(init<const ResourcePtrT&>())
            .def("swap", &ResourcePtrT::swap)
            .def("reset", (void(ResourcePtrT::*)())&ResourcePtrT::reset)
            .def("valid", &valid)
            .def("get", &get,  return_internal_reference<1, with_custodian_and_ward_postcall<0, 1> >())
        ;
        /*def("NoDeletePtr", &thor::NoDeletePtr<const Resource>,
            with_custodian_and_ward_postcall<1, 0>()); // keep ResourcePtr as long as original pointer is alive*/
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

namespace aur
{
    template<class T>
    T* get_pointer(aur::CopiedPtr<T> &p)
    {
        return p.get();
    }
    template<class T>
    const T* get_pointer(const aur::CopiedPtr<T> &p)
    {
        return p.get();
    }
} // namespace aur

namespace boost
{
    namespace python
    {
        template <class T>
        struct pointee< aur::CopiedPtr<T> >
        {
            typedef T type;
        };
    }
} // namespace boost::python



// "Thin wrappers"
/*void Emitter_setEmissionZone(thor::Emitter &emitter, thor::Zone &zone)
{
    emitter.setEmissionZone(aur::CopiedPtr<thor::Zone>(zone));
}
void TargetEmitter_setTargetZone(thor::TargetEmitter &emitter, thor::Zone &zone)
{
    emitter.setTargetZone(zone);
}*/

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
        .def("getElapsedTime", &thor::StopWatch::getElapsedTime)
        .def("isRunning", &thor::StopWatch::isRunning)
        .def("start", &thor::StopWatch::start)
        .def("stop", &thor::StopWatch::stop)
        .def("reset", &thor::StopWatch::reset)
        .def("reset", &thor_stopwatch_reset)
    ; class_<thor::Timer>("Timer",
        "Clock class that has the semantics of a timer")
        .def(bp::init<sf::Time, optional<bool> >())
        .def("getRemainingTime", &thor::Timer::getRemainingTime)
        .def("isRunning", &thor::Timer::isRunning)
        .def("isExpired", &thor::Timer::isExpired)
        .def("start", &thor::Timer::start)
        .def("stop", &thor::Timer::stop)
        .def("reset", &thor::Timer::reset)
        .def("reset", &thor_timer_reset)
    ;
    class_<thor::ColorGradient>("ColorGradient",
        "Class to implement color gradients", bp::init<const sf::Color&>())
        .def("getColor", &thor::ColorGradient::getColor)
    ; def("toString", (std::string(*)(const sf::Color&))&thor::toString);

    // Events
    class_<ActionStrMap, boost::noncopyable>("ActionStrMap",
        "Class that associates string identifiers with dynamic actions",
        bp::init<sf::Window&>())
        .def("update", &ActionStrMap::update)
        .def("pushEvent", &ActionStrMap::pushEvent)
        .def("clearEvents", &ActionStrMap::clearEvents)
        .def("__getitem__", &thor_ActionStrMap_getitem,
            return_value_policy<copy_non_const_reference>())
        .def("__setitem__", &thor_ActionStrMap_setitem,
            with_custodian_and_ward<1,2>()) // to let container keep value
        .def("removeAction", &ActionStrMap::removeAction)
        .def("clearActions", &ActionStrMap::clearActions)
        .def("isActive", &ActionStrMap::isActive)
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
    ; class_<thor::Joystick>("Joystick",
        "Small wrapper class for joystick number and button number",
        bp::init<unsigned int, unsigned int>())
        .def_readwrite("id", &thor::Joystick::id)
        .def_readwrite("button", &thor::Joystick::button)
    ; class_<thor::Joy>("Joy",
        "Proxy class that allows the Joy(id).button(button) syntax",
        bp::init<unsigned int>())
        .def("button", &thor::Joy::button)
        .def_readwrite("id", &thor::Joy::id)
    ;
    // Resources
    {
    using namespace thor::Resources;
    scope nested = class_<Resources>("Resources", "thor::Resources namespace",
        no_init)
    ; class_<ImageKey>("ImageKey", no_init)
        .def("fromSize", &ImageKey::fromSize, imgkey_fromsize())
        .def("fromFile", &ImageKey::fromFile, imgkey_fromfile())
        .staticmethod("fromSize")
        .staticmethod("fromFile")
        .def("swap", &ImageKey::swap)
        .def(self < other<const ImageKey>())
    ; class_<TextureKey>("TextureKey", no_init)
        .def("fromSize", &TextureKey::fromSize, texturekey_fromsize())
        .def("fromFile", &TextureKey::fromFile, texturekey_fromfile())
        .def("fromImage", &TextureKey::fromImage, texturekey_fromimg())
        .staticmethod("fromSize")
        .staticmethod("fromFile")
        .staticmethod("fromImage")
    ; class_<FontKey>("FontKey", no_init)
        .def("fromFile", &FontKey::fromFile, fontkey_fromfile())
        .staticmethod("fromFile")
        .def("swap", &FontKey::swap)
        .def(self < other<const FontKey>())
    ; } // end of thor::Resources scope
    ResourcePtr_wrapper<sf::Texture>::wrap("TextureResourcePtr");
    ResourcePtr_wrapper<sf::Image>::wrap("ImageResourcePtr");
    ResourcePtr_wrapper<sf::Font>::wrap("FontResourcePtr");

    // Geometry
    class_<thor::Zone, boost::noncopyable, aur::CopiedPtr<thor::Zone> >("Zone",
        "Abstract base class for geometric zones", no_init)
        .method(thor::Zone, setPosition, void, sf::Vector2f)
        .method(thor::Zone, setPosition, void, float, float)
        .method(thor::Zone, move, void, sf::Vector2f)
        .method(thor::Zone, move, void, float, float)
        .def("getPosition", &thor::Zone::getPosition)
        .def("setRotation", &thor::Zone::setRotation)
        .def("rotate", &thor::Zone::rotate)
        .def("getRotation", &thor::Zone::getRotation)
    ; class_<thor::Circle, bases<thor::Zone>, aur::CopiedPtr<thor::Circle> >("Circle", "Geometric circle class",
        bp::init<sf::Vector2f, float>())
        .def("getRandomPoint", &thor::Circle::getRandomPoint)
        .def("clone", pure_virtual(&thor::Circle::clone),
            return_value_policy<manage_new_object>())
        .def("setRadius", &thor::Circle::setRadius)
        .def("getRadius", &thor::Circle::getRadius)
    ; class_<thor::Point, bases<thor::Zone>, aur::CopiedPtr<thor::Point> >("Point", "Geometric point class",
        bp::init<sf::Vector2f>())
        .def("getRandomPoint", &thor::Point::getRandomPoint)
        .def("clone", pure_virtual(&thor::Point::clone),
            return_value_policy<manage_new_object>())
        .def(bp::init<float, float>())
    ; class_<thor::Rectangle, bases<thor::Zone>, aur::CopiedPtr<thor::Rectangle> >("Rectangle",
        "Geometric rectangle class",
        bp::init< const sf::FloatRect&, optional<float> >())
        .def(bp::init< sf::Vector2f, sf::Vector2f, optional<float> >())
        .def(bp::init< float, float, float, float, optional<float> >())
        .def("getRandomPoint", &thor::Rectangle::getRandomPoint)
        .def("clone", pure_virtual(&thor::Rectangle::clone),
            return_value_policy<manage_new_object>())
        .def("setSize", &thor::Rectangle::setSize)
        .def("getSize", &thor::Rectangle::getSize)
    ;
    // Particles
    {
    using namespace thor;
    implicitly_convertible< aur::CopiedPtr<Circle>, aur::CopiedPtr<Zone> >();
    implicitly_convertible< aur::CopiedPtr<Point>, aur::CopiedPtr<Zone> >();
    implicitly_convertible< aur::CopiedPtr<Rectangle>, aur::CopiedPtr<Zone> >();
    class_<ParticleSystem, boost::noncopyable>("ParticleSystem",
        "Class for simple particle systems",
            bp::init<ResourcePtr<const sf::Texture>,
            optional<const sf::IntRect&> >())
        .def("swap", &ParticleSystem::swap)
        .def("addAffector", (void(ParticleSystem::*)(Affector::Ptr))
             &ParticleSystem::addAffector, with_custodian_and_ward<1, 2>()) // keep affector as long as manager is alive
        .def("removeAffector", &ParticleSystem::removeAffector)
        .def("clearAffectors", &ParticleSystem::clearAffectors)
        .def("containsAffector", &ParticleSystem::containsAffector)
        .def("addEmitter", (void(ParticleSystem::*)(Emitter::Ptr))
             &ParticleSystem::addEmitter, with_custodian_and_ward<1, 2>()) // keep affector as long as manager is alive
        .def("removeEmitter", &ParticleSystem::removeEmitter)
        .def("clearEmitters", &ParticleSystem::clearEmitters)
        .def("containsEmitter", &ParticleSystem::containsEmitter)
        .def("update", &ParticleSystem::update)
        .def("draw", &ParticleSystem::draw)
        .def("clearParticles", &ParticleSystem::clearParticles)
        .def("setGlowing", &ParticleSystem::setGlowing)
        .def("isGlowing", &ParticleSystem::isGlowing)
    ; ResourcePtr_wrapper<const sf::Texture>::wrap("TextureResourcePtrConst");
    class_<Emitter, boost::noncopyable, std::tr1::shared_ptr<Emitter> >("Emitter",
        "Abstract base class for particle emitters", no_init)
        .def("setEmissionZone", &Emitter::setEmissionZone,
             with_custodian_and_ward<1, 2>()) // keep zone as long as emitter is alive
        .def("getEmissionZone", (Zone&(Emitter::*)())&Emitter::getEmissionZone,
            return_internal_reference<>())
        .def("getEmissionZone", (const Zone&(Emitter::*)()const)
             &Emitter::getEmissionZone, return_internal_reference<>())
        .def("setEmissionRate", &Emitter::setEmissionRate)
        .def("getEmissionRate", &Emitter::getEmissionRate)
        .def("setParticleScale", &Emitter::setParticleScale)
        .def("getParticleScale", &Emitter::getParticleScale)
        .def("setParticleColor", &Emitter::setParticleColor)
        .def("getParticleColor", &Emitter::getParticleColor,
             return_internal_reference<>())
        .def("setParticleLifetime", &Emitter::setParticleLifetime)
        .def("getParticledLifetime", &Emitter::getParticleLifetime)
    ; class_<DirectionalEmitter, bases<Emitter>, std::tr1::shared_ptr<DirectionalEmitter> >(
        "DirectionalEmitter", "Class that emits particles in a given direction",
        bp::init<float, sf::Time>())
        .def("create", &DirectionalEmitter::create)
        .staticmethod("create")
        .def("setParticleVelocity", &DirectionalEmitter::setParticleVelocity)
        .def("getParticleVelocity", &DirectionalEmitter::getParticleVelocity)
        .def("setEmissionAngle", &DirectionalEmitter::setEmissionAngle)
        .def("getEmissionAngle", &DirectionalEmitter::getEmissionAngle)
    ; class_<TargetEmitter, bases<Emitter>, std::tr1::shared_ptr<TargetEmitter> >(
        "TargetEmitter", "Emits particles towards a specified target zone",
        bp::init<float, sf::Time>())
        .def("create", &TargetEmitter::create)
        .staticmethod("create")
        .def("setTargetZone", &TargetEmitter::setTargetZone,
             with_custodian_and_ward<1, 2>()) // keep zone as long as emitter is alive
        .def("getTargetZone", (Zone&(TargetEmitter::*)())&TargetEmitter::getTargetZone,
            return_internal_reference<>())
        .def("getTargetZone", (const Zone&(TargetEmitter::*)()const)
             &TargetEmitter::getTargetZone, return_internal_reference<>())
        .def("setParticleSpeed", &TargetEmitter::setParticleSpeed)
        .def("getParticleSpeed", &TargetEmitter::getParticleSpeed)
    ; implicitly_convertible< std::tr1::shared_ptr<DirectionalEmitter>, std::tr1::shared_ptr<Emitter> >();
    implicitly_convertible< std::tr1::shared_ptr<TargetEmitter>, std::tr1::shared_ptr<Emitter> >();

    class_<Affector, boost::noncopyable, std::tr1::shared_ptr<Affector> >(
        "Affector", "Abstract base class for particle affectors", no_init)
    ; class_<ColorAffector, std::tr1::shared_ptr<ColorAffector> >("ColorAffector",
        "Applies a color gradient to particles", bp::init<const ColorGradient&>())
        .def("create", &ColorAffector::create)
        .staticmethod("create")
        .def("setGradient", &ColorAffector::setGradient,
             with_custodian_and_ward<1, 2>()) // keep gradient as long as affector is alive
        .def("getGradient", &ColorAffector::getGradient,
             return_internal_reference<>())
    ; class_<FadeInAffector, std::tr1::shared_ptr<FadeInAffector> >("FadeInAffector",
        "Fades particles in over time", bp::init< optional<float> >())
        .def("create", &FadeInAffector::create)
        .staticmethod("create")
        .def("setTimeRatio", &FadeInAffector::setTimeRatio)
        .def("getTimeRatio", &FadeInAffector::getTimeRatio)
    ; class_<FadeOutAffector, std::tr1::shared_ptr<FadeOutAffector> >("FadeOutAffector",
        "Fades particles out over time", bp::init< optional<float> >())
        .def("create", &FadeOutAffector::create)
        .staticmethod("create")
        .def("setTimeRatio", &FadeOutAffector::setTimeRatio)
        .def("getTimeRatio", &FadeOutAffector::getTimeRatio)
    ; class_<ForceAffector, std::tr1::shared_ptr<ForceAffector> >("ForceAffector",
        "Applies a translational acceleration to particles over time",
        bp::init<sf::Vector2f>())
        .def("create", &ForceAffector::create)
        .staticmethod("create")
        .def("setAcceleration", &ForceAffector::setAcceleration)
        .def("getAcceleration", &ForceAffector::getAcceleration)
    ; class_<ScaleAffector, std::tr1::shared_ptr<ScaleAffector> >("ScaleAffector",
        "Scales particles over time", bp::init<sf::Vector2f>())
        .def("create", &ScaleAffector::create)
        .staticmethod("create")
        .def("setScaleFactor", &ScaleAffector::setScaleFactor)
        .def("getScaleFactor", &ScaleAffector::getScaleFactor)
    ; class_<TorqueAffector , std::tr1::shared_ptr<TorqueAffector > >("TorqueAffector",
        "Applies a rotational acceleration to particles over time",
        bp::init<float>())
        .def("create", &TorqueAffector ::create)
        .staticmethod("create")
        .def("setAngularAcceleration", &TorqueAffector::setAngularAcceleration)
        .def("getAngularAcceleration", &TorqueAffector::getAngularAcceleration)
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
    ; def("lenght", (float(*)(const thor::PolarVector2f&))&thor::length);
    def("polarAngle", (float(*)(const thor::PolarVector2f&))&thor::polarAngle);
}
