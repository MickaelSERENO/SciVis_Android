package com.sereno.vfv.Data.Annotation;

import com.sereno.color.Color;

public class DrawableAnnotationPosition extends DrawableAnnotationLogComponent
{
    protected DrawableAnnotationPosition(AnnotationPosition pos)
    {
        super(nativeInitPtr(pos.getAnnotationLog().getPtr(), pos.getPtr()));
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

    private static native void    nativeSetColor(long ptr, float r, float g, float b, float a);
    private static native float[] nativeGetColor(long ptr);
    private static native long    nativeInitPtr(long containerPtr, long posPtr);
}