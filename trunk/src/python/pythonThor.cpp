#include <iostream>
#include "PythonEmbedder.hpp"
#include <boost/python/call.hpp>

using namespace boost::python;

#include <thor/Time.hpp>
#include <thor/Math/Distributions.hpp>
#include <thor/Multimedia.hpp>
#include <thor/Events.hpp>
#include <thor/Resources.hpp>
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
        def("noDeletePtr", &thor::noDeletePtr<const Resource>);
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

sf::Vector2f PolarVector2f_to_Vector2f(const thor::PolarVector2f &vector)
{
    return static_cast<sf::Vector2f>(vector);
}

template <typename T>
struct PythonCallback
{
    PythonCallback(PyObject *callable_) : callable(handle<>(borrowed(callable_)))
    {

    }

    T operator() ()
    {
        return call<T>(callable.ptr());
    }

    object callable;
};

void UniversalEmitter_setLifetime(thor::UniversalEmitter &emitter, sf::Time particleLifetime)
{
    emitter.setLifetime(particleLifetime);
}
void UniversalEmitter_setLifetimeCallback(thor::UniversalEmitter &emitter, PyObject *callable)
{
    emitter.setLifetime(PythonCallback<sf::Time>(callable));
}

void UniversalEmitter_setPosition(thor::UniversalEmitter &emitter, sf::Vector2f particlePosition)
{
    emitter.setPosition(particlePosition);
}
void UniversalEmitter_setPositionCallback(thor::UniversalEmitter &emitter, PyObject *callable)
{
    emitter.setPosition(PythonCallback<sf::Vector2f>(callable));
}

void UniversalEmitter_setVelocity(thor::UniversalEmitter &emitter, sf::Vector2f particleVelocity)
{
    emitter.setVelocity(particleVelocity);
}
void UniversalEmitter_setVelocityCallback(thor::UniversalEmitter &emitter, PyObject *callable)
{
    emitter.setVelocity(PythonCallback<sf::Vector2f>(callable));
}

void UniversalEmitter_setRotation(thor::UniversalEmitter &emitter, float particleRotation)
{
    emitter.setRotation(particleRotation);
}
void UniversalEmitter_setRotationCallback(thor::UniversalEmitter &emitter, PyObject *callable)
{
    emitter.setRotation(PythonCallback<float>(callable));
}

void UniversalEmitter_setRotationSpeed(thor::UniversalEmitter &emitter, float particleRotationSpeed)
{
    emitter.setRotationSpeed(particleRotationSpeed);
}
void UniversalEmitter_setRotationSpeedCallback(thor::UniversalEmitter &emitter, PyObject *callable)
{
    emitter.setRotationSpeed(PythonCallback<float>(callable));
}

void UniversalEmitter_setScale(thor::UniversalEmitter &emitter, sf::Vector2f particleScale)
{
    emitter.setScale(particleScale);
}
void UniversalEmitter_setScaleCallback(thor::UniversalEmitter &emitter, PyObject *callable)
{
    emitter.setScale(PythonCallback<sf::Vector2f>(callable));
}

void UniversalEmitter_setColor(thor::UniversalEmitter &emitter, sf::Color particeColor)
{
    emitter.setColor(particeColor);
}
void UniversalEmitter_setColorCallback(thor::UniversalEmitter &emitter, PyObject *callable)
{
    emitter.setColor(PythonCallback<sf::Color>(callable));
}

thor::ColorGradient createGradientFromList(const sf::Color &first, const list &l)
{
    thor::detail::ColorGradientConvertible creator(first);
    for (unsigned int i = 0; i < static_cast<unsigned int>(len(l) / 2 + 1); i += 2)
    {
        creator(extract<float>(l[i])) (extract<sf::Color>(l[i+1]));
    }
    return thor::ColorGradient(creator);
}

struct DistributionsNamespace { };


/// Wrapper for sf:Distribution<T>
template <typename T>
struct DistributionWrapper
{
    static void wrap(const std::string &python_name)
    {
        class_< aur::Distribution<T> >(python_name.c_str(),
            "Class holding a rule to create values with predefined properties",
            no_init)
            .def("get", &distribution_get)
        ;
    }

    static inline T distribution_get(const aur::Distribution<T> &distribution)
    {
        return distribution();
    }
};

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

    // Multimedia
    class_<thor::ColorGradient>("ColorGradient",
        "Class to implement color gradients", bp::init<const sf::Color&>())
        .def("getColor", &thor::ColorGradient::getColor)
    ; def("createGradient", &createGradientFromList);
    def("toString", (std::string(*)(const sf::Color&))&thor::toString);

    // Particles
    {
    using namespace thor;
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
    ; class_<UniversalEmitter, bases<Emitter>,
        UniversalEmitter::Ptr>("UniversalEmitter",
        "Class that emits particles with customizable initial conditions")
        .def("create", &UniversalEmitter::create)
        .def("setEmissionRate", &UniversalEmitter::setEmissionRate)
        .def("setLifetime", &UniversalEmitter_setLifetime)
        .def("setPosition", &UniversalEmitter_setPosition)
        .def("setVelocity", &UniversalEmitter_setVelocity)
        .def("setRotation", &UniversalEmitter_setRotation)
        .def("setRotationSpeed", &UniversalEmitter_setRotationSpeed)
        .def("setScale", &UniversalEmitter_setScale)
        .def("setColor", &UniversalEmitter_setColor)
        .def("setLifetimeCallback", &UniversalEmitter_setLifetimeCallback)
        .def("setPositionCallback", &UniversalEmitter_setPositionCallback)
        .def("setVelocityCallback", &UniversalEmitter_setVelocityCallback)
        .def("setRotationCallback", &UniversalEmitter_setRotationCallback)
        .def("setRotationSpeedCallback", &UniversalEmitter_setRotationSpeedCallback)
        .def("setScaleCallback", &UniversalEmitter_setScaleCallback)
        .def("setColorCallback", &UniversalEmitter_setColorCallback)
    ; implicitly_convertible< std::tr1::shared_ptr<UniversalEmitter>, std::tr1::shared_ptr<Emitter> >();

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

    // Math
    { // thor::Distributions
        scope nested = class_<DistributionsNamespace>("Distributions",
            "thor::Distributions namespace", no_init);
        def("uniform", &thor::Distributions::uniform);
        def("rect", &thor::Distributions::rect);
        def("circle", &thor::Distributions::circle);
        def("deflect", &thor::Distributions::deflect);
    } // end of thor::Distributions scope

    // AURORA
    DistributionWrapper<float>::wrap("DistributionFloat");
    DistributionWrapper<sf::Vector2f>::wrap("DistributionVector2f");
}
