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
    public void pushValue(int value)
    {
        if(cursor == 0)
            m_annotID = value;
        else if(cursor == 1)
            m_compID = value;
        else if(cursor <= 4)
            m_indexes[cursor-2] = value;
        super.pushValue(value);
    }

    @Override
    public byte getCurrentType()
    {
        if(cursor <= 4)
            return 'I';
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