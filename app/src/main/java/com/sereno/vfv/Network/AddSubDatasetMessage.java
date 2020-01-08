package com.sereno.vfv.Network;

public class AddSubDatasetMessage extends ServerMessage
{
    private int    m_datasetID = 0;  /** The Dataset to add this SubDataset*/
    private int    m_sdID      = 0;  /** The new SubDataset ID*/
    private String m_name;           /** The SubDataset name*/
    private int    m_ownerID   = -1; /** The owner ID*/

    @Override
    byte getCurrentType()
    {
        if(cursor <= 1)
            return 'I';
        else if(cursor == 2)
            return 's';
        else if(cursor == 3)
            return 'I';
        return 0;
    }

    @Override
    public int getMaxCursor()
    {
        return 3;
    }

    @Override
    public void pushValue(int value)
    {
        if(cursor == 0)
            m_datasetID = value;
        else if(cursor == 1)
            m_sdID = value;
        else if(cursor == 3)
            m_ownerID = value;
        super.pushValue(value);
    }

    @Override
    public void pushValue(String value)
    {
        if(cursor == 2)
            m_name = value;
        super.pushValue(value);
    }

    /** Get the DatasetID aimed at adding a new SubDataset
     * @return the DatasetID*/
    public int getDatasetID() {return m_datasetID;}

    /** Get the SubDatasetID to add
     * @return the SubDataset ID*/
    public int getSubDatasetID() {return m_sdID;}

    /** Get the Owner ID (-1 == public SubDataset)
     * @return the owner ID, -1 if no owner (i.e., public SubDataset)*/
    public int getOwnerID() {return m_ownerID;}

    /** Get the name of the SubDataset ID as defined
     * @return the name of the newly to-create SubDataset*/
    public String getSubDatasetName() {return m_name;}
}
