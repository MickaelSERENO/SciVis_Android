#include "Graphics/Color.h"

namespace sereno
{
    const Color Color::WHITE       = Color(1.0, 1.0, 1.0, 1.0);
    const Color Color::BLACK       = Color(0.0, 0.0, 0.0, 1.0);
    const Color Color::RED         = Color(1.0, 0.0, 0.0, 1.0);
    const Color Color::GREEN       = Color(0.0, 1.0, 0.0, 1.0);
    const Color Color::BLUE        = Color(0.0, 0.0, 1.0, 1.0);
    const Color Color::MAGENTA     = Color(1.0, 0.0, 1.0, 1.0);
    const Color Color::YELLOW      = Color(1.0, 1.0, 0.0, 1.0);
    const Color Color::CYAN        = Color(0.0, 1.0, 1.0, 1.0);
    const Color Color::TRANSPARENT = Color(0.0, 0.0, 0.0, 0.0);

    Color::Color(float red, float green, float blue, float alpha) : r(red), g(green), b(blue), a(alpha)
    {}

    Color::Color(const Color& color) : r(color.r), g(color.g), b(color.b), a(color.a)
    {
    }

    Color::Color(float* color) : r(color[0]), g(color[1]), b(color[2]), a(color[3])
    {}

    Color Color::operator=(const Color& color)
    {
        if(this != &color)
        {
            r = color.r;
            g = color.g;
            b = color.b;
            a = color.a;
        }
        return *this;
    }

    void Color::getFloatArray(float* array) const
    {
        array[0] = r;
        array[1] = g;
        array[2] = b;
        array[3] = a;
    }
}
