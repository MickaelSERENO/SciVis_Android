package com.sereno.vfv.Network;

public class RemoveSubDatasetGroupMessage extends ServerMessage
{
    private int m_sdgID;

    @Override
    public void pushValue(int value)
    {
        if(cursor == 0)
            m_sdgID = value;
        super.pushValue(value);
    }

    @Override
    public byte getCurrentType()
    {
        if(cursor == 0)
            return 'I';
        return 0;
    }

    @Override
    public int getMaxCursor(){return 0;}

    public int getSubDatasetGroupID() {return m_sdgID;}

}
