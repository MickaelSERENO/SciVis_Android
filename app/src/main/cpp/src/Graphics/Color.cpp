#include "Graphics/Color.h"

namespace sereno
{

    /*----------------------------------------------------------------------------*/
    /*-----------------------------------Colors-----------------------------------*/
    /*----------------------------------------------------------------------------*/

    const Color Color::COLD_COLOR  = Color(59.0f/255.0f, 76.0f/255.0f, 192.0f/255.0f, 1.0f);
    const Color Color::WARM_COLOR  = Color(180.0/255.0f,  4.0f/255.0f,  38.0f/255.0f, 1.0f);
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
        float min = (float)fmin(fmin(color.r, color.g), color.b);
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
        float h2 = (h/60.0f);
        float x  = c*(1.0f-fabs(fmod(h2, 2.0f) - 1.0f));
        float m  = v - c;
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
    
    const XYZColor XYZColor::REFERENCE = XYZColor(0.9505f, 1.01f, 1.0890f, 1.0f);

    XYZColor::XYZColor(float _x, float _y, float _z, float _a) : x(_x), y(_y), z(_z), a(_a)
    {}

    XYZColor::XYZColor(const Color& color)
    {
        setFromRGB(color);
    }

    XYZColor::XYZColor(const XYZColor& copy)
    {
        *this = copy;
    }

    XYZColor::XYZColor(XYZColor&& mvt)
    {
        *this = mvt;
    }

    XYZColor& XYZColor::operator=(const XYZColor& copy)
    {
        if(this != &copy)
        {
            x = copy.x;
            y = copy.y;
            z = copy.z;
            a = copy.a;
        }
        return *this;
    }

    XYZColor XYZColor::operator+(const XYZColor& color) const
    {
        return XYZColor(x + color.x,
                        y + color.y,
                        z + color.z,
                        a + color.a);
    }

    XYZColor XYZColor::operator-(const XYZColor& color) const
    {
        return XYZColor(x - color.x,
                        y - color.y,
                        z - color.z,
                        a - color.a);
    }

    XYZColor XYZColor::operator*(float t) const
    {
        return XYZColor(x * t,
                        y * t,
                        z * t,
                        a * t);
    }

    void XYZColor::operator+=(const XYZColor& color)
    {
        *this = *this + color;
    }

    void XYZColor::operator-=(const XYZColor& color)
    {
        *this = *this - color;
    }

    void XYZColor::operator*=(float t)
    {
        *this = *this * t;
    }

    void XYZColor::setFromRGB(const Color& color)
    {
        x = (float)(color.r*0.4124 + color.g*0.3576 + color.b*0.1805);
        y = (float)(color.r*0.2126 + color.g*0.7152 + color.b*0.0722);
        z = (float)(color.r*0.0193 + color.g*0.1192 + color.b*0.9505);
        a = color.a;
    }

    Color XYZColor::toRGB() const
    {
        return Color(fmin(1.0f,  3.2405f*x - 1.5371f*y - 0.4985f*z),
                     fmin(1.0f, -0.9692f*x + 1.8760f*y + 0.0415f*z),
                     fmin(1.0f,  0.0556f*x - 0.2040f*y + 1.0572f*z),
                     a);
    }

    /*----------------------------------------------------------------------------*/
    /*---------------------------------LAB Color----------------------------------*/
    /*----------------------------------------------------------------------------*/

    const LABColor LABColor::COLD_COLOR = LABColor(Color::COLD_COLOR);
    const LABColor LABColor::WHITE      = LABColor(Color::WHITE);
    const LABColor LABColor::WARM_COLOR = LABColor(Color::WARM_COLOR);

    LABColor::LABColor(float _l, float _a, float _b, float _transparency) : l(_l), a(_a), b(_b), transparency(_transparency)
    {}

    LABColor::LABColor(const Color& color)
    {
        setFromRGB(color);
    }

    LABColor::LABColor(const XYZColor& color)
    {
        setFromXYZ(color);
    }

    LABColor::LABColor(const LABColor& copy)
    {
        *this = copy;
    }

    LABColor::LABColor(LABColor&& mvt)
    {
        *this = mvt;
    }

    LABColor& LABColor::operator=(const LABColor& copy)
    {
        if(this != &copy)
        {
            l = copy.l;
            a = copy.a;
            b = copy.b;
            transparency = copy.transparency;
        }
        return *this;
    }

    LABColor LABColor::operator+(const LABColor& color) const
    {
        return LABColor(l + color.l,
                        a + color.a,
                        b + color.b,
                        transparency + color.transparency);
    }

    LABColor LABColor::operator-(const LABColor& color) const
    {
        return LABColor(l - color.l,
                        a - color.a,
                        b - color.b,
                        transparency - color.transparency);
    }

    LABColor LABColor::operator*(float t) const
    {
        return LABColor(l * t,
                        a * t,
                        b * t,
                        transparency * t);
    }

    void LABColor::operator+=(const LABColor& color)
    {
        *this = *this + color;
    }

    void LABColor::operator-=(const LABColor& color)
    {
        *this = *this - color;
    }

