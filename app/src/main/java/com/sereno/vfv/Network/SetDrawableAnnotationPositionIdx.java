package com.sereno.vfv.Network;

public class SetDrawableAnnotationPositionIdx extends ServerMessage
{
    private int   m_datasetID;
    private int   m_sdID;
    private int   m_drawableID;
    private int[] m_idx = new int[0];

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
            m_idx = new int[value];
        else if(cursor < 4 + m_idx.length)
            m_idx[cursor-4] = value;
        super.pushValue(value);
    }

    @Override
    public byte getCurrentType()
    {
        return 'I';
    }

    @Override
    public int getMaxCursor()
    {
        return 3+m_idx.length;
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

    /** Get the indices to read from the log container attached to this drawable
     * @return the indice values*/
    public int[] getIndices()
    {
        return m_idx;
    }
}
