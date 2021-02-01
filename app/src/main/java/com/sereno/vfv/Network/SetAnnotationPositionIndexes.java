package com.sereno.vfv.Network;

public class SetAnnotationPositionIndexes extends ServerMessage
{
    /** The Annotation Log ID owning this Annotation Position*/
    private int m_annotID;

    /** This component ID inside the owner Annotation Log object*/
    private int m_compID;

    /** The new indexes to use*/
    private int[] m_indexes = new int[3];

    @Override
    public byte getCurrentType()
    {
        return 0;
    }

    @Override
    public int getMaxCursor()
    {
        return 4;
    }

    /** Get the Annotation Log ID owning this Annotation Position
     * @return the owner Annotation Log ID*/
    public int getAnnotID()
    {
        return m_annotID;
    }

    /** Get the Annotation Component ID inside the owner "getAnnotID" annotation log object
     * @return the component ID*/
    public int getComponentID()
    {
        return m_compID;
    }

    /** Get the new indexes to use
     * @return the new XYZ indexes to use*/
    public int[] getIndexes()
    {
        return m_indexes;
    }
}