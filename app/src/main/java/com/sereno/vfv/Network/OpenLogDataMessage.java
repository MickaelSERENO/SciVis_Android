package com.sereno.vfv.Network;

public class OpenLogDataMessage extends ServerMessage
{
    private int     m_dataID;
    private String  m_path;
    private boolean m_hasHeader;
    private int     m_timeHeader;

    @Override
    public byte getCurrentType()
    {
        if(cursor == 0 || cursor == 3)
            return 'I';
        else if(cursor == 1)
            return 's';
        else if(cursor == 2)
            return 'b';
        return 0;
    }

    @Override
    public void pushValue(String value)
    {
        m_path = value;
        super.pushValue(value);
    }

    @Override
    public void pushValue(byte value)
    {
        if(cursor == 2)
            m_hasHeader = (value != 0);
        super.pushValue(value);
    }


    @Override
    public void pushValue(int value)
    {
        if(cursor == 0)
            m_dataID = value;
        else if(cursor == 3)
            m_timeHeader = value;
        super.pushValue(value);
    }

    @Override
    public int getMaxCursor()
    {
        return 3;
    }

    /** Get the data ID of the Log Data
     * @return the data ID saved by the server*/
    public int getDataID() {return m_dataID;}

    /** Get the path of the Data to open
     * @return the Log data path*/
    public String getPath() {return m_path;}

    /** Is there an header associated to this log data?
     * @return true if yes, false otherwise*/
    public boolean hasHeader() {return m_hasHeader;}

    /** Get which header ID (column ID) encodes time values
     * @return the column ID encoding time values. -1 == no time*/
    public int getTimeHeaderID() {return m_timeHeader;}
}