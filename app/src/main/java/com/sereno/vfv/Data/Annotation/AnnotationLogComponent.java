package com.sereno.vfv.Data.Annotation;

import com.sereno.color.Color;

public class AnnotationLogComponent
{
    protected long m_ptr;

    /** Constructor, initialize this interface
     * @param ptr a std::shared_ptr<AnnotationLogComponent> c++ native pointer or similar (i.e., derivative class of AnnotationLogComponent)*/
    AnnotationLogComponent(long ptr)
    {
        m_ptr = ptr;
    }

    /** Destructor, free the C++ native pointer*/
    public void finalize()
    {
        nativeDelPtr(m_ptr);
    }

    public void setEnableTime(boolean t)
    {
        nativeSetEnableTime(m_ptr, t);
    }

    public boolean getEnableTime()
    {
        return nativeGetEnableTime(m_ptr);
    }

    long getPtr()
    {
        return m_ptr;
    }

    private static native void    nativeDelPtr(long ptr);
    private static native void    nativeSetEnableTime(long ptr, boolean t);
    private static native boolean nativeGetEnableTime(long ptr);
}
