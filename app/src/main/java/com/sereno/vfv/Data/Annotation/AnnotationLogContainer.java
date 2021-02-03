package com.sereno.vfv.Data.Annotation;

import java.util.ArrayList;

public class AnnotationLogContainer
{
    /** Interface for managing events from AnnotationLogContainer objects*/
    public interface IAnnotationLogContainerListener
    {
        /** Function called when an AnnotationPosition has been added
         * @param container the container calling this method
         * @param position the AnnotationPosition added to "container"*/
        void onAddAnnotationLogPosition(AnnotationLogContainer container, AnnotationPosition position);
    }

    private ArrayList<IAnnotationLogContainerListener> m_listeners = new ArrayList<>(); /**The listener to call on events*/

    private long                          m_ptr;  /**The native C++ pointer of a std::shared_ptr<sereno::AnnotationLogContainer>*/
    private String[]                      m_headers; /**The headers of the database. Can be null if no header was asked*/
    private ArrayList<AnnotationPosition> m_positions = new ArrayList<>(); /**The list of registered AnnotationPosition*/
    private boolean                       m_isValid = false; /**Is this object in a valid state? Can be invalid if, e.g., the input file is ill-formed*/
    private String                        m_path; /**The path of the internal database. Only CSV are handled for the moment*/
    private int                           m_id;  /** The object ID (network communication)*/

    /**Constructor. See "isValid" to know if the object is correctly constructed
     * @param id the annotation log container model ID (used for network communication)
     * @param path the file path of the database. Only CSV files are handled for the moment
     * @param hasHeaders has this database any headers (column name)?*/
    public AnnotationLogContainer(int id, String path, boolean hasHeaders)
    {
        m_id = id;
        m_ptr  = nativeInitPtr(hasHeaders);
        m_path = path;
        if(path.endsWith(".csv"))
        {
            m_isValid = nativeParseCSV(m_ptr, path);
        }
        if(m_isValid && hasHeaders)
            m_headers = nativeGetHeaders(m_ptr);
    }

    /** Register a listener to this object
     * @param listener the object to call when events are fired*/
    public void addListener(IAnnotationLogContainerListener listener)
    {
        if(!m_listeners.contains(listener))
            m_listeners.add(listener);
    }

    /** Do not call anymore a registered listener
     * @param listener The listener to remove*/
    public void removeListener(IAnnotationLogContainerListener listener)
    {
        if(m_listeners.contains(listener))
            m_listeners.remove(listener);
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

    /** Get the ID of this object (network communication)
     * @return The object model ID as defined by the server*/
    public int getID() {return m_id;}

    /** Init an AnnotationPosition object bound to this log
     * @param compID the component ID
     * @return the annotation position object bound to this log*/
    public AnnotationPosition initAnnotationPosition(int compID)
    {
        return new AnnotationPosition(this, nativeInitAnnotationPosition(m_ptr), compID);
    }

    /** Push an AnnotationPosition to this container. This shall update the remaining and consumed headers.
     * @param ann the annotation to push
     * @return true if success, false otherwise*/
    public boolean pushAnnotationPosition(AnnotationPosition ann)
    {
        if(nativePushAnnotationPosition(m_ptr, ann.getPtr()))
        {
            m_positions.add(ann);
            for(IAnnotationLogContainerListener l : m_listeners)
                l.onAddAnnotationLogPosition(this, ann);
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

    /** Get the annotation positions registered. While those objects can parse data, the "stored" data this container has may differ due to users' changes.
     * @return the list of registered annotation positions*/
    public ArrayList<AnnotationPosition> getAnnotationPositions()
    {
        return m_positions;
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