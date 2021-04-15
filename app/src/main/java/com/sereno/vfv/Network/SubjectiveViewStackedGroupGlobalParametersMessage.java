package com.sereno.vfv.Network;

public class SubjectiveViewStackedGroupGlobalParametersMessage extends ServerMessage
{
    private int     m_sdgID;
    private int     m_stackedMethod;
    private float   m_gap;
    private boolean m_isMerged;

    @Override
    public void pushValue(int value)
    {
        if(cursor == 0)
            m_sdgID = value;
        else if(cursor == 1)
            m_stackedMethod = value;

        super.pushValue(value);
    }

    @Override
    public void pushValue(float value)
    {
        if(cursor == 2)
            m_gap = value;
        super.pushValue(value);
    }

    @Override
    public void pushValue(byte value)
    {
        if(cursor == 3)
            m_isMerged = (value != 0);
        super.pushValue(value);
    }

    @Override
    public byte getCurrentType()
    {
        if(cursor < 2)
            return 'I';
        else if(cursor == 2)
            return 'f';
        else if(cursor == 3)
            return 'b';
        return 0;
    }

    @Override
    public int getMaxCursor()
    {
        return 3;
    }

    public int getSubDatasetGroupID() {return m_sdgID;}
    public int getStackedMethod() {return m_stackedMethod;}
    public float getGap() {return m_gap;}
    public boolean isMerged() {return m_isMerged;}
}