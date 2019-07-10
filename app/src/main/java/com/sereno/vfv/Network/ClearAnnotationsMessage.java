package com.sereno.vfv.Network;

/** Clear Annotations Message, ask to clear all the annotations of a particular subdataset*/
public class ClearAnnotationsMessage extends ServerMessage
{
    private int     m_datasetID = -1;
    private int     m_sdID      = -1;
    private boolean m_inPublic  = true;

    @Override
    public void pushValue(int value)
    {
        if(cursor == 0)
            m_datasetID = value;
        else if(cursor == 1)
            m_sdID = value;
        super.pushValue(value);
    }

    public void pushValue(byte value)
    {
        if(cursor == 2)
            m_inPublic = value != 0;
        super.pushValue(value);
    }

    @Override
    public byte getCurrentType()
    {
        if(cursor <= 1)
            return 'I';
        else if(cursor == 2)
            return 'b';
        return 0;
    }

    @Override
    public int getMaxCursor()
    {
        return 2;
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

    /** Is this command for the public space?*/
    public boolean doneIntoPublicSpace() { return m_inPublic; }
}
