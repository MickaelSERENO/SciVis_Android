package com.sereno.vfv.Network;

/** Clear Annotations Message, ask to clear all the annotations of a particular subdataset*/
public class ClearAnnotationsMessage extends ServerMessage
{
    private int m_datasetID;
    private int m_sdID;

    @Override
    public void pushValue(int value)
    {
        if(cursor == 0)
            m_datasetID = value;
        else if(cursor == 1)
            m_sdID = value;
        super.pushValue(value);
    }

    @Override
    public byte getCurrentType()
    {
        if(cursor <= 1)
            return 'I';
        return 0;
    }

    @Override
    public int getMaxCursor()
    {
        return 1;
    }

    /** The dataset ID to remove the annotations*/
    public int getDatasetID()
    {
        return m_datasetID;
    }

    /** The subdataset ID to remove the annotations. The targeted dataset is then dataset[getDatasetID()].getSubDatasets()[getSubDatasetID()]*/
    public int getSubDatasetID()
    {
        return m_sdID;
    }
}
