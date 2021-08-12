package com.sereno.vfv.Network;

public class SetSubDatasetClippingMessage extends ServerMessage
{
    /** The datasetID of the message*/
    private int m_datasetID;

    /** The subdataset ID of the message*/
    private int m_subDatasetID;

    /** The min depth clipping value*/
    private float m_minDepthClipping;

    /** The max depth clipping value*/
    private float m_maxDepthClipping;

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
            m_minDepthClipping = v;
        else if(cursor == 3)
            m_maxDepthClipping = v;
        super.pushValue(v);
    }

    @Override
    public byte getCurrentType()
    {
        if(cursor <= 1)
            return 'I';
        else if(cursor <= 3)
            return 'f';
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
        return m_subDatasetID;
    }

    /** Get the max Depth clipping parsed
     * @return the max depth clipping. value should be clamped between 0.0f (total clipping) to 1.0f (no clipping applied)*/
    public float getMaxDepthClipping() {return m_maxDepthClipping;}

    /** Get the min Depth clipping parsed
     * @return the min depth clipping. value should be clamped between 0.0f (total clipping) to 1.0f (no clipping applied)*/
    public float getMinDepthClipping() {return m_minDepthClipping;}
}
