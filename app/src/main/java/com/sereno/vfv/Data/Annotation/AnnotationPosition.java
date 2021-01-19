package com.sereno.vfv.Data.Annotation;

import com.sereno.color.Color;

public class AnnotationPosition extends AnnotationLogComponent
{
    AnnotationPosition(long ptr)
    {
        super(ptr);
    }

    public AnnotationPosition(AnnotationLogContainer ann)
    {
        this(nativeInitPtr(ann.getPtr()));
    }

    public void setXYZHeader(String x, String y, String z)
    {
        nativeSetXYZHeader_String(m_ptr, x, y, z);
    }

    public void setXYZHeader(int x, int y, int z)
    {
        nativeSetXYZHeader_Integer(m_ptr, x, y, z);
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

    private static native long    nativeInitPtr(long annLogPtr);
    private static native void    nativeSetColor(long ptr, float r, float g, float b, float a);
    private static native float[] nativeGetColor(long ptr);
    private static native void    nativeSetXYZHeader_String(long ptr, String x, String y, String z);
    private static native void    nativeSetXYZHeader_Integer(long ptr, int x, int y, int z);
}