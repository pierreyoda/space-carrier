#include "PythonEmbedder.hpp"
#include <SFML/Graphics.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <thor/Multimedia/ToString.hpp>

using namespace boost::python;

// A macro to simplify overloaded member function (=method) binding
#define method(CLASS, METHOD, RTYPE, ARGS...) \
    def(BOOST_PP_STRINGIZE(METHOD), \
        (RTYPE(CLASS::*)(ARGS))&CLASS::METHOD)
// same but with arguments - const version
#define method_const(CLASS, METHOD, RTYPE, ARGS...) \
    def(BOOST_PP_STRINGIZE(METHOD), \
        (RTYPE(CLASS::*)(ARGS)const)&CLASS::METHOD)

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
            .def("__repr__", (std::string(*)(const sf::Vector2<T>&))&thor::ToString)
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
            .method_const(sf::Rect<T>, Contains, bool, T, T)
            .method_const(sf::Rect<T>, Contains, bool, const sf::Vector2<T>&)
            .method_const(sf::Rect<T>, Intersects, bool, const sf::Rect<T>&)
            .method_const(sf::Rect<T>, Intersects, bool, const sf::Rect<T>&, sf::Rect<T>&)
            .def_readwrite("Left", &sf::Rect<T>::Left)
            .def_readwrite("Top", &sf::Rect<T>::Top)
            .def_readwrite("Width", &sf::Rect<T>::Width)
            .def_readwrite("Height", &sf::Rect<T>::Height)
            .def("__repr__", (std::string(*)(const sf::Rect<T>&))&thor::ToString)
        ;
    }
};

/// In sf.Key class, for test purpose
unsigned int key_as_int(const sf::Keyboard::Key &key) { return key; }

