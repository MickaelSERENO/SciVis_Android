package com.sereno.vfv.Data;

/** Point Field Description bound to a particular Dataset. It contains metadata of a Dataset point field*/
public class PointFieldDesc
{
    /** The point field ID*/
    private int   m_id;

    /** The minimum value of the point field ID*/
    private float m_min;

    /** The maximum value of the point field ID*/
    private float m_max;

    /** Is the point field loaded?*/
    private boolean m_loaded;

    /** The point field descriptor constructor
     * @param id the ID of the point field
     * @param min the minimum value of the point field.
     * @param max the maximum value of the point field
     * @param loaded is the point field loaded in memory? If false, min and max are not necessary (can be whatever)*/
    public PointFieldDesc(int id, float min, float max, boolean loaded)
    {
        m_id     = id;
        m_min    = min;
        m_max    = max;
        m_loaded = loaded;
    }

    /** Get the ID of this point field descriptor as defined by the bound Dataset
     * @return the point field descriptor ID*/
    public int getID()
    {
        return m_id;
    }

    /** Get the Min value possessed in this point field descriptor. This function is meaningful only if isLoaded() == true
     * @return the min value of all the point field values*/
    public float getMin()
    {
        return m_min;
    }

    /** Get the Max value possessed in this point field descriptor. This function is meaningful only if isLoaded() == true
     * @return the max value of all the point field values*/
    public float getMax()
    {
        return m_max;
    }

    /** Tells whether this point field is loaded in memory or not. If false, min and max values are meaningless
     * @return true if loaded in memory, false otherwise*/
    public boolean isLoaded()
    {
        return m_loaded;
    }
}
