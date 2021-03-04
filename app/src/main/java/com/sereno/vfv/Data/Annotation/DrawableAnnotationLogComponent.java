package com.sereno.vfv.Data.Annotation;

import com.sereno.vfv.Data.SubDataset;

/**Abstract base class for Drawable annotation component. Contains graphical information to render log annotation components.*/
public abstract class DrawableAnnotationLogComponent
{
    /** The native C++ pointer of a std::shared_ptr<DrawableAnnotationLogComponent> linked to this java object*/
    protected long m_ptr;

    /** The id of this object, inside the subdataset, as defined by the server*/
    protected int m_id;

    /** The subdataset linked to this object*/
    protected SubDataset m_sd;

    /** Constructor
     * @param ptr the native C++ std::shared_ptr<DrawableAnnotationLogComponent> ptr or derived
     * @param sd the subdataset owning this drawable annotation log component
     * @param id the ID as defined by the server*/
    protected DrawableAnnotationLogComponent(long ptr, SubDataset sd, int id)
    {
        m_ptr = ptr;
        m_sd  = sd;
        m_id  = id;
    }

    @Override
    public void finalize() {}

    /** Get the native C++ std::shared_ptr<DrawableAnnotationLogComponent> ptr
     * @return the native C++ ptr. Derived class might use derived types for the C++ ptr*/
    public long getPtr()
    {
        return m_ptr;
    }

    /** Get the ID of this object as defined by the server
     * @return the ID of this object INSIDE the corresponding subdataset*/
    public int getID() {return m_id;}

    /** Set whether or not this component should try to consider time values
     * @param t true if this component should try to consider time values, false otherwise*/
    public void setEnableTime(boolean t)
    {
        nativeSetEnableTime(m_ptr, t);
    }

    /** Get whether or not this component should try to consider time values
     * @return true if yes, false otherwise*/
    public boolean getEnableTime()
    {
        return nativeGetEnableTime(m_ptr);
    }

    /** Get whether or not the time component is used. This is different than "getEnableTime" as this function also considers the parent containers time settings
     * @return true if yes, false otherwise*/
    public boolean IsTimeUsed()
    {
        return nativeIsTimeUsed(m_ptr);
    }

    public SubDataset getSubDataset() {return m_sd;}

    private static native void    nativeSetEnableTime(long ptr, boolean t);
    private static native boolean nativeGetEnableTime(long ptr);
    private static native boolean nativeIsTimeUsed(long ptr);
}