package com.sereno.vfv.Data.Annotation;

import com.sereno.color.Color;
import com.sereno.vfv.Data.SubDataset;

import java.util.ArrayList;

/** Instance of this class are drawable objects that can be drawn based on the data of an AnnotationLogContainer*/
public class DrawableAnnotationPosition extends DrawableAnnotationLogComponent
{
    /** The listener events bound to the DrawableAnnotationPosition*/
    public interface IDrawableAnnotationPositionListener
    {
        /** Called when the color of 'l' has been changed
         * @param l the object calling this event
         * @param c the new color to apply*/
        void onSetColor(DrawableAnnotationPosition l, Color c);

        /** Called when the mapped data indices of 'l' has been changed
         * @param l the object calling this event
         * @param idx the new indices to apply. Can be empty, i.e., no data indices to use*/
        void onSetMappedDataIndices(DrawableAnnotationPosition l, int[] idx);
    }

    /** The annotation position data giving positions and other related data*/
    private AnnotationPosition m_data = null;

    /**The listener to call on events*/
    private ArrayList<IDrawableAnnotationPositionListener> m_listeners = new ArrayList<>();

    /** Constructor
     * @param pos the internal data to read from
     * @param sd the subdataset owning this drawable annotation log component
     * @param id the id of this Drawable, as defined by the server*/
    public DrawableAnnotationPosition(AnnotationPosition pos, SubDataset sd, int id)
    {
        super(nativeInitPtr(pos.getAnnotationLog().getPtr(), pos.getPtr()), sd, id);
        m_data = pos;
    }

    /** Register a listener to this object
     * @param listener the object to call when events are fired*/
    public void addListener(IDrawableAnnotationPositionListener listener)
    {
        if(!m_listeners.contains(listener))
            m_listeners.add(listener);
    }

    /** Do not call anymore a registered listener
     * @param listener The listener to remove*/
    public void removeListener(IDrawableAnnotationPositionListener listener)
    {
        if(m_listeners.contains(listener))
            m_listeners.remove(listener);
    }

    /** Get the default color to use to render this annotation position
     * @return the default RGBA color*/
    public Color getColor()
    {
        float[] color = nativeGetColor(m_ptr);
        return new Color(color[0], color[1], color[2], color[3]);
    }

    /** Set the default color to use to render this annotation position
     * @param c the new default RGBA color to use*/
    public void setColor(Color c)
    {
        if(c.equals(getColor()))
            return;

        nativeSetColor(m_ptr, c.r, c.g, c.b, c.a);
        for(IDrawableAnnotationPositionListener l : m_listeners)
            l.onSetColor(this, c);
    }

    /** Get the indices that are used for graphical contents. Indices represent columns inside the AnnotationLogCOntainer linked with this drawable
     * @return the ordered list of the indices to use. These might be interesting to use with the transfer function of the linked SubDataset
     * If empty, this object is not mapped to any column inside the bound AnnotationLogContainer*/
    public int[] getMappedDataIndices()
    {
        return nativeGetMappedDataIndices(m_ptr);
    }

    /** Set the indices that are used for graphical contents. Indices represent columns inside the AnnotationLogCOntainer linked with this drawable
     * @param idx the ordered list of the indices to use. These might be interesting to use with the transfer function of the linked SubDataset. If empty, this object is not mapped to any column inside the bound AnnotationLogContainer*/
    public void setMappedDataIndices(int[] idx)
    {
        if(idx == null)
            idx = new int[0];

        if(idx.equals(getMappedDataIndices()))
            return;

        nativeSetMappedDataIndices(m_ptr, idx);
        for(IDrawableAnnotationPositionListener l : m_listeners)
            l.onSetMappedDataIndices(this, idx);
    }

    public AnnotationPosition getData() {return m_data;}

    private static native void    nativeSetColor(long ptr, float r, float g, float b, float a);
    private static native float[] nativeGetColor(long ptr);
    private static native int[]   nativeGetMappedDataIndices(long ptr);
    private static native void    nativeSetMappedDataIndices(long ptr, int[] idx);
    private static native long    nativeInitPtr(long containerPtr, long posPtr);
}