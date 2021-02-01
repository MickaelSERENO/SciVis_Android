package com.sereno.vfv.Data.Annotation;

import com.sereno.color.Color;

import java.util.ArrayList;

public class AnnotationLogComponent
{
    /** The Listener interface concerning general events on AnnotationLogComponent*/
    public interface AnnotationLogComponentListener
    {
        /** Method called when the headers of a component has changed
         * @param component the AnnotationLogComponent firing the event*/
        void onSetHeaders(AnnotationLogComponent component);
    }

    private ArrayList<AnnotationLogComponentListener> m_listeners = new ArrayList<>(); /**The listener to call on events*/

    protected AnnotationLogContainer m_annot;

    protected long m_ptr; /**The native C++ pointer pointing to a std::shared_ptr<sereno::AnnotationLogComponent> object (or an inherited class object)*/

    private int m_id; /**The ID of this component as set by the server*/

    /** Constructor, initialize this interface
     * @param ptr a std::shared_ptr<AnnotationLogComponent> c++ native pointer or similar (i.e., derivative class of AnnotationLogComponent)
     * @param compID the ID of this component as set by the server*/
    AnnotationLogComponent(AnnotationLogContainer container, long ptr, int compID)
    {
        m_annot = container;
        m_ptr = ptr;
        m_id  = compID;
    }

    /** Destructor, free the C++ native pointer*/
    public void finalize()
    {
        nativeDelPtr(m_ptr);
    }

    /** Register a listener to this object
     * @param listener the object to call when events are fired*/
    public void addListener(AnnotationLogComponentListener listener)
    {
        if(!m_listeners.contains(listener))
            m_listeners.add(listener);
    }

    /** Do not call anymore a registered listener
     * @param listener The listener to remove*/
    public void removeListener(AnnotationLogComponentListener listener)
    {
        if(m_listeners.contains(listener))
            m_listeners.remove(listener);
    }
    /** Get the headers that this component read on an AnnotationLogContainer
     * @param the header indexes consumed*/
    public int[] getHeaders() {return nativeGetHeaders(m_ptr);}

    /** Get thje native C++ pointer on a std::shared_ptr<sereno::AnnotationLogComponent>
     * @param the pointer in a long format*/
    public long getPtr()
    {
        return m_ptr;
    }

    /** Get the ID of this component as set by the server
     * @return the ID of this component. -1 if no ID*/
    public int getID()
    {
        return m_id;
    }

    /** Get the annotation log container from which this component is reading from
     * @return the AnnotationLogContainer linked to this component*/
    public AnnotationLogContainer getAnnotationLog()
    {
        return m_annot;
    }

    /** Function for inherited class only. Fire the "onSetHeaders" event*/
    protected void fireOnSetHeaders()
    {
        for(AnnotationLogComponentListener list : m_listeners)
            list.onSetHeaders(this);
    }

    private static native void    nativeDelPtr(long ptr);
    private static native int[]   nativeGetHeaders(long ptr);
}
