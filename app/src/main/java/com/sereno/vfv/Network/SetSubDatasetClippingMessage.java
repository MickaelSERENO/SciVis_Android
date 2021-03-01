package com.sereno.vfv.Network;

public class SetSubDatasetClippingMessage extends ServerMessage
{
    /** The datasetID of the message*/
    private int m_datasetID;

    /** The subdataset ID of the message*/
    private int m_subDatasetID;

    /** The depth clipping value*/
    private float m_depthClipping;

    @Override
    public void pushValue(int v)
    {
        if(cursor == 0)
            m_datasetID = v;
        else if(cursor == 1)
            m_subDatasetID = v;

        super.pushValue(v);
    }

    @Override
    public void pushValue(float v)
    {
        if(cursor == 2)
            m_depthClipping = v;
        super.pushValue(v);
    }

    @Override
    public byte getCurrentType()
    {
        if(cursor <= 1)
            return 'I';
        else if(cursor == 2)
            return 'f';
        return 0;
    }

    @Override
    public int getMaxCursor()
    {
        return 2;
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

    /** Get the depth clipping parsed
     * @return the depth clipping. value should be clamped between 0.0f (total clipping) to 1.0f (no clipping applied)*/
    public float getDepthClipping() {return m_depthClipping;}
}