    void LABColor::operator*=(float t)
    {
        *this = *this * t;
    }

    void LABColor::setFromXYZ(const XYZColor& xyz)
    {
        float fX = f(xyz.x/XYZColor::REFERENCE.x);
        float fY = f(xyz.y/XYZColor::REFERENCE.y);
        float fZ = f(xyz.z/XYZColor::REFERENCE.z);

        l = 116*fY - 16.0f;
        a = 500*(fX - fY);
        b = 200*(fY - fZ);

        transparency = xyz.a;
    }

    void LABColor::setFromRGB(const Color& rgb)
    {
        setFromXYZ(XYZColor(rgb));
    }

    XYZColor LABColor::toXYZ() const
    {
        return XYZColor(XYZColor::REFERENCE.x * invF((float)((l+16.0)/116.0 + a/500.0)),
                        XYZColor::REFERENCE.y * invF((float)((l+16.0)/116.0)),
                        XYZColor::REFERENCE.z * invF((float)((l+16.0)/116.0 - b/200.0)),
                        transparency);
    }

    Color LABColor::toRGB() const
    {
        return toXYZ().toRGB();
    }

    /*----------------------------------------------------------------------------*/
    /*----------------------------------LUVColor----------------------------------*/
    /*----------------------------------------------------------------------------*/

    const LUVColor LUVColor::COLD_COLOR = LUVColor(Color::COLD_COLOR);
    const LUVColor LUVColor::WHITE      = LUVColor(Color::WHITE);
    const LUVColor LUVColor::WARM_COLOR = LUVColor(Color::WARM_COLOR);

    LUVColor::LUVColor(float _l, float _u, float _v, float _a) : l(_l), u(_u), v(_v), a(_a)
    {}

    LUVColor::LUVColor(const Color& color)
    {
        setFromRGB(color);
    }

    LUVColor::LUVColor(const XYZColor& color)
    {
        setFromXYZ(color);
    }

    LUVColor::LUVColor(const LUVColor& copy)
    {
        *this = copy;
    }

    LUVColor::LUVColor(LUVColor&& mvt)
    {
        *this = mvt;
    }

    LUVColor& LUVColor::operator=(const LUVColor& copy)
    {
        if(this != &copy)
        {
            l = copy.l;
            u = copy.u;
            v = copy.v;
            a = copy.a;
        }
        return *this;
    }

    LUVColor LUVColor::operator+(const LUVColor& color) const
    {
        return LUVColor(l + color.l,
                        u + color.u,
                        v + color.v,
                        a + color.a);
    }

    LUVColor LUVColor::operator-(const LUVColor& color) const
    {
        return LUVColor(l - color.l,
                        u - color.u,
                        v - color.v,
                        a - color.a);
    }

    LUVColor LUVColor::operator*(float t) const
    {
        return LUVColor(l * t,
                        u * t,
                        v * t,
                        a * t);
    }

    void LUVColor::operator+=(const LUVColor& color)
    {
        *this = *this + color;
    }

    void LUVColor::operator-=(const LUVColor& color)
    {
        *this = *this - color;
    }

    void LUVColor::operator*=(float t)
    {
        *this = *this * t;
    }

    void LUVColor::setFromRGB(const Color& color)
    {
        setFromXYZ(XYZColor(color));
    }

    void LUVColor::setFromXYZ(const XYZColor& xyz)
    {
        float un = 4*XYZColor::REFERENCE.x/(XYZColor::REFERENCE.x+15*XYZColor::REFERENCE.y+3*XYZColor::REFERENCE.z);
        float vn = 9*XYZColor::REFERENCE.y/(XYZColor::REFERENCE.x+15*XYZColor::REFERENCE.y+3*XYZColor::REFERENCE.z);

        float y = xyz.y/XYZColor::REFERENCE.y;

        if(y < 0.008856f)      //(6/29)**3 =   0.008856
            l = 903.296296f*y; //(29/3)**3 = 903.296296
        else
            l = 116.0f*(float)(pow(y, 1.0f/3.0f)) - 16.0f;

        u = 13.0f*l * (4.0f*xyz.x/(xyz.x + 15.0f*xyz.y + 3.0f*xyz.z) - un);
        v = 13.0f*l * (9.0f*xyz.y/(xyz.x + 15.0f*xyz.y + 3.0f*xyz.z) - vn);

        a = xyz.a;
    }

    Color LUVColor::toRGB() const
    {
        return toXYZ().toRGB();
    }

    XYZColor LUVColor::toXYZ() const
    {

        float un = 4*XYZColor::REFERENCE.x/(XYZColor::REFERENCE.x+15*XYZColor::REFERENCE.y+3*XYZColor::REFERENCE.z);
        float vn = 9*XYZColor::REFERENCE.y/(XYZColor::REFERENCE.x+15*XYZColor::REFERENCE.y+3*XYZColor::REFERENCE.z);

        float uprime = u/(13.0f*l) + un;
        float vprime = v/(13.0f*l) + vn;

        float z = 0.0f;
        float y = 0.0f;
        float x = 0.0f;

        if(l <= 8.0)
            y = XYZColor::REFERENCE.y*l*(0.001107056f); //0.001107056 = (3.0/29.0)**3
        else
        {
            float lprime = (l+16.0f)/116.0f;
            y = XYZColor::REFERENCE.y*lprime*lprime*lprime;
        }
        x = y*9*uprime/(4*vprime);
        z = y*(12 - 3*uprime - 20*vprime)/(4*vprime);

        return XYZColor(x, y, z, a);
    }

