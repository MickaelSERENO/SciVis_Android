package com.sereno.vfv.Data.Annotation;

import com.sereno.color.Color;

public class AnnotationPosition extends AnnotationLogComponent
{
    AnnotationPosition(AnnotationLogContainer container, long ptr, int compID)
    {
        super(container, ptr, compID);
    }

    public AnnotationPosition(AnnotationLogContainer ann, int compID)
    {
        this(ann, nativeInitPtr(ann.getPtr()), compID);
    }

    public void setXYZHeader(String x, String y, String z)
    {
        nativeSetXYZHeaderString(m_ptr, x, y, z);
    }

    public void setXYZHeader(int x, int y, int z)
    {
        nativeSetXYZHeaderInteger(m_ptr, x, y, z);
    }

    private static native long    nativeInitPtr(long annLogPtr);
    private static native void    nativeSetXYZHeaderString(long ptr, String x, String y, String z);
    private static native void    nativeSetXYZHeaderInteger(long ptr, int x, int y, int z);
}