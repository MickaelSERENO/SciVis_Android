package com.sereno.vfv.Network;

public class RotateDatasetMessage extends ServerMessage
{
    /** The datasetID of the message*/
    private int m_datasetID;

    /** The subdataset ID of the message*/
    private int m_subDatasetID;

    /** The new rotation to apply*/
    private float[] m_rotation = new float[4];

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
        m_rotation[cursor-2] = val;
        super.pushValue(val);
    }

    @Override
    public int getMaxCursor()
    {
        return 5;
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

    /** The rotation parsed. 0 -> w, 1 -> i, 2 -> j, 3 ->k
     * @return the rotation quaternion parsed*/
    public float[] getRotation()
    {
        return m_rotation;
    }
}
