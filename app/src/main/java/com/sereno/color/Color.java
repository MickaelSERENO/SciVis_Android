package com.sereno.color;

public class Color
{
    public float r; /**!< Red component [0, 1]*/
    public float g; /**!< Green component [0, 1]*/
    public float b; /**!< Blue component [0, 1]*/
    public float a; /**!< Alpha component [0, 1]*/

    /** \brief Create a color
     * red, green, blue and alpha must to be between 0.0f and 1.0f
     * @param _r red component
     * @param _g green component
     * @param _b blue component
     * @param _a alpha component*/
    public Color(float _r, float _g, float _b, float _a)
    {
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }

    /** Get a Int32 ARGB 8888 color
     * @return the color into a int format*/
    public int toARGB8888()
    {
        return ((int)(255*a) << 24) +
               ((int)(255*r) << 16) +
               ((int)(255*g) << 8)  +
               ((int)(255*b));
    }

    @Override
    public Object clone()
    {
        return new Color(r, g, b, a);
    }

    static final Color WHITE   = new Color(1.0f, 1.0f, 1.0f, 1.0f);
    static final Color BLACK   = new Color(0.0f, 0.0f, 0.0f, 0.0f);
    static final Color RED     = new Color(1.0f, 0.0f, 0.0f, 1.0f);
    static final Color GREEN   = new Color(0.0f, 1.0f, 0.0f, 1.0f);
    static final Color BLUE    = new Color(0.0f, 0.0f, 1.0f, 1.0f);
    static final Color YELLOW  = new Color(1.0f, 1.0f, 0.0f, 1.0f);
    static final Color CYAN    = new Color(0.0f, 1.0f, 1.0f, 1.0f);
    static final Color MAGENTA = new Color(1.0f, 0.0f, 1.0f, 1.0f);

}
