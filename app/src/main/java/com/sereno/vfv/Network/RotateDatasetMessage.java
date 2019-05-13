package com.sereno.vfv.Network;

public class RotateDatasetMessage extends ServerMessage
{
    /** The datasetID of the message*/
    private int m_datasetID;

    /** The subdataset ID of the message*/
    private int m_subDatasetID;

    /** The headset ID performing this movement. -1 == server call*/
    private int m_headsetID;

    /** The new rotation to apply*/
    private float[] m_rotation = new float[4];

    /** Was the scaling done in the public view?*/
    private byte m_inPublic = 1;

    @Override
    public byte getCurrentType()
    {
        if(cursor <= 2)
            return 'I';
        else if(cursor == 3)
            return 'b';
        return 'f';
    }

    @Override
    public void pushValue(int val)
    {
        if(cursor == 0)
            m_datasetID = val;
        else if(cursor == 1)
            m_subDatasetID = val;
        else if(cursor == 2)
            m_headsetID = val;
        super.pushValue(val);
    }

    @Override
    public void pushValue(byte val)
    {
        m_inPublic = val;
        super.pushValue(val);
    }

    @Override
    public void pushValue(float val)
    {
        m_rotation[cursor-4] = val;
        super.pushValue(val);
    }

    @Override
    public int getMaxCursor()
    {
        return 7;
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

    /** Get the headset ID parsed.
     * @return the headset ID rotating this dataset. -1 == server call*/
    public int getHeadsetID() {return m_headsetID;}

    /** The rotation parsed. 0 -> w, 1 -> i, 2 -> j, 3 ->k
     * @return the rotation quaternion parsed*/
    public float[] getRotation()
    {
        return m_rotation;
    }

    /** Is this rotation made in public space?
     * @return true in public space, false otherwise*/
    public boolean doneIntoPublicSpace() {return m_inPublic != 0;}
}
