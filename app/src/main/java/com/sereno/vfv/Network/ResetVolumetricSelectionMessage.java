package com.sereno.vfv.Network;

public class ResetVolumetricSelectionMessage extends ServerMessage
{
    /** The datasetID of the message*/
    private int m_datasetID;

    /** The subdataset ID of the message*/
    private int m_subDatasetID;

    /** The headset ID of the message*/
    private int m_headsetID;

    @Override
    public byte getCurrentType()
    {
        if(cursor <= 2)
            return 'I';
        return 0;
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

    /** Get the headset ID asking to reset the volumetric selection
     * @return the headset ID*/
    public int getHeadsetID() {return m_headsetID;}
}
