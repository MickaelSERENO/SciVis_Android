package com.sereno.vfv.Network;

public class AddCloudPointDatasetMessage extends ServerMessage
{
    /** The data ID associated to this dataset*/
    private int  m_dataID;

    /** The file path to read*/
    private String m_path;

    @Override
    public byte getCurrentType()
    {
        if(cursor == 0)
            return 'I';
        else if(cursor == 1)
            return 's';
        return 0;
    }

    @Override
    public void pushValue(int value)
    {
        if(cursor == 0)
            m_dataID = value;
        super.pushValue(value);
    }

    @Override
    public void pushValue(String value)
    {
        if(cursor == 1)
            m_path = value;
        super.pushValue(value);
    }


    @Override
    public int getMaxCursor()
    {
        return 1;
    }

    /** Get the data ID of the CloudPoint Dataset
     * @return the data ID saved by the server*/
    public int getDataID() {return m_dataID;}

    /** Get the path of the CloudPoint Dataset
     * @return the VTKDataset path*/
    public String getPath() {return m_path;}

}
