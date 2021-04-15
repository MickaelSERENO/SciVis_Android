package com.sereno.vfv.Network;

public class AddSubjectiveViewGroupMessage extends ServerMessage
{
    private int m_subjectiveViewType;
    private int m_baseDatasetID;
    private int m_baseSubDatasetID;
    private int m_sdgID;

    public void pushValue(int value)
    {
        if(cursor == 1)
            m_baseDatasetID = value;
        else if(cursor == 2)
            m_baseSubDatasetID = value;
        else if(cursor == 3)
            m_sdgID = value;
        super.pushValue(value);
    }

    @Override
    public void pushValue(byte value)
    {
        m_subjectiveViewType = value;
        super.pushValue(value);
    }

    @Override
    public byte getCurrentType()
    {
        if(cursor == 0)
            return 'b';
        else if(cursor < 4)
            return 'I';
        return 0;
    }

    @Override
    public int getMaxCursor()
    {
        return 3;
    }

    public int getBaseDatasetID() {return m_baseDatasetID;}
    public int getBaseSubDatasetID() {return m_baseSubDatasetID;}
    public int getSubDatasetGroupID() {return m_sdgID;}
    public int getSubjectiveViewType() {return m_subjectiveViewType;}
}