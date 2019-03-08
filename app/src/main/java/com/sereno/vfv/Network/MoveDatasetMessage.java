package com.sereno.vfv.Network;

public class MoveDatasetMessage extends ServerMessage
{
    /** The datasetID of the message*/
    private int m_datasetID;

    /** The subdataset ID of the message*/
    private int m_subDatasetID;

    /** The new rotation to apply*/
    private float[] m_position = new float[3];

    @Override
    public byte getCurrentType()
    {
        if(cursor <= 1)
            return 'I';
        return 'f';
    }

    @Override
    public void pushValue(int val)
    {
        if(cursor == 0)
            m_datasetID = val;
        else if(cursor == 1)
            m_subDatasetID = val;
        super.pushValue(val);
    }

    @Override
    public void pushValue(float val)
    {
        m_position[cursor-2] = val;
        super.pushValue(val);
    }

    @Override
    public int getMaxCursor()
    {
        return 4;
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
        return m_subDatasetID;
    }

    /** The position parsed.
     * @return the Vector3 position parsed*/
    public float[] getPosition()
    {
        return m_position;
    }
}
