package com.sereno.vfv.Network;

public class AddSubDatasetToSubjectiveViewStackedGroupMessage extends ServerMessage
{
    private int m_sdgID;
    private int m_datasetID;
    private int m_stackedID;
    private int m_linkedID;

    @Override
    public void pushValue(int value)
    {
        if(cursor == 0)
            m_sdgID = value;
        else if(cursor == 1)
            m_datasetID = value;
        else if(cursor == 2)
            m_stackedID = value;
        else if(cursor == 3)
            m_linkedID = value;

        super.pushValue(value);
    }

    @Override
    public byte getCurrentType()
    {
        if(cursor < 4)
            return 'I';
        return 0;
    }

    @Override
    public int getMaxCursor()
    {
        return 3;
    }

    public int getSubDatasetGroupID() {return m_sdgID;}
    public int getDatasetID() {return m_datasetID;}
    public int getStackedSubDatasetID() {return m_stackedID;}
    public int getLinkedSubDatasetID() {return m_linkedID;}
}