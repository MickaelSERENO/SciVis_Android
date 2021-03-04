package com.sereno.vfv.Network;

public class SetDrawableAnnotationPositionColor extends ServerMessage
{
    private int m_datasetID;
    private int m_sdID;
    private int m_drawableID;
    private int m_color;

    @Override
    public void pushValue(int value)
    {
        if(cursor == 0)
            m_datasetID = value;
        else if(cursor == 1)
            m_sdID = value;
        else if(cursor == 2)
            m_drawableID = value;
        else if(cursor == 3)
            m_color = value;
        super.pushValue(value);
    }

    @Override
    public byte getCurrentType()
    {
        if(cursor <= 3)
            return 'I';
        return 0;
    }

    @Override
    public int getMaxCursor()
    {
        return 3;
    }

    /** Get the dataset ID parsed
     * @return the dataset ID to update. Need to be used with getSubDatasetID()*/
    public int getDatasetID()
    {
        return m_datasetID;
    }

    /** Get the subdataset ID parsed
     * @return the subdataset ID to update. Update dataset ID = getDatasetID() .getSubDataset[getSubDatasetID()].*/
    public int getSubDatasetID()
    {
        return m_sdID;
    }

    /** Get the drawable ID of the drawable inside the subdataset
     * @return the drawable ID to update*/
    public int getDrawableID()
    {
        return m_drawableID;
    }

    /** Get the default color values to use
     * @return the ARGB8888 color values*/
    public int getColor()
    {
        return m_color;
    }
}
