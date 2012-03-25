#include "PythonEmbedder.hpp"
#include "../engine_2d/Entity.hpp"
#include <SFML/Graphics.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <thor/Multimedia/ToString.hpp>

using namespace boost::python;

/// Wrapper for sf::Vector2<T>
template <typename T>
struct Vector2_wrapper
{
    static void wrap(const std::string &python_name)
    {
        class_< sf::Vector2<T> >(python_name.c_str(),
            "Utility template class for manipulating 2-dimensional vectors")
            .def(init<T, T>())
            .def(init<const sf::Vector2<T>&>())
            .def_readwrite("x", &sf::Vector2<T>::x)
            .def_readwrite("y", &sf::Vector2<T>::y)
            .def(self != other<const sf::Vector2<T> >())
            .def(self * other<T>())
            .def(self *= other<T>())
            .def(self + other<const sf::Vector2<T> >())
            .def(self += other<const sf::Vector2<T> >())
            .def(self - other<const sf::Vector2<T> >())
            .def(self -= other<const sf::Vector2<T> >())
            .def(self / other<T>())
            .def(self /= other<T>())
            .def("__repr__", (std::string(*)(const sf::Vector2<T>&))&thor::toString)
        ;
    }
};

/// Wrapper for sf:Rect<T>
template <typename T>
struct Rect_wrapper
{
    static void wrap(const std::string &python_name)
    {
        //std::string (*::repr)(const sf::Rect<T>&) = &thor::ToString< sf:Rect<T> >;
        class_< sf::Rect<T> >(python_name.c_str(),
            "Utility class for manipulating 2D axis aligned rectangles")
            .def(init<T, T, T, T>())
            .def(init<const sf::Vector2<T>&, const sf::Vector2<T>&>())
            .def(init<const sf::Rect<T>&>())
            //.def("Contains", &sf::Rect<T>::Contains)
            .method_const(sf::Rect<T>, contains, bool, T, T)
            .method_const(sf::Rect<T>, contains, bool, const sf::Vector2<T>&)
            .method_const(sf::Rect<T>, intersects, bool, const sf::Rect<T>&)
            .method_const(sf::Rect<T>, intersects, bool, const sf::Rect<T>&, sf::Rect<T>&)
            .def_readwrite("left", &sf::Rect<T>::left)
            .def_readwrite("top", &sf::Rect<T>::top)
            .def_readwrite("width", &sf::Rect<T>::width)
            .def_readwrite("height", &sf::Rect<T>::height)
            .def("__repr__", (std::string(*)(const sf::Rect<T>&))&thor::toString)
        ;
    }
};

/// "Thin wrapper" for sf::RenderTarget::Draw
inline void RenderTarget_draw(sf::RenderTarget &target,
    const sf::Drawable &drawable)
{
    target.draw(drawable);
}

/// Makes target.Draw(myEntity...) possible
void sfTarget_draw_Entity(sf::RenderTarget &target, const Entity &entity)
{
    target.draw(entity);
}
void sfTarget_draw_Entity2(sf::RenderTarget &target, const Entity &entity,
    const sf::RenderStates &states)
{
    target.draw(entity, states);
}

/// Return a sf::Transform's 4x4 matrix as a std::vector<float>
std::vector<float> sfTransform_getMatrix(const sf::Transform &transform)
{
    const float *matrix = transform.getMatrix();
    std::vector<float> matrix2;
    matrix2.assign(matrix, matrix + 16);
    return matrix2;
}

inline unsigned int string_invalidpos() // avoid "TypeError: No to_python (by-value) converter found for C++ type: unsigned int"
{
    return sf::String::InvalidPos;
}

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Create_overloads, create, 2, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(AddPoint_overloads,  addPoint, 2, 4) // float version
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(AddPoint_overloads2, addPoint, 1, 3) // vector2f version
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Image_copy, copy, 3, 5)

