package com.sereno.vfv.Network;

public class NextTBTrialMessage extends ServerMessage
{
    private int m_tangibleMode;
    private int m_trialID;
    private int m_subTrialID;
    private boolean m_inTraining;

    @Override
    public void pushValue(int value)
    {
        if(cursor == 0)
            m_tangibleMode = value;
        else if(cursor == 1)
            m_trialID = value;
        else if(cursor == 2)
            m_subTrialID = value;

        super.pushValue(value);
    }

    @Override
    public void pushValue(byte value)
    {
        if(cursor == 3)
            m_inTraining = !(value == 0);
        super.pushValue(value);
    }

    @Override
    public byte getCurrentType()
    {
        if(cursor == 0 || cursor == 1 || cursor == 2)
            return 'I';
        else if(cursor == 3)
            return 'b';
        return 0;
    }

    @Override
    public int getMaxCursor()
    {
        return 3;
    }

    /** Get the interaction mode (Absolute, relative-aligned, relative-full) to use for this trial
     * @return the interaction mode*/
    public int getTangibleMode()
    {
        return m_tangibleMode;
    }

    /** Get the trial ID of this message (related to the current dataset in use)
     * @return the trial ID*/
    public int getTrialID()
    {
        return m_trialID;
    }

    /** Get the sub trial ID of this message (related to the current trial in a given dataset). This has to be coupled with isInTraining()
     * @return the sub trial ID*/
    public int getSubTrialID()
    {
        return m_trialID;
    }

    /** Are we currently in a training trial?
     * @return true if yes, false otherwise*/
    public boolean isInTraining() {return m_inTraining;}
}