    /*----------------------------------------------------------------------------*/
    /*---------------------------------MSH Color----------------------------------*/
    /*----------------------------------------------------------------------------*/

    const MSHColor MSHColor::COLD_COLOR = MSHColor(Color::COLD_COLOR);
    const MSHColor MSHColor::WHITE      = MSHColor(Color::WHITE);
    const MSHColor MSHColor::WARM_COLOR = MSHColor(Color::WARM_COLOR);

    MSHColor::MSHColor(float _m, float _s, float _h, float _a) : m(_m), s(_s), h(_h), a(_a)
    {}

    MSHColor::MSHColor(const Color& color)
    {
        setFromRGB(color);
    }

    MSHColor::MSHColor(const XYZColor& color)
    {
        setFromXYZ(color);
    }

    MSHColor::MSHColor(const LABColor& color)
    {
        setFromLAB(color);
    }

    MSHColor::MSHColor(const MSHColor& copy)
    {
        *this = copy;
    }

    MSHColor::MSHColor(MSHColor&& mvt)
    {
        *this = mvt;
    }

    MSHColor& MSHColor::operator=(const MSHColor& copy)
    {
        if(this != &copy)
        {
            m = copy.m;
            s = copy.s;
            h = copy.h;
            a = copy.a;
        }
        return *this;
    }

    MSHColor MSHColor::operator+(const MSHColor& color) const
    {
        return MSHColor(m + color.m,
                        s + color.s,
                        h + color.h,
                        a + color.a);
    }

    MSHColor MSHColor::operator-(const MSHColor& color) const
    {
        return MSHColor(m - color.m,
                        s - color.s,
                        h - color.h,
                        a - color.a);
    }

    MSHColor MSHColor::operator*(float t) const
    {
        return MSHColor(m * t,
                        s * t,
                        h * t,
                        a * t);
    }

    void MSHColor::operator+=(const MSHColor& color)
    {
        *this = *this + color;
    }

    void MSHColor::operator-=(const MSHColor& color)
    {
        *this = *this - color;
    }

    void MSHColor::operator*=(float t)
    {
        *this = *this * t;
    }

    void MSHColor::setFromLAB(const LABColor& color)
    {
        m = (float)sqrt(color.l*color.l + color.a*color.a + color.b*color.b);
        s = (float)acos(color.l/m);
        h = (float)atan2(color.b, color.a);
        a = color.transparency;
    }

    LABColor MSHColor::toLAB() const
    {
        float l  = (float)(m * cos(s));
        float _a = (float)(m * sin(s) * cos(h));
        float b  = (float)(m * sin(s) * sin(h));
        return LABColor(l, _a, b, a);
    }

    void MSHColor::setFromXYZ(const XYZColor& xyz)
    {
        setFromLAB(LABColor(xyz));
    }

    void MSHColor::setFromRGB(const Color& color)
    {
        setFromXYZ(XYZColor(color));
    }

    XYZColor MSHColor::toXYZ() const
    {
        return toLAB().toXYZ();
    }

    Color MSHColor::toRGB() const
    {
        return toXYZ().toRGB();
    }

    MSHColor MSHColor::fromColorInterpolation(const Color& c1, const Color& c2, float interp)
    {
        MSHColor m1 = MSHColor(c1);
        MSHColor m2 = MSHColor(c2);

        float radDiff = fabs(m1.h - m2.h);

        if(m1.s > 0.05 && 
           m2.s > 0.05 && 
           radDiff > M_PI/3.0f)
        {
            float midM = (float)(fmax(fmax(m1.m, m2.m), 98));
            if(interp < 0.5f)
            {
                m2.m = midM;
                m2.s = 0;
                m2.h = 0;
                interp *= 2.0f;
            }
            else
            {
                m1.m = midM;
                m1.s = 0;
                m1.h = 0;
                interp = 2.0f*interp - 1.0f;
            }
        }

        if(m1.s < 0.05 && m2.s > 0.05)
            m1.h = adjustHue(m2, m1.m);
        else if(m1.s > 0.05 && m2.s < 0.05)
            m2.h = adjustHue(m1, m2.m);

        return m1*(1.0f-interp) + m2*(interp);
    }

    float MSHColor::adjustHue(const MSHColor& color, float m)
    {
        if(color.m >= m)
            return color.h;

        float hSpin = (float)(color.s * sqrt(m*m - color.m*color.m) / (color.m*sin(color.s)));
        if(hSpin > -M_PI/3.0)
            return color.h + hSpin;
        return color.h - hSpin;
    }
}
