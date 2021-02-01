package com.sereno.vfv.Data.Annotation;

import com.sereno.color.Color;

public abstract class DrawableAnnotationLogComponent
{
    protected long m_ptr;

    /** Constructor
     *  @param ptr the native C++ std::shared_ptr<DrawableAnnotationLogComponent> ptr or derived*/
    protected DrawableAnnotationLogComponent(long ptr)
    {
        m_ptr = ptr;
    }

    @Override
    public void finalize()
    {

    }

    /** Get the native C++ std::shared_ptr<DrawableAnnotationLogComponent> ptr
     * @return the native C++ ptr. Derived class might use derived types for the C++ ptr*/
    public long getPtr()
    {
        return m_ptr;
    }

    public void setEnableTime(boolean t)
    {
        nativeSetEnableTime(m_ptr, t);
    }

    public boolean getEnableTime()
    {
        return nativeGetEnableTime(m_ptr);
    }

    public Color getColor()
    {
        float[] color = nativeGetColor(m_ptr);
        return new Color(color[0], color[1], color[2], color[3]);
    }

    public void setColor(Color c)
    {
        nativeSetColor(m_ptr, c.r, c.g, c.b, c.a);
    }

    private static native void    nativeSetEnableTime(long ptr, boolean t);
    private static native boolean nativeGetEnableTime(long ptr);
    private static native void    nativeSetColor(long ptr, float r, float g, float b, float a);
    private static native float[] nativeGetColor(long ptr);
}