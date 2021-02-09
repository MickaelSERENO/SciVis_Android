package com.sereno.vfv.Network;

public class AddAnnotationPositionToSDMessage extends ServerMessage
{
    private int m_datasetID;
    private int m_sdID;
    private int m_annotID;
    private int m_compID;
    private int m_drawableID;

    @Override
    public byte getCurrentType()
    {
        return 'I';
    }

    @Override
    public void pushValue(int value)
    {
        if(cursor == 0)
            m_datasetID = value;
        else if(cursor == 1)
            m_sdID = value;
        else if(cursor == 2)
            m_annotID = value;
        else if(cursor == 3)
            m_compID = value;
        else if(cursor == 4)
            m_drawableID = value;
        super.pushValue(value);
    }

    @Override
    public int getMaxCursor()
    {
        return 4;
    }

    public int getDatasetID() {return  m_datasetID;}
    public int getSubDatasetID() {return m_sdID;}
    public int getAnnotID() {return m_annotID;}
    public int getComponentID() {return m_compID;}
    public int getDrawableID() {return m_drawableID;}
}
