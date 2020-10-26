package com.sereno.vfv.Network;

public class NextTBTrialMessage extends ServerMessage
{
    private int m_trialID;
    private int m_modeUserStudy;
    private int m_tangibleMode;

    @Override
    public void pushValue(int value)
    {
        if(cursor == 0)
            m_trialID = value;
        super.pushValue(value);
    }

    @Override
    public void pushValue(byte value)
    {
        if(cursor == 1)
            m_modeUserStudy = value;
        else if(cursor == 2)
            m_tangibleMode = value;
        super.pushValue(value);
    }

    @Override
    public byte getCurrentType()
    {
        if(cursor == 0)
            return 'I';
        else if(cursor <= 2)
            return 'b';
        return 0;
    }

    @Override
    public int getMaxCursor()
    {
        return 2;
    }

    /** Get the trial ID of this message
     * @return the trial ID*/
    public int getTrialID()
    {
        return m_trialID;
    }

    /** Get the visualization mode (either 2D or AR) or this trial
     * @return the visualization mode */
    public int getUserStudyMode()
    {
        return m_modeUserStudy;
    }

    /** Get the interaction mode (Absolute, relative-aligned, relative-full) to use for this trial
     * @return the interaction mode*/
    public int getTangibleMode()
    {
        return m_tangibleMode;
    }
}