// TODO (Pierre-Yves#5#): [EXPORT-SF] Cannot access to predefined sf::Color ("TypeError: 'Color' object is not callable")
void PythonEmbedder::exportSf()
{
    namespace bp = boost::python; // needed for init<...> to not collide with PythonEmbedder::init

    Vector2_wrapper<int>::wrap("Vector2i");
    Vector2_wrapper<float>::wrap("Vector2f");
    Vector2_wrapper<unsigned int>::wrap("Vector2u");
    Rect_wrapper<int>::wrap("IntRect");
    Rect_wrapper<float>::wrap("FloatRect");
    def("seconds", &sf::seconds);
    def("milliseconds", &sf::milliseconds);
    def("microseconds", &sf::microseconds);
    class_<sf::Time>("Time", "Represents a time value")
        .def("asSeconds", &sf::Time::asSeconds)
        .def("asMilliseconds", &sf::Time::asMilliseconds)
        .def("asMicroseconds", &sf::Time::asMicroseconds)
        .add_static_property("Zero", &sf::Time::Zero)
        .def(self == other<sf::Time>())
        .def(self != other<sf::Time>())
        .def(self < other<sf::Time>())
        .def(self > other<sf::Time>())
        .def(self <= other<sf::Time>())
        .def(self >= other<sf::Time>())
        .def(-self)
        .def(self + other<sf::Time>())
        .def(self += other<sf::Time>())
        .def(self - other<sf::Time>())
        .def(self -= other<sf::Time>())
        .def(self * float())
        .def(self * sf::Int64())
        .def(float() * self)
        .def(sf::Int64()* self)
        .def(self *= float())
        .def(self *= sf::Int64())
        .def(self / float())
        .def(self / sf::Int64())
        .def(self /= float())
        .def(self /= sf::Int64())
    ; class_<sf::Color>("Color", "Utility class for manpulating RGBA colors")
        .def(bp::init<sf::Uint8, sf::Uint8, sf::Uint8, optional<sf::Uint8> >())
        .def_readwrite("r", &sf::Color::r)
        .def_readwrite("g", &sf::Color::g)
        .def_readwrite("b", &sf::Color::b)
        .def_readwrite("a", &sf::Color::a)
        .add_static_property("Black", &sf::Color::Black)
        .add_static_property("White", &sf::Color::White)
        .add_static_property("Red", &sf::Color::Red)
        .add_static_property("Green", &sf::Color::Green)
        .add_static_property("Blue", &sf::Color::Blue)
        .add_static_property("Yellow", &sf::Color::Yellow)
        .add_static_property("Magenta", &sf::Color::Magenta)
        .add_static_property("Cyan", &sf::Color::Cyan)
        .def(self == other<const sf::Color>())
        .def(self + other<const sf::Color>())
        .def(self * other<const sf::Color>())
        .def(self *= other<const sf::Color>())
        .def(self += other<const sf::Color>())
        .def("__repr__", (std::string(*)(const sf::Color&))&thor::toString)
    ; class_<sf::String>("String",
        "Utility string class that automatically handles conversions between types and encodings")
        .def(bp::init<const std::wstring&>())
        .def(bp::init<const sf::String&>())
        .method(sf::String, toAnsiString, std::string, void)
        .def("toWideString", &sf::String::toWideString)
        .def(self += other<const sf::String>())
        .def("clear", &sf::String::clear)
        .def("__len__", &sf::String::getSize)
        .def("isEmpty", &sf::String::isEmpty)
        .method(sf::String, erase, void, std::size_t)
        .method(sf::String, erase, void, std::size_t, std::size_t)
        .def("insert", &sf::String::insert)
        .method(sf::String, find, std::size_t, const sf::String&)
        .method(sf::String, find, std::size_t, const sf::String&, std::size_t)
        .add_property("data",
            range((sf::String::Iterator(sf::String::*)())&sf::String::begin,
                (sf::String::Iterator(sf::String::*)())&sf::String::end))
        .add_property("data_const",
            range((sf::String::ConstIterator(sf::String::*)()const)&sf::String::begin,
                (sf::String::ConstIterator(sf::String::*)()const)&sf::String::end))
        .add_static_property("InvalidPos", &string_invalidpos)
    ; implicitly_convertible<std::wstring, sf::String>();
    {
    sf::Transform& (sf::Transform::*translate1)(float, float) = &sf::Transform::translate;
    sf::Transform& (sf::Transform::*translate2)(const sf::Vector2f&) = &sf::Transform::translate;
    sf::Transform& (sf::Transform::*rotate1)(float) = &sf::Transform::rotate;
    sf::Transform& (sf::Transform::*rotate2)(float, float, float) = &sf::Transform::rotate;
    sf::Transform& (sf::Transform::*rotate3)(float, const sf::Vector2f&) = &sf::Transform::rotate;
    sf::Transform& (sf::Transform::*scale1)(float, float) = &sf::Transform::scale;
    sf::Transform& (sf::Transform::*scale2)(float, float, float, float) = &sf::Transform::scale;
    sf::Transform& (sf::Transform::*scale3)(const sf::Vector2f&) = &sf::Transform::scale;
    sf::Transform& (sf::Transform::*scale4)(const sf::Vector2f&, const sf::Vector2f&) = &sf::Transform::scale;
    class_<std::vector<float> >("float_vec")
        .def(vector_indexing_suite<std::vector<float> >())
    ; class_<sf::Transform>("Transform",
            "Define a 3x3 transform matrix")
            .def(bp::init<float, float, float, float, float, float, float, float, float>())
            .def("getMatrix", &sfTransform_getMatrix)
            //.def("getInverse", &sf::Transform::getInverse)
            .method_const(sf::Transform, transformPoint, sf::Vector2f, float,
                float)
            .method_const(sf::Transform, transformPoint, sf::Vector2f,
                const sf::Vector2f&)
            .def("transformRect", &sf::Transform::transformRect)
            .def("combine", &sf::Transform::combine, return_internal_reference<>())
            .def("translate", translate1, return_internal_reference<>())
            .def("translate", translate2, return_internal_reference<>())
            .def("rotate", rotate1, return_internal_reference<>())
            .def("rotate", rotate2, return_internal_reference<>())
            .def("rotate", rotate3, return_internal_reference<>())
            .def("scale", scale1, return_internal_reference<>())
            .def("scale", scale2, return_internal_reference<>())
            .def("scale", scale3, return_internal_reference<>())
            .def("scale", scale4, return_internal_reference<>())
            .def(self * sf::Transform())
            .def(self *= sf::Transform())
            .def(self * sf::Vector2f())
    ; }
    class_<sf::Drawable, boost::noncopyable>("Drawable",
        "Abstract base class for objects that can be drawn to a render target",
        no_init)
    ; class_<sf::Transformable>("Transformable",
        "Decomposed transform defined by a position, a rotation and a scale")
        .method(sf::Transformable, setPosition, void, float, float)
        .method(sf::Transformable, setPosition, void, const sf::Vector2f&)
        .def("setRotation", &sf::Transformable::setRotation)
        .method(sf::Transformable, setScale, void, float, float)
        .method(sf::Transformable, setScale, void, const sf::Vector2f&)
        .method(sf::Transformable, setOrigin, void, const sf::Vector2f&)
        .method(sf::Transformable, setOrigin, void, float, float)
        .def("getPosition", &sf::Transformable::getPosition,
            return_internal_reference<>())
        .def("getRotation", &sf::Transformable::getRotation)
        .def("getScale", &sf::Transformable::getScale,
            return_internal_reference<>())
        .def("getOrigin", &sf::Transformable::getOrigin,
            return_internal_reference<>())
        .method(sf::Transformable, move, void, float, float)
        .method(sf::Transformable, move, void, const sf::Vector2f&)
        .def("rotate", &sf::Transformable::rotate)
        .method(sf::Transformable, scale, void, float, float)
        .method(sf::Transformable, scale, void, const sf::Vector2f&)
        .def("getTransform", &sf::Transformable::getTransform,
            return_internal_reference<>())
        .def("getInverseTransform", &sf::Transformable::getInverseTransform,
            return_internal_reference<>())
    ; class_<sf::Image>("Image",
        "Class for loading, manipulating and saving images")
        .method(sf::Image, create, void, unsigned int, unsigned int,
            const sf::Color&)
        .def("loadFromFile", &sf::Image::loadFromFile)
        .def("saveToFile", &sf::Image::saveToFile)
        .def("getWidth", &sf::Image::getWidth)
        .def("getHeight", &sf::Image::getHeight)
        .method(sf::Image, createMaskFromColor, void, const sf::Color&)
        .method(sf::Image, createMaskFromColor, void, const sf::Color&,
            sf::Uint8)
        .def("copy", &sf::Image::copy, Image_copy())
        .def("setPixel", &sf::Image::setPixel)
        .def("getPixel", &sf::Image::getPixel)
        //.def("getPixelsPtr", &sf::Texture::getPixelsPtr)
        .def("flipHorizontally", &sf::Image::flipHorizontally)
        .def("flipVertically", &sf::Image::flipVertically)
    ; class_<sf::Texture>("Texture",
        "Image living on the graphics card that can be used for drawing")
        .def(bp::init<const sf::Texture&>())
        .def("create", &sf::Texture::create)
        .method(sf::Texture, loadFromFile, bool, const std::string&)
        .method(sf::Texture, loadFromFile, bool, const std::string&,
            const sf::IntRect&)
        .method(sf::Texture, loadFromImage, bool, const sf::Image&)
        .method(sf::Texture, loadFromImage, bool, const sf::Image&,
            const sf::IntRect&)
        .def("getWidth", &sf::Texture::getWidth)
        .def("getHeight", &sf::Texture::getHeight)
        .def("copyToImage", &sf::Texture::copyToImage)
        .method(sf::Texture, update, void, const sf::Image&)
        .method(sf::Texture, update, void, const sf::Image&, unsigned int,
            unsigned int)
        .method(sf::Texture, update, void, const sf::Window&)
        .method(sf::Texture, update, void, const sf::Window&, unsigned int,
            unsigned int)
        //.def("Bind", &sf::Texture::Bind)
        .def("setSmooth", &sf::Texture::setSmooth)
        .def("isSmooth", &sf::Texture::isSmooth)
        .def("setRepeated", &sf::Texture::setRepeated)
        .def("isRepeated", &sf::Texture::isRepeated)
        .def("getMaximumSize", &sf::Texture::getMaximumSize)
        .staticmethod("getMaximumSize")
    ; enum_<sf::BlendMode>("BlendMode",
        "Available blending modes for drawing")
        .value("Alpha", sf::BlendAlpha)
        .value("Add", sf::BlendAdd)
        .value("Multiply", sf::BlendMultiply)
        .value("None", sf::BlendNone)
    ; class_<sf::Sprite, bases<sf::Drawable, sf::Transformable> >("Sprite",
        "Drawable representation of an image, with its own transformations, color, etc")
        .def(bp::init<sf::Texture&>())
        .def(bp::init<sf::Texture&, const sf::IntRect&>())
        .method(sf::Sprite, setTexture, void, const sf::Texture&)
        .method(sf::Sprite, setTexture, void, const sf::Texture&, bool)
        .def("setTextureRect", &sf::Sprite::setTextureRect)
        .def("setColor", &sf::Sprite::setColor)
        .def("getTexture", &sf::Sprite::getTexture,
            return_internal_reference<>())
        .def("getTextureRect", &sf::Sprite::getTextureRect,
            return_internal_reference<>())
        .def("getColor", &sf::Sprite::getColor,
            return_internal_reference<>())
        .def("getLocalBounds", &sf::Sprite::getLocalBounds)
        .def("getGlobalBounds", &sf::Sprite::getGlobalBounds)
    ; class_<sf::Shape, bases<sf::Drawable, sf::Transformable>,
            boost::noncopyable>("Shape",
            "Base class for textured shapes with outline", no_init)
        .method(sf::Shape, setTexture, void, const sf::Texture&)
        .method(sf::Shape, setTexture, void, const sf::Texture&, bool)
        .def("setTextureRect", &sf::Shape::setTextureRect)
        .def("setFillColor", &sf::Shape::setFillColor)
        .def("setOutlineColor", &sf::Shape::setOutlineColor)
        .def("setOutlineThickness", &sf::Shape::setOutlineThickness)
        .def("getTexture", &sf::Shape::getTexture,
            return_internal_reference<>())
        .def("getTextureRect", &sf::Shape::getTextureRect,
            return_value_policy<reference_existing_object>())
        .def("getFillColor", &sf::Shape::getFillColor,
            return_value_policy<reference_existing_object>())
        .def("getOutlineColor", &sf::Shape::getOutlineColor,
            return_value_policy<reference_existing_object>())
        .def("getOutlineThickness", &sf::Shape::getOutlineThickness)
        /*.def("getPointCount", pure_virtual(&sf::Shape::getPointCount))
        .def("getPoint", pure_virtual(&sf::Shape::getPoint))*/
        .def("getLocalBounds", &sf::Sprite::getLocalBounds)
        .def("getGlobalBounds", &sf::Sprite::getGlobalBounds)
    ; class_<sf::CircleShape, bases<sf::Shape> >("CircleShape",
        "Specialized shape representing a circle",
        bp::init< optional<float, unsigned int> >())
        .def("setRadius", &sf::CircleShape::setRadius)
        .def("getRadius", &sf::CircleShape::getRadius)
        .def("getPointCount", &sf::CircleShape::getPointCount)
        .def("getPoint", &sf::CircleShape::getPoint)
    ; class_<sf::ConvexShape, bases<sf::Shape> >("ConvexShape",
        "Specialized shape representing a convex polygon",
        bp::init< optional<unsigned int> >())
        .def("setPointCount", &sf::ConvexShape::setPointCount)
        .def("getPointCount", &sf::ConvexShape::getPointCount)
        .def("setPoint", &sf::ConvexShape::setPoint)
        .def("getPoint", &sf::ConvexShape::getPoint)
    ; class_<sf::RectangleShape, bases<sf::Shape> >("RectangleShape",
        "Specialized shape representing a rectangle",
        bp::init< optional<const sf::Vector2f&> >())
        .def("setSize", &sf::RectangleShape::setSize)
        .def("getSize", &sf::RectangleShape::getSize,
            return_value_policy<reference_existing_object>())
        .def("getPointCount", &sf::CircleShape::getPointCount)
        .def("getPoint", &sf::CircleShape::getPoint)
    ; class_<sf::Font>("Font",
        "Class for loading and manipulating character fonts")
        .def(bp::init<const sf::Font&>())
        .def("loadFromFile", &sf::Font::loadFromFile)
        //.def("GetGlyph", &sf::Font::getGlyph)
        .def("getKerning", &sf::Font::getKerning)
        .def("getLineSpacing", &sf::Font::getLineSpacing)
        .def("getTexture", &sf::Font::getTexture,
             return_value_policy<reference_existing_object>())
        .def("getDefaultFont", &sf::Font::getDefaultFont,
             return_value_policy<reference_existing_object>())
        .staticmethod("getDefaultFont")
    ; { scope nested = class_<sf::Text, bases<sf::Drawable, sf::Transformable> >("Text",
        "Graphical text that can be drawn to a render target")
        .def(bp::init<const sf::String&, optional<const sf::Font&, unsigned int> >())
        .def("setString", &sf::Text::setString)
        .def("setFont", &sf::Text::setFont)
        .def("setCharacterSize", &sf::Text::setCharacterSize)
        .def("setStyle", &sf::Text::setStyle)
        .def("setColor", &sf::Text::setColor)
        .def("getString", &sf::Text::getString,
            return_value_policy<reference_existing_object>())
        .def("getFont", &sf::Text::getFont,
            return_value_policy<reference_existing_object>())
        .def("getCharacterSize", &sf::Text::getCharacterSize)
        .def("getStyle", &sf::Text::getStyle)
        .def("getColor", &sf::Text::getColor,
            return_value_policy<reference_existing_object>())
        .def("findCharacterPos", &sf::Text::findCharacterPos)
        .def("getLocalBounds", &sf::Sprite::getLocalBounds)
        .def("getGlobalBounds", &sf::Sprite::getGlobalBounds)
        ; enum_<sf::Text::Style>("Style",
            "Enumeration of the string drawing styles")
            .value("Regular", sf::Text::Regular)
            .value("Bold", sf::Text::Bold)
            .value("Italic", sf::Text::Italic)
            .value("Underlined", sf::Text::Underlined)
            .export_values();
    } ; // end of sf::Text scope
    class_<sf::View>("View",
        "2D camera that defines what region is shown on screen")
        .def(bp::init<const sf::FloatRect&>())
        .def(bp::init<const sf::Vector2f&, const sf::Vector2f&>())
        .method(sf::View, setCenter, void, float, float)
        .method(sf::View, setCenter, void, const sf::Vector2f&)
        .method(sf::View, setSize, void, float, float)
        .method(sf::View, setSize, void, const sf::Vector2f&)
        .def("setRotation", &sf::View::setRotation)
        .def("setViewport", &sf::View::setViewport)
        .def("reset", &sf::View::reset)
        .def("getCenter", &sf::View::getCenter,
            return_value_policy<reference_existing_object>())
        .def("getSize", &sf::View::getSize,
            return_value_policy<reference_existing_object>())
        .def("getRotation", &sf::View::getRotation)
        .def("getViewport", &sf::View::getViewport,
            return_value_policy<reference_existing_object>())
        .method(sf::View, move, void, float, float)
        .method(sf::View, move, void, const sf::Vector2f&)
        .def("rotate", &sf::View::rotate)
        .def("zoom", &sf::View::zoom)
    ; class_<sf::RenderStates>("RenderStates",
        "Define the states used for drawing to a RenderTarget")
        .def(bp::init<sf::BlendMode>())
        .def(bp::init<const sf::Transform&>())
        .def(bp::init<const sf::Texture*>())
        .def(bp::init<const sf::Shader*>())
        .def(bp::init<sf::BlendMode, const sf::Transform&, const sf::Texture*,
             const sf::Shader*>())
        .def_readwrite("blendMode", &sf::RenderStates::blendMode)
        .def_readwrite("transform", &sf::RenderStates::transform)
        .def_readwrite("texture", &sf::RenderStates::texture)
        .def_readwrite("shader", &sf::RenderStates::shader)
        .add_static_property("Default", &sf::RenderStates::Default)
    ; class_<sf::RenderTarget, boost::noncopyable>("RenderTarget",
        "Base class for all render targets (window, image, ...)", no_init)
        .method(sf::RenderTarget, clear, void, void)
        .method(sf::RenderTarget, clear, void, const sf::Color&)
        .def("setView", &sf::RenderTarget::setView)
        .def("getView", &sf::RenderTarget::getView,
            return_internal_reference<>())
        .def("getDefaultView", &sf::RenderTarget::getDefaultView,
            return_internal_reference<>())
        .def("getViewport", &sf::RenderTarget::getViewport)
        .method_const(sf::RenderTarget, convertCoords, sf::Vector2f,
            unsigned int, unsigned int)
        .method_const(sf::RenderTarget, convertCoords, sf::Vector2f,
            unsigned int, unsigned int, const sf::View&)
        .method(sf::RenderTarget, draw, void, const sf::Drawable&, const sf::RenderStates&)
        .def("draw", &RenderTarget_draw) // only 1 argument (drawable)
        .def("draw", &sfTarget_draw_Entity)
        .def("draw", &sfTarget_draw_Entity2)
        .def("pushGLStates", &sf::RenderTarget::pushGLStates)
        .def("popGLStates", &sf::RenderTarget::popGLStates)
        .def("resetGLStates", &sf::RenderTarget::resetGLStates)
    ; class_<sf::RenderWindow, bases<sf::RenderTarget, sf::Window>, boost::noncopyable>(
        "RenderWindow", "Window that can serve as a target for 2D drawing")
        .def(bp::init<sf::VideoMode, const std::string&, optional<
             unsigned long, const sf::ContextSettings&> >())
        .def("capture", &sf::RenderWindow::capture)
        .def("getSize", &sf::Window::getSize)
    ;
}
