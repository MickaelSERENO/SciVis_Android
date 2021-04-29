package com.sereno.vfv.Network;

public class RenameSubDatasetMessage extends ServerMessage
{
    /** The datasetID of the message*/
    private int m_datasetID;

    /** The subdataset ID of the message*/
    private int m_subDatasetID;

    /** The name to apply */
    private String m_name;

    @Override
    public void pushValue(int value)
    {
        if(cursor == 0)
            m_datasetID = value;
        else if(cursor == 1)
            m_subDatasetID = value;
        super.pushValue(value);
    }

    @Override
    public void pushValue(String value)
    {
        if(cursor == 2)
            m_name = value;
        super.pushValue(value);
    }

    @Override
    public byte getCurrentType()
    {
        if(cursor <= 1)
            return 'I';
        else if(cursor == 2)
            return 's';
        return 0;
    }

    @Override
    public int getMaxCursor()
    {
        return 2;
    }

    /** Get the DatasetID aimed at adding a new SubDataset
     * @return the DatasetID*/
    public int getDatasetID() {return m_datasetID;}

    /** Get the SubDatasetID to add
     * @return the SubDataset ID*/
    public int getSubDatasetID() {return m_subDatasetID;}

    /** Get the name of the SubDataset ID as defined
     * @return the name of the newly to-create SubDataset*/
    public String getSubDatasetName() {return m_name;}
}
