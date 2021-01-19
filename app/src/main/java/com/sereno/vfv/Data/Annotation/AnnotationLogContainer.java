package com.sereno.vfv.Data.Annotation;

import java.util.ArrayList;

public class AnnotationLogContainer
{
    private long                          m_ptr;
    private String[]                      m_headers;
    private ArrayList<AnnotationPosition> m_positions;
    private boolean                       m_isValid = false;

    public AnnotationLogContainer(String path, boolean hasHeaders)
    {
        m_ptr = nativeInitPtr(hasHeaders);
        if(path.endsWith(".csv"))
        {
            if(nativeParseCSV(m_ptr, path));
                m_isValid = true;
        }
        if(m_isValid)
            m_headers = nativeGetHeaders(m_ptr);
    }

    public void setTimeHeader(int header)
    {
        nativeSetTimeHeaderInt(m_ptr, header);
    }

    public void setTimeHeader(String header)
    {
        nativeSetTimeHeaderString(m_ptr, header);
    }

    public int getNbColumns()
    {
        return nativeGetNbColumns(m_ptr);
    }

    public String[] getHeaders()
    {
        return m_headers;
    }

    public boolean hasHeaders()
    {
        return nativeHasHeaders(m_ptr);
    }

    public int[] getRemainingHeaders()
    {
        return nativeGetRemainingHeaders(m_ptr);
    }

    public int[] getConsumedHeaders()
    {
        return nativeGetConsumedHeaders(m_ptr);
    }

    public void finalize()
    {
        nativeDelPtr(m_ptr);
    }

    public AnnotationPosition initAnnotationPosition()
    {
        return new AnnotationPosition(nativeInitAnnotationPosition(m_ptr));
    }

    public boolean pushAnnotationPosition(AnnotationPosition ann)
    {
        if(nativePushAnnotationPosition(m_ptr, ann.getPtr()))
        {
            m_positions.add(ann);
            return true;
        }
        return false;
    }

    long getPtr()
    {
        return m_ptr;
    }

    private static native long     nativeInitPtr(boolean hasHeaders);
    private static native void     nativeDelPtr(long ptr);
    private static native boolean  nativeParseCSV(long ptr, String path);
    private static native String[] nativeGetHeaders(long ptr);
    private static native boolean  nativeSetTimeHeaderInt(long ptr, int header);
    private static native boolean  nativeSetTimeHeaderString(long ptr, String header);
    private static native int      nativeGetNbColumns(long ptr);
    private static native boolean  nativeHasHeaders(long ptr);
    private static native int[]    nativeGetRemainingHeaders(long ptr);
    private static native int[]    nativeGetConsumedHeaders(long ptr);
    private static native long     nativeInitAnnotationPosition(long ptr);
    private static native boolean  nativePushAnnotationPosition(long ptr, long posPtr);
}