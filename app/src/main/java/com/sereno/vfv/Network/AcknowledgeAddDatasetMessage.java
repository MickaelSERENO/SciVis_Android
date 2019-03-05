package com.sereno.vfv.Network;

public class AcknowledgeAddDatasetMessage extends ServerMessage
{
    /**The ID of the dataset added*/
    private int m_id = -1;

    @Override
    byte getCurrentType()
    {
        return 'I';
    }

    void pushValue(int value)
    {
        m_id = value;
        super.pushValue(value);
    }

    @Override
    int getMaxCursor() {return 0;}

    /** Get the ID of the acknowledged dataset*/
    public int getID() {return m_id;}
}
