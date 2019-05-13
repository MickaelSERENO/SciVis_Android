package com.sereno.vfv.Network;

public class VisibilityMessage extends ServerMessage
{
    /** The dataset ID*/
    private int m_datasetID;

    /** The SubDataset ID*/
    private int m_subDatasetID;

    /** The visibility to apply to the subdataset*/
    private int m_visibility;

    @Override
    public byte getCurrentType()
    {
        return 'I';
    }

    @Override
    public int getMaxCursor()
    {
        return 2;
    }

    @Override
    public void pushValue(int value)
    {
        if(cursor == 0)
            m_datasetID = value;
        else if(cursor == 1)
            m_subDatasetID = value;
        else if(cursor == 2)
            m_visibility = value;
        super.pushValue(value);
    }

    /** Get the visibility to apply
     * @return the visibility to apply. See SubDataset.VISIBILITY_PUBLIC and SubDataset.VISIBILITY_PRIVATE for more details*/
    public int getVisibility() {return m_visibility;}

    /** Get the dataset ID to modify
     * @return the dataset ID*/
    public int getDatasetID() {return m_datasetID;}

    /** Get the subdataset ID to modify
     * @return the subdataset ID*/
    public int getSubDatasetID() {return m_subDatasetID;}
}
