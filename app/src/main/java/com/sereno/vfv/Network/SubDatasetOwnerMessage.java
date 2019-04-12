package com.sereno.vfv.Network;

public class SubDatasetOwnerMessage extends ServerMessage
{
    /** The dataset ID*/
    private int m_datasetID = 0;

    /** The subdataset ID*/
    private int m_sdID      = 0;

    /** The headset ID owning this subdataset*/
    private int m_headsetID = 0;
    @Override
    public void pushValue(int v)
    {
        if(cursor == 0)
            m_datasetID = v;
        else if(cursor == 1)
            m_sdID = v;
        else
            m_headsetID = v;
    }

    @Override
    public int getMaxCursor()
    {
        return 2;
    }

    @Override
    public byte getCurrentType()
    {
        if(cursor <= 2)
            return 'I';
        return 0;
    }

    /** Get the dataset ID
     * @return the dataset ID of this new link*/
    public int getDatasetID()
    {
        return m_datasetID;
    }

    /** Get the subdataset ID
     * @return the subdataset ID of this new link*/
    public int getSubDatasetID()
    {
        return m_sdID;
    }

    /** Get the headset ID owning this subdataset
     * @return the headset ID owning this subdataset*/
    public int getHeadsetID()
    {
        return m_headsetID;
    }
}
