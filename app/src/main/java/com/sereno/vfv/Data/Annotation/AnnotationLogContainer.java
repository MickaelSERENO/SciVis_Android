package com.sereno.vfv.Data.Annotation;

import java.util.ArrayList;

public class AnnotationLogContainer
{
    private long                          m_ptr;
    private String[]                      m_headers;
    private ArrayList<AnnotationPosition> m_positions;
    private boolean                       m_isValid = false;
    private String                        m_path;

    public AnnotationLogContainer(String path, boolean hasHeaders)
    {
        m_ptr  = nativeInitPtr(hasHeaders);
        m_path = path;
        if(path.endsWith(".csv"))
        {
            m_isValid = nativeParseCSV(m_ptr, path);
        }
        if(m_isValid && hasHeaders)
            m_headers = nativeGetHeaders(m_ptr);
    }

    /** Set the header (column ID) corresponding to the time component
     * @param header the header column ID, ranging from 0 to getHeaders.length - 1*/
    public boolean setTimeHeader(int header)
    {
        return nativeSetTimeHeaderInt(m_ptr, header);
    }

    /** Set the header (column name, see hasHeaders() and getHeaders()
     * @param header the time header name. The header name must be a valid name, see getHeaders()*/
    public boolean setTimeHeader(String header)
    {
        return nativeSetTimeHeaderString(m_ptr, header);
    }

    /** Get the number of column this log contains
     * @return the number of columns. If the log is empty, returns 0*/
    public int getNbColumns()
    {
        return nativeGetNbColumns(m_ptr);
    }

    /** Get all the available headers for this log. See also hasHeaders()
     * @return all the log headers name*/
    public String[] getHeaders()
    {
        return m_headers;
    }

    /** Has this log headers?
     * @return true if yes, false otherwise*/
    public boolean hasHeaders()
    {
        return nativeHasHeaders(m_ptr);
    }

    /** Is this log container in a valid state?
     * @return true if yes, false otherwise*/
    public boolean isValid() {return m_isValid;}

    /** Get the headers remaining to use.
     * @return the column IDs that can still be linked to a component*/
    public int[] getRemainingHeaders()
    {
        return nativeGetRemainingHeaders(m_ptr);
    }

    /** Get the headers that are already assigned
     * @return the assigned column IDs to a variable (e.g., to 3D position information)*/
    public int[] getConsumedHeaders()
    {
        return nativeGetConsumedHeaders(m_ptr);
    }

    public void finalize()
    {
        nativeDelPtr(m_ptr);
    }

    /** Init an AnnotationPosition object bound to this log
     * @return the annotation position object bound to this log*/
    public AnnotationPosition initAnnotationPosition()
    {
        return new AnnotationPosition(nativeInitAnnotationPosition(m_ptr));
    }

    /** Push an AnnotationPosition to this container. This shall update the remaining and consumed headers.
     * @param ann the annotation to push
     * @return true if success, false otherwise*/
    public boolean pushAnnotationPosition(AnnotationPosition ann)
    {
        if(nativePushAnnotationPosition(m_ptr, ann.getPtr()))
        {
            m_positions.add(ann);
            return true;
        }
        return false;
    }

    /** Get the file path associated with this logged information
     * @return the file path containing the data */
    public String getFilePath()
    {
        return m_path;
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