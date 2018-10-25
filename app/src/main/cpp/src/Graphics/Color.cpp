#include "Graphics/Color.h"

namespace sereno
{

    /*----------------------------------------------------------------------------*/
    /*-----------------------------------Colors-----------------------------------*/
    /*----------------------------------------------------------------------------*/

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


    /*----------------------------------------------------------------------------*/
    /*----------------------------------HSVColor----------------------------------*/
    /*----------------------------------------------------------------------------*/

    HSVColor::HSVColor(float _h, float _s, float _v, float _a) : h(_h), s(_s), v(_v), a(_a)
    {}

    HSVColor::HSVColor(const Color& color)
    {
        setFromRGB(color);
    }

    HSVColor::HSVColor(const HSVColor& copy)
    {
        *this = copy;
    }

    HSVColor::HSVColor(HSVColor&& mvt)
    {
        *this = mvt;
    }

    HSVColor& HSVColor::operator=(const HSVColor& copy)
    {
        if(this != &copy)
        {
            h = copy.h;
            s = copy.s;
            v = copy.v;
            a = copy.a;
        }

        return *this;
    }

    HSVColor HSVColor::operator+(const HSVColor& color) const
    {
        return HSVColor(h + color.h,
                        s + color.s,
                        v + color.v,
                        a + color.a);
    }

    HSVColor HSVColor::operator-(const HSVColor& color) const
    {
        return HSVColor(h - color.h,
                        s - color.s,
                        v - color.v,
                        a - color.a);
    }

    void HSVColor::operator+=(const HSVColor& color)
    {
        *this = *this + color;
    }

    void HSVColor::operator-=(const HSVColor& color)
    {
        *this = *this - color;
    }

    HSVColor HSVColor::operator*(float t) const
    {
        return HSVColor(h * t,
                        s * t,
                        v * t,
                        a * t);
    }

    void HSVColor::operator*=(float t)
    {
        *this = *this * t;
    }

    HSVColor operator*(float t, const HSVColor& color)
    {
        return color * t;
    }

    void HSVColor::setFromRGB(const Color& color)
    {
        float max = (float)fmax(fmax(color.r, color.g), color.b);
        float min = (float)fmin(fmin(color.r, color.g), color.g);
        float c   = max-min;

        //Compute the Hue
        if(c == 0)
            h = 0;
        else if(max == color.r)
            h = (color.g - color.b)/c;
        else if(max == color.g)
            h = (color.b - color.r)/c;
        else if(max == color.b)
            h = (color.r - color.g)/c;
        h *= 60.0f;

        //Compute the Saturation
        if(max == 0)
            s = 0;
        else
            s = c/max;

        //Compute the Value
        v = max;
    }

    Color HSVColor::toRGB() const
    {
        float c  = v*s;
        float h2 = h/60.0f;
        float x  = c*(1-(h2 - h2/2.0f - 1));
        float m  = v + c;
        switch((int)h2)
        {
            case 0:
                return Color(c+m, x+m, m, a);
            case 1:
                return Color(x+m, c+m, m, a);
            case 2:
                return Color(m, c+m, x+m, a);
            case 3:
                return Color(m, x+m, c+m, a);
            case 4:
                return Color(x+m, m, c+m, a);
            default:
                return Color(c+m, m, x+m, a);
        }
    }


    /*----------------------------------------------------------------------------*/
    /*-------------------------------XYZ colorspace-------------------------------*/
    /*----------------------------------------------------------------------------*/
}
