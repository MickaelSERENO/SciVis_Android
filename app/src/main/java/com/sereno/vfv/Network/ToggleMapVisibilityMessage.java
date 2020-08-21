package com.sereno.vfv.Network;

public class ToggleMapVisibilityMessage extends ServerMessage
{
    /** The dataset ID to apply the map visibility to*/
    private int m_datasetID = 0;

    /** The subdataset ID to apply the map visibility to*/
    private int m_sdID      = 0;

    /** The map visibility to apply*/
    private boolean m_visibility = false;

    @Override
    void pushValue(int val)
    {
        if(cursor == 0)
            m_datasetID = val;
        else if(cursor == 1)
            m_sdID = val;
        super.pushValue(val);
    }

    @Override
    void pushValue(byte val)
    {
        if(cursor == 2)
            m_visibility = (val != 0);
        super.pushValue(val);
    }

    @Override
    byte getCurrentType()
    {
        if(cursor <= 1)
            return 'I';
        else if(cursor == 2)
            return 'b';
        return 0;
    }

    @Override
    int getMaxCursor()
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
     * @return the subdataset ID to update. Update dataset ID = getDatasetID().getSubDataset[getSubDatasetID()].*/
    public int getSubDatasetID()
    {
        return m_sdID;
    }

    /** Get the map visibility to apply
     * @return the map visibility to apply*/
    public boolean getVisibility() {return m_visibility;}
}
