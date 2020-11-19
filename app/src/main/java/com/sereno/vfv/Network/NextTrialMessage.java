package com.sereno.vfv.Network;

public class NextTrialMessage extends ServerMessage
{
    /** The current trial*/
    private int m_trialID     = -1;

    /** The volumetric mapping to use*/
    private int m_techniqueID = -1;

    @Override
    void pushValue(int value)
    {
        if(cursor == 0)
            m_trialID = value;
        else if(cursor == 1)
            m_techniqueID = value;
        super.pushValue(value);
    }

    @Override
    byte getCurrentType()
    {
        if(cursor <= 1)
            return 'I';
        return 0;
    }

    @Override
    int getMaxCursor()
    {
        return 1;
    }


    int getTrialID()
    {
        return m_trialID;
    }

    int getTechniqueID()
    {
        return m_techniqueID;
    }
}
