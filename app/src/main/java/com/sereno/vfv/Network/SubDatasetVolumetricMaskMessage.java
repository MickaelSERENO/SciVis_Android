package com.sereno.vfv.Network;

public class SubDatasetVolumetricMaskMessage extends ServerMessage
{
    /** The datasetID of the message*/
    private int m_datasetID;

    /** The subdataset ID of the message*/
    private int m_subDatasetID;

    /** The byte array containing the volumetric mask data. Each bit (and not byte) encodes one value.*/
    private byte[] m_mask;

    /** Is the volumetric mask enabled?*/
    private boolean m_enabled;

    @Override
    public byte getCurrentType()
    {
        if(cursor <= 1)
            return 'I';
        else if(cursor == 2)
            return 'a';
        else if(cursor == 3)
            return 'b';
        return 0;
    }

    @Override
    public void pushValue(int val)
    {
        if (cursor == 0)
            m_datasetID = val;
        else if (cursor == 1)
            m_subDatasetID = val;
        super.pushValue(val);
    }

    @Override
    public void pushValue(byte[] val)
    {
        if(cursor == 2)
            m_mask = val;
        super.pushValue(val);
    }

    @Override
    public void pushValue(byte val)
    {
        if(cursor == 3)
            m_enabled = (val != 0);
        super.pushValue(val);
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

    /** Get the volumetric spatial mask parsed
     * @return the byte array containing the volumetric mask data. Each bit (and not byte) encodes one value.*/
    public byte[] getMask()
    {
        return m_mask;
    }

    /** Is the volumetric mask enabled?
     * @return true if yes, false otherwise*/
    public boolean isEnabled() {return m_enabled;}
}