inline unsigned int string_invalidpos() // avoid "TypeError: No to_python (by-value) converter found for C++ type: unsigned int"
{
    return sf::String::InvalidPos;
}

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Create_overloads, Create, 2, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(AddPoint_overloads,  AddPoint, 2, 4) // float version
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(AddPoint_overloads2, AddPoint, 1, 3) // vector2f version
BOOST_PYTHON_FUNCTION_OVERLOADS(Line_overloads, sf::Shape::Line, 6, 8) // same...
BOOST_PYTHON_FUNCTION_OVERLOADS(Line_overloads2, sf::Shape::Line, 4, 6)
BOOST_PYTHON_FUNCTION_OVERLOADS(Rectangle_overloads, sf::Shape::Rectangle, 5, 7)
BOOST_PYTHON_FUNCTION_OVERLOADS(Rectangle_overloads2, sf::Shape::Rectangle, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(Circle_overloads, sf::Shape::Circle, 4, 6)
BOOST_PYTHON_FUNCTION_OVERLOADS(Circle_overloads2, sf::Shape::Circle, 3, 5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Image_copy, Copy, 3, 5)

enum JoystickConstants
{
    Count = sf::Joystick::Count,
    ButtonCount = sf::Joystick::ButtonCount,
    AxisCount = sf::Joystick::AxisCount
};

// TODO (Pierre-Yves#5#): [EXPORT-SF] Cannot access to predefined sf::Color ("TypeError: 'Color' object is not callable")
void PythonEmbedder::exportSf()
{
    namespace bp = boost::python; // needed for init<...> to not collide with PythonEmbedder::init

    Vector2_wrapper<int>::wrap("Vector2i");
    Vector2_wrapper<float>::wrap("Vector2f");
    Vector2_wrapper<unsigned int>::wrap("Vector2u");
    Rect_wrapper<int>::wrap("IntRect");
    Rect_wrapper<float>::wrap("FloatRect");
    class_<sf::Color>("Color", "Utility class for manpulating RGBA colors")
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
        .def("__repr__", (std::string(*)(const sf::Color&))&thor::ToString)
    ; class_<sf::String>("String",
        "Utility string class that automatically handles conversions between types and encodings")
        .def(bp::init<const std::wstring&>())
        .def(bp::init<const sf::String&>())
        .method(sf::String, ToAnsiString, std::string, void)
        .def("ToWideString", &sf::String::ToWideString)
        .def(self += other<const sf::String>())
        .def("Clear", &sf::String::Clear)
        .def("__len__", &sf::String::GetSize)
        .def("IsEmpty", &sf::String::IsEmpty)
        .method(sf::String, Erase, void, std::size_t)
        .method(sf::String, Erase, void, std::size_t, std::size_t)
        .def("Insert", &sf::String::Insert)
        .method(sf::String, Find, std::size_t, const sf::String&)
        .method(sf::String, Find, std::size_t, const sf::String&, std::size_t)
        .add_property("data",
            range((sf::String::Iterator(sf::String::*)())&sf::String::Begin,
                (sf::String::Iterator(sf::String::*)())&sf::String::End))
        .add_property("data_const",
            range((sf::String::ConstIterator(sf::String::*)()const)&sf::String::Begin,
                (sf::String::ConstIterator(sf::String::*)()const)&sf::String::End))
        .add_static_property("InvalidPos", &string_invalidpos)
    ; implicitly_convertible<std::wstring, sf::String>();
    class_<sf::Drawable, boost::noncopyable>("Drawable",
        "Abstract base class for objects that can be drawn to a render target",
        no_init)
        .method(sf::Drawable, SetPosition, void, float, float)
        .method(sf::Drawable, SetPosition, void, const sf::Vector2f&)
        .def("SetX", &sf::Drawable::SetX)
        .def("SetY", &sf::Drawable::SetY)
        .method(sf::Drawable, SetScale, void, float, float)
        .method(sf::Drawable, SetScale, void, const sf::Vector2f&)
        .def("SetScaleX", &sf::Drawable::SetScaleX)
        .def("SetScaleY", &sf::Drawable::SetScaleY)
        .method(sf::Drawable, SetOrigin, void, const sf::Vector2f&)
        .method(sf::Drawable, SetOrigin, void, float, float)
        .def("SetRotation", &sf::Drawable::SetRotation)
        .def("SetColor", &sf::Drawable::SetColor)
        .def("SetBlendMode", &sf::Drawable::SetBlendMode)
        .def("GetPosition", &sf::Drawable::GetPosition,
            return_value_policy<copy_const_reference>())
        .def("GetScale", &sf::Drawable::GetScale,
            return_value_policy<copy_const_reference>())
        .def("GetOrigin", &sf::Drawable::GetOrigin,
            return_value_policy<copy_const_reference>())
        .def("GetRotation", &sf::Drawable::GetRotation)
        .def("GetColor", &sf::Drawable::GetColor,
            return_value_policy<reference_existing_object>())
        .def("GetBlendMode", &sf::Drawable::GetBlendMode)
        .method(sf::Drawable, Move, void, float, float)
        .method(sf::Drawable, Move, void, const sf::Vector2f&)
        .method(sf::Drawable, Scale, void, float, float)
        .method(sf::Drawable, Scale, void, const sf::Vector2f&)
        .def("Rotate", &sf::Drawable::Rotate)
        .def("TransformToLocal", &sf::Drawable::TransformToLocal)
        .def("TransformToGlobal", &sf::Drawable::TransformToGlobal)
    ; class_<sf::Image>("Image",
        "Class for loading, manipulating and saving images")
        .method(sf::Image, Create, void, unsigned int, unsigned int,
            const sf::Color&)
        .def("LoadFromFile", &sf::Image::LoadFromFile)
        .def("SaveToFile", &sf::Image::SaveToFile)
        .def("GetWidth", &sf::Image::GetWidth)
        .def("GetHeight", &sf::Image::GetHeight)
        .method(sf::Image, CreateMaskFromColor, void, const sf::Color&)
        .method(sf::Image, CreateMaskFromColor, void, const sf::Color&,
            sf::Uint8)
        .def("Copy", &sf::Image::Copy, Image_copy())
        .def("SetPixel", &sf::Image::SetPixel)
        .def("GetPixel", &sf::Image::GetPixel)
        //.def("GetPixelsPtr", &sf::Texture::GetPixelsPtr)
        .def("FlipHorizontally", &sf::Image::FlipHorizontally)
        .def("FlipVertically", &sf::Image::FlipVertically)
    ; class_<sf::Texture>("Texture",
        "Image living on the graphics card that can be used for drawing")
        .def(bp::init<const sf::Texture&>())
        .def("Create", &sf::Texture::Create)
        .method(sf::Texture, LoadFromFile, bool, const std::string&)
        .method(sf::Texture, LoadFromFile, bool, const std::string&,
            const sf::IntRect&)
        .method(sf::Texture, LoadFromImage, bool, const sf::Image&)
        .method(sf::Texture, LoadFromImage, bool, const sf::Image&,
            const sf::IntRect&)
        .def("GetWidth", &sf::Texture::GetWidth)
        .def("GetHeight", &sf::Texture::GetHeight)
        .def("CopyToImage", &sf::Texture::CopyToImage)
        .method(sf::Texture, Update, void, const sf::Image&)
        .method(sf::Texture, Update, void, const sf::Image&, unsigned int,
            unsigned int)
        .method(sf::Texture, Update, void, const sf::Window&)
        .method(sf::Texture, Update, void, const sf::Window&, unsigned int,
            unsigned int)
        .def("Bind", &sf::Texture::Bind)
        .def("SetSmooth", &sf::Texture::SetSmooth)
        .def("IsSmooth", &sf::Texture::IsSmooth)
        .def("GetTexCoords", &sf::Texture::GetTexCoords)
        .def("GetMaximumSize", &sf::Texture::GetMaximumSize)
        .staticmethod("GetMaximumSize")
    ; enum_<sf::Blend::Mode>("Blend",
        "Available blending modes for drawable objects")
        .value("Alpha", sf::Blend::Alpha)
        .value("Add", sf::Blend::Add)
        .value("Multiply", sf::Blend::Multiply)
        .value("None", sf::Blend::None)
    ; class_<sf::Sprite, bases<sf::Drawable> >("Sprite",
        "Drawable representation of an image, with its own transformations, color, blend mode, etc. ")
        .def(bp::init< optional<sf::Texture&> >())
        .method(sf::Sprite, SetTexture, void, const sf::Texture&)
        .method(sf::Sprite, SetTexture, void, const sf::Texture&, bool)
        .def("SetSubRect", &sf::Sprite::SetSubRect)
        .method(sf::Sprite, Resize, void, float, float)
        .method(sf::Sprite, Resize, void, const sf::Vector2f&)
        .def("FlipX", &sf::Sprite::FlipX)
        .def("FlipY", &sf::Sprite::FlipY)
        .def("GetTexture", &sf::Sprite::GetTexture,
            return_internal_reference<>())
        .def("GetSubRect", &sf::Sprite::GetSubRect,
            return_value_policy<reference_existing_object>())
        .def("GetSize", &sf::Sprite::GetSize)
    ;
    void (sf::Shape::*ap1)(float, float, const sf::Color&, const sf::Color&) =
        &sf::Shape::AddPoint;
    void (sf::Shape::*ap2)(const sf::Vector2f&, const sf::Color&,
        const sf::Color&) = &sf::Shape::AddPoint;
    sf::Shape (*line1)(float, float, float, float, float,
        const sf::Color&, float, const sf::Color&) = &sf::Shape::Line;
    sf::Shape (*line2)(const sf::Vector2f&, const sf::Vector2f&,
        float, const sf::Color&, float, const sf::Color&) = &sf::Shape::Line;
    sf::Shape (*recta1)(float, float, float, float, const sf::Color&, float,
        const sf::Color&) = &sf::Shape::Rectangle;
    sf::Shape (*recta2)(const sf::FloatRect&, const sf::Color&, float,
        const sf::Color&) = &sf::Shape::Rectangle;
    sf::Shape (*circle1)(float, float, float, const sf::Color&, float,
        const sf::Color&) = &sf::Shape::Circle;
    sf::Shape (*circle2)(const sf::Vector2f&, float, const sf::Color&, float,
        const sf::Color&) = &sf::Shape::Circle;
    ; class_<sf::Shape, bases<sf::Drawable> >("Shape",
        "A convex, colored polygon with an optional outline")
        .def("AddPoint", ap1, AddPoint_overloads())
        .def("AddPoint", ap2, AddPoint_overloads2())
        .def("GetPointsCount", &sf::Shape::GetPointsCount)
        .def("EnableFill", &sf::Shape::EnableFill)
        .def("EnableOutline", &sf::Shape::EnableOutline)
        .method(sf::Shape, SetPointPosition, void, unsigned int,
            const sf::Vector2f&)
        .method(sf::Shape, SetPointPosition, void, unsigned int, float, float)
        .def("SetPointColor", &sf::Shape::SetPointColor)
        .def("SetPointOutlineColor", &sf::Shape::SetPointOutlineColor)
        .def("SetOutlineThickness", &sf::Shape::SetOutlineThickness)
        .def("GetPointPosition", &sf::Shape::GetPointPosition,
            return_value_policy<reference_existing_object>())
        .def("GetPointColor", &sf::Shape::GetPointColor,
            return_value_policy<reference_existing_object>())
        .def("GetPointOutlineColor", &sf::Shape::GetPointOutlineColor,
            return_value_policy<reference_existing_object>())
        .def("GetOutlineThickness", &sf::Shape::GetOutlineThickness)
        .def("Line", line1, Line_overloads())
        .def("Line", line2, Line_overloads2())
        .def("Rectangle", recta1, Rectangle_overloads())
        .def("Rectangle", recta2, Rectangle_overloads2())
        .def("Circle", circle1, Circle_overloads())
        .def("Circle", circle2, Circle_overloads2())
        .staticmethod("Line")
        .staticmethod("Rectangle")
        .staticmethod("Circle")
    ; class_<sf::Font>("Font",
        "Class for loading and manipulating character fonts")
        .def(bp::init<const sf::Font&>())
        .def("LoadFromFile", &sf::Font::LoadFromFile)
        //.def("GetGlyph", &sf::Font::GetGlyph)
        .def("GetKerning", &sf::Font::GetKerning)
        .def("GetLineSpacing", &sf::Font::GetLineSpacing)
        .def("GetTexture", &sf::Font::GetTexture,
             return_value_policy<reference_existing_object>())
        .def("GetDefaultFont", &sf::Font::GetDefaultFont,
             return_value_policy<reference_existing_object>())
        .staticmethod("GetDefaultFont")
    ; { scope nested = class_<sf::Text, bases<sf::Drawable> >("Text",
        "Graphical text that can be drawn to a render target")
        .def(bp::init<const sf::String&, optional<const sf::Font&, unsigned int> >())
        .def("SetString", &sf::Text::SetString)
        .def("SetFont", &sf::Text::SetFont)
        .def("SetCharacterSize", &sf::Text::SetCharacterSize)
        .def("SetStyle", &sf::Text::SetStyle)
        .def("GetString", &sf::Text::GetString,
            return_value_policy<reference_existing_object>())
        .def("GetFont", &sf::Text::GetFont,
            return_value_policy<reference_existing_object>())
        .def("GetCharacterSize", &sf::Text::GetCharacterSize)
        .def("GetStyle", &sf::Text::GetStyle)
        .def("GetCharacterPos", &sf::Text::GetCharacterPos)
        .def("GetRect", &sf::Text::GetRect)
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
        .method(sf::View, SetCenter, void, float, float)
        .method(sf::View, SetCenter, void, const sf::Vector2f&)
        .method(sf::View, SetSize, void, float, float)
        .method(sf::View, SetSize, void, const sf::Vector2f&)
        .method(sf::View, Move, void, float, float)
        .method(sf::View, Move, void, const sf::Vector2f&)
        .def("Rotate", &sf::View::Rotate)
        .def("SetRotation", &sf::View::SetRotation)
        .def("Zoom", &sf::View::Zoom)
        .def("SetViewport", &sf::View::SetViewport)
        .def("Reset", &sf::View::Reset)
        .def("GetCenter", &sf::View::GetCenter,
            return_value_policy<reference_existing_object>())
        .def("GetSize", &sf::View::GetSize,
            return_value_policy<reference_existing_object>())
        .def("GetRotation", &sf::View::GetRotation)
        .def("GetViewport", &sf::View::GetViewport,
            return_value_policy<reference_existing_object>())
    ; class_<sf::RenderTarget, boost::noncopyable>("RenderTarget",
        "Base class for all render targets (window, image, ...)", no_init)
        .method(sf::RenderTarget, Clear, void, void)
        .method(sf::RenderTarget, Clear, void, const sf::Color&)
        .method(sf::RenderTarget, Draw, void, const sf::Drawable&)
        .method(sf::RenderTarget, Draw, void, const sf::Drawable&,
            const sf::Shader&)
        .def("GetWidth", &sf::RenderTarget::GetWidth)
        .def("GetHeight", &sf::RenderTarget::GetHeight)
        .def("SetView", &sf::RenderTarget::SetView)
        .def("GetView", &sf::RenderTarget::GetView,
            return_value_policy<reference_existing_object>())
        .def("GetDefaultView", &sf::RenderTarget::GetDefaultView,
            return_value_policy<reference_existing_object>())
        .def("GetViewport", &sf::RenderTarget::GetViewport)
        .method_const(sf::RenderTarget, ConvertCoords, sf::Vector2f,
            unsigned int, unsigned int)
        .method_const(sf::RenderTarget, ConvertCoords, sf::Vector2f,
            unsigned int, unsigned int, const sf::View&)
    ; class_<sf::Window, boost::noncopyable>("Window",
        "Window that serves as a target for OpenGL rendering")
        .def(bp::init<sf::VideoMode, const std::string&, optional<
             unsigned long, const sf::ContextSettings&> >())
        .def("Close", &sf::Window::Close)
        .def("IsOpened", &sf::Window::IsOpened)
        .def("GetWidth", &sf::Window::GetWidth)
        .def("GetHeight", &sf::Window::GetHeight)
        .def("GetSettings", &sf::Window::GetSettings,
             return_value_policy<reference_existing_object>())
        .def("PollEvent", &sf::Window::PollEvent)
        .def("WaitEvent", &sf::Window::WaitEvent)
        .def("EnableVerticalSync", &sf::Window::EnableVerticalSync)
        .def("ShowMouseCursor", &sf::Window::ShowMouseCursor)
        .def("SetPosition", &sf::Window::SetPosition)
        .def("SetSize", &sf::Window::SetSize)
        .def("SetTitle", &sf::Window::SetTitle)
        .def("Show", &sf::Window::Show)
        .def("EnableKeyRepeat", &sf::Window::EnableKeyRepeat)
        //.def("SetIcon", &sf::Window::SetIcon) // TO TEST
        .def("SetActive", &sf::Window::SetActive)
        .def("Display", &sf::Window::Display)
        .def("SetFramerateLimit", &sf::Window::SetFramerateLimit)
        .def("GetFrameTime", &sf::Window::GetFrameTime)
        .def("SetJoystickThreshold", &sf::Window::SetJoystickThreshold)
    ; class_<sf::RenderWindow, bases<sf::RenderTarget, sf::Window>, boost::noncopyable>(
        "RenderWindow", "Window that can serve as a target for 2D drawing")
        .def(bp::init<sf::VideoMode, const std::string&, optional<
             unsigned long, const sf::ContextSettings&> >())
        .def("Capture", &sf::RenderWindow::Capture)
        .def("GetWidth", &sf::RenderWindow::GetWidth)
        .def("GetHeight", &sf::RenderWindow::GetHeight)
    ; class_<sf::ContextSettings>("ContextSettings",
        "Structure defining the settings of the OpenGL context attached to a window")
        .def_readwrite("DepthBits", &sf::ContextSettings::DepthBits)
        .def_readwrite("StencilBits", &sf::ContextSettings::StencilBits)
        .def_readwrite("AntialiasingLevel", &sf::ContextSettings::AntialiasingLevel)
        .def_readwrite("MajorVersion", &sf::ContextSettings::MajorVersion)
        .def_readwrite("MinorVersion", &sf::ContextSettings::MinorVersion)
    ; class_<sf::VideoMode>("VideoMode",
        "VideoMode defines a video mode (width, height, bpp)")
        .def(bp::init<unsigned int, unsigned int, optional<unsigned int> >())
        .def("IsValid", &sf::VideoMode::IsValid)
        .def("GetDesktopMode", &sf::VideoMode::GetDesktopMode)
        .def_readwrite("Width", &sf::VideoMode::Width)
        .def_readwrite("Height", &sf::VideoMode::Height)
        .def_readwrite("BitsPerPixel", &sf::VideoMode::BitsPerPixel)
        .staticmethod("GetDesktopMode")
        .def("GetFullscreenModes", &sf::VideoMode::GetFullscreenModes,
            return_value_policy<reference_existing_object>())
        .staticmethod("GetFullscreenModes")
    ; class_<std::vector<sf::VideoMode> >("VideoModeVec")
        .def(vector_indexing_suite<std::vector<sf::VideoMode> >())
    ; { scope nested = class_<sf::Mouse>("Mouse",
        "Give access to the real-time state of the mouse")
        .def("IsButtonPressed", &sf::Mouse::IsButtonPressed)
        .def("GetPosition", static_cast<sf::Vector2i(*)()>(
            &sf::Mouse::GetPosition))
        .def("GetPosition", static_cast<sf::Vector2i(*)(const sf::Window&)>(&
            sf::Mouse::GetPosition))
        .def("SetPosition", static_cast<void(*)(const sf::Vector2i&)>(
            &sf::Mouse::SetPosition))
        .def("SetPosition", static_cast<void(*)(const sf::Vector2i&,
            const sf::Window&)>(&sf::Mouse::SetPosition))
        .staticmethod("IsButtonPressed")
        .staticmethod("GetPosition")
        .staticmethod("SetPosition")
    ; enum_<sf::Mouse::Button>("Button", "Mouse buttons")
        .value("Left", sf::Mouse::Left)
        .value("Right", sf::Mouse::Right)
        .value("Middle", sf::Mouse::Middle)
        .value("XButton1", sf::Mouse::XButton1)
        .value("XButton2", sf::Mouse::XButton2)
        .value("ButtonCount", sf::Mouse::ButtonCount)
        .export_values() // export as nested to sf::Mouse scope
    ; } // end of sf::Mouse scope
    { scope nested = class_<sf::Keyboard>("Keyboard",
        "Give access to the real-time state of the keyboard")
        .def("IsKeyPressed", &sf::Keyboard::IsKeyPressed)
        .staticmethod("IsKeyPressed")
    ; enum_<sf::Keyboard::Key>("Key", "Key codes")
        .value("A", sf::Keyboard::A)
        .value("B", sf::Keyboard::B)
        .value("C", sf::Keyboard::C)
        .value("D", sf::Keyboard::D)
        .value("E", sf::Keyboard::E)
        .value("F", sf::Keyboard::F)
        .value("G", sf::Keyboard::G)
        .value("H", sf::Keyboard::H)
        .value("I", sf::Keyboard::I)
        .value("J", sf::Keyboard::J)
        .value("K", sf::Keyboard::K)
        .value("L", sf::Keyboard::L)
        .value("M", sf::Keyboard::M)
        .value("N", sf::Keyboard::N)
        .value("O", sf::Keyboard::O)
        .value("P", sf::Keyboard::P)
        .value("Q", sf::Keyboard::Q)
        .value("R", sf::Keyboard::R)
        .value("S", sf::Keyboard::S)
        .value("T", sf::Keyboard::T)
        .value("U", sf::Keyboard::U)
        .value("V", sf::Keyboard::V)
        .value("W", sf::Keyboard::W)
        .value("X", sf::Keyboard::X)
        .value("Y", sf::Keyboard::Y)
        .value("Z", sf::Keyboard::Z)
        .value("Num0", sf::Keyboard::Num0)
        .value("Num1", sf::Keyboard::Num1)
        .value("Num2", sf::Keyboard::Num2)
        .value("Num3", sf::Keyboard::Num3)
        .value("Num4", sf::Keyboard::Num4)
        .value("Num5", sf::Keyboard::Num5)
        .value("Num6", sf::Keyboard::Num6)
        .value("Num7", sf::Keyboard::Num7)
        .value("Num8", sf::Keyboard::Num8)
        .value("Num9", sf::Keyboard::Num9)
        .value("Escape", sf::Keyboard::Escape)
        .value("LControl", sf::Keyboard::LControl)
        .value("LShift", sf::Keyboard::LShift)
        .value("LAlt", sf::Keyboard::LAlt)
        .value("LSystem", sf::Keyboard::LSystem)
        .value("RControl", sf::Keyboard::RControl)
        .value("RShift", sf::Keyboard::RShift)
        .value("RAlt", sf::Keyboard::RAlt)
        .value("RSystem", sf::Keyboard::RSystem)
        .value("Menu", sf::Keyboard::Menu)
        .value("LBracket", sf::Keyboard::LBracket)
        .value("RBracket", sf::Keyboard::RBracket)
        .value("SemiColon", sf::Keyboard::SemiColon)
        .value("Comma", sf::Keyboard::Comma)
        .value("Period", sf::Keyboard::Period)
        .value("Quote", sf::Keyboard::Quote)
        .value("Slash", sf::Keyboard::Slash)
        .value("BackSlash", sf::Keyboard::BackSlash)
        .value("Tilde", sf::Keyboard::Tilde)
        .value("Equal", sf::Keyboard::Equal)
        .value("Dash", sf::Keyboard::Dash)
        .value("Space", sf::Keyboard::Space)
        .value("Return", sf::Keyboard::Return)
        .value("Back", sf::Keyboard::Back)
        .value("Tab", sf::Keyboard::Tab)
        .value("PageUp", sf::Keyboard::PageUp)
        .value("PageDown", sf::Keyboard::PageDown)
        .value("End", sf::Keyboard::End)
        .value("Home", sf::Keyboard::Home)
        .value("Insert", sf::Keyboard::Insert)
        .value("Delete", sf::Keyboard::Delete)
        .value("Add", sf::Keyboard::Add)
        .value("Subtract", sf::Keyboard::Subtract)
        .value("Multiply", sf::Keyboard::Multiply)
        .value("Divide", sf::Keyboard::Divide)
        .value("Left", sf::Keyboard::Left)
        .value("Right", sf::Keyboard::Right)
        .value("Up", sf::Keyboard::Up)
        .value("Down", sf::Keyboard::Down)
        .value("Numpad0", sf::Keyboard::Numpad0)
        .value("Numpad1", sf::Keyboard::Numpad1)
        .value("Numpad2", sf::Keyboard::Numpad2)
        .value("Numpad3", sf::Keyboard::Numpad3)
        .value("Numpad4", sf::Keyboard::Numpad4)
        .value("Numpad5", sf::Keyboard::Numpad5)
        .value("Numpad6", sf::Keyboard::Numpad6)
        .value("Numpad7", sf::Keyboard::Numpad7)
        .value("Numpad8", sf::Keyboard::Numpad8)
        .value("Numpad9", sf::Keyboard::Numpad9)
        .value("F1", sf::Keyboard::F1)
        .value("F2", sf::Keyboard::F2)
        .value("F3", sf::Keyboard::F3)
        .value("F4", sf::Keyboard::F4)
        .value("F5", sf::Keyboard::F5)
        .value("F6", sf::Keyboard::F6)
        .value("F7", sf::Keyboard::F7)
        .value("F8", sf::Keyboard::F8)
        .value("F9", sf::Keyboard::F9)
        .value("F10", sf::Keyboard::F10)
        .value("F11", sf::Keyboard::F11)
        .value("F12", sf::Keyboard::F12)
        .value("F13", sf::Keyboard::F13)
        .value("F14", sf::Keyboard::F14)
        .value("F15", sf::Keyboard::F15)
        .value("Pause", sf::Keyboard::Pause)
        .value("KeyCount", sf::Keyboard::KeyCount)
        .export_values(); // end of sf::Keyboard::Key enum
        def("key_as_int", &key_as_int)
    ; } // end of sf::Keyboard scope
    { scope nested = class_<sf::Joystick>("Joystick",
        "Give access to the real-time state of the joysticks")
        .def("IsConnected", &sf::Joystick::IsConnected)
        .def("GetButtonCount", &sf::Joystick::GetButtonCount)
        .def("HasAxis", &sf::Joystick::HasAxis)
        .def("IsButtonPressed", &sf::Joystick::IsButtonPressed)
        .def("GetAxisPosition", &sf::Joystick::GetAxisPosition)
        .def("Update", &sf::Joystick::Update)
        .staticmethod("IsConnected")
        .staticmethod("GetButtonCount")
        .staticmethod("HasAxis")
        .staticmethod("IsButtonPressed")
        .staticmethod("GetAxisPosition")
        .staticmethod("Update")
    ; enum_<JoystickConstants>("Constants",
        "Constants related to joysticks capabilities")
        .value("Count", Count)
        .value("ButtonCount", ButtonCount)
        .value("AxisCount", AxisCount)
        .export_values()
    ; enum_<sf::Joystick::Axis>("Axis", "Axes supported by SFML joysticks")
        .value("X", sf::Joystick::X)
        .value("Y", sf::Joystick::Y)
        .value("Z", sf::Joystick::Z)
        .value("R", sf::Joystick::R)
        .value("U", sf::Joystick::U)
        .value("V", sf::Joystick::V)
        .value("PovX", sf::Joystick::PovX)
        .value("PovY", sf::Joystick::PovY)
        .export_values()
    ; } // end of sf::Joystick scope
    { scope nested = class_<sf::Event>("Event")
        .def_readwrite("Type", &sf::Event::Type)
        .def_readwrite("Size", &sf::Event::Size)
        .def_readwrite("Key", &sf::Event::Key)
        .def_readwrite("Text", &sf::Event::Text)
        .def_readwrite("MouseMove", &sf::Event::MouseMove)
        .def_readwrite("MouseButton", &sf::Event::MouseButton)
        .def_readwrite("MouseWheel", &sf::Event::MouseWheel)
        .def_readwrite("JoystickMove", &sf::Event::JoystickMove)
        .def_readwrite("JoystickButton", &sf::Event::JoystickButton)
        .def_readwrite("JoystickConnect", &sf::Event::JoystickConnect)
    ; enum_<sf::Event::EventType>("EventType")
        .value("Closed", sf::Event::Closed)
        .value("Resized", sf::Event::Resized)
        .value("LostFocus", sf::Event::LostFocus)
        .value("GainedFocus", sf::Event::GainedFocus)
        .value("TextEntered", sf::Event::TextEntered)
        .value("KeyPressed", sf::Event::KeyPressed)
        .value("KeyReleased", sf::Event::KeyReleased)
        .value("MouseWheelMoved", sf::Event::MouseWheelMoved)
        .value("MouseButtonPressed", sf::Event::MouseButtonPressed)
        .value("MouseButtonReleased", sf::Event::MouseButtonReleased)
        .value("MouseMoved", sf::Event::MouseMoved)
        .value("MouseEntered", sf::Event::MouseEntered)
        .value("MouseLeft", sf::Event::MouseLeft)
        .value("JoystickButtonPressed", sf::Event::JoystickButtonPressed)
        .value("JoystickButtonReleased", sf::Event::JoystickButtonReleased)
        .value("JoystickMoved", sf::Event::JoystickMoved)
        .value("JoystickConnected", sf::Event::JoystickConnected)
        .value("JoystickDisconnected", sf::Event::JoystickDisconnected)
        .value("Count", sf::Event::Count)
        .export_values()
    ; class_<sf::Event::SizeEvent>("SizeEvent")
        .def_readwrite("Width", &sf::Event::SizeEvent::Width)
        .def_readwrite("Height", &sf::Event::SizeEvent::Height)
    ; class_<sf::Event::KeyEvent>("KeyEvent")
        .def_readwrite("Code", &sf::Event::KeyEvent::Code)
        .def_readwrite("Alt", &sf::Event::KeyEvent::Alt)
        .def_readwrite("Control", &sf::Event::KeyEvent::Control)
        .def_readwrite("Shift", &sf::Event::KeyEvent::Shift)
        .def_readwrite("System", &sf::Event::KeyEvent::System)
    ; class_<sf::Event::TextEvent>("TextEvent")
        .def_readwrite("Unicode", &sf::Event::TextEvent::Unicode)
    ; class_<sf::Event::MouseMoveEvent>("MouseMoveEvent")
        .def_readwrite("X", &sf::Event::MouseMoveEvent::X)
        .def_readwrite("Y", &sf::Event::MouseMoveEvent::Y)
    ; class_<sf::Event::MouseButtonEvent>("MouseButtonEvent")
        .def_readwrite("Button", &sf::Event::MouseButtonEvent::Button)
        .def_readwrite("X", &sf::Event::MouseButtonEvent::X)
        .def_readwrite("Y", &sf::Event::MouseButtonEvent::Y)
    ; class_<sf::Event::MouseWheelEvent>("MouseWheelEvent")
        .def_readwrite("Delta", &sf::Event::MouseWheelEvent::Delta)
        .def_readwrite("X", &sf::Event::MouseWheelEvent::X)
        .def_readwrite("Y", &sf::Event::MouseWheelEvent::Y)
    ; class_<sf::Event::JoystickMoveEvent>("JoystickMoveEvent")
        .def_readwrite("JoystickId", &sf::Event::JoystickMoveEvent::JoystickId)
        .def_readwrite("Axis", &sf::Event::JoystickMoveEvent::Axis)
        .def_readwrite("Position", &sf::Event::JoystickMoveEvent::Position)
    ; class_<sf::Event::JoystickButtonEvent>("JoystickButtonEvent")
        .def_readwrite("JoystickId", &sf::Event::JoystickButtonEvent::JoystickId)
        .def_readwrite("Button", &sf::Event::JoystickButtonEvent::Button)
    ; class_<sf::Event::JoystickConnectEvent>("JoystickConnectEvent")
        .def_readwrite("JoystickId", &sf::Event::JoystickConnectEvent::JoystickId)
    ; } // end of sf::Event scope
}
