package com.sereno.vfv.Network;

/** Class handling trial data messages*/
public class TrialDataCHI2020Message extends ServerMessage
{
    /** The target annotation position*/
    private float[] m_annotPos = new float[3];

    /** The current tablet ID that should handle the next annotation*/
    private int     m_curTabletID;

    /** The current trial ID*/
    private int     m_curTrialID;

    /** The current study ID. 0 == training, 1 == study 1, 2 == study 2. 3 == finish*/
    private int     m_curStudyID;

    /** The pointing ID to use during this trial*/
    private int     m_pointingID;

    @Override
    public void pushValue(int val)
    {
        if(cursor == 0)
            m_curTabletID = val;
        else if(cursor == 1)
            m_curTrialID = val;
        else if(cursor == 2)
            m_curStudyID = val;
        else if(cursor == 3)
            m_pointingID = val;
        super.pushValue(val);
    }

    @Override
    public void pushValue(float val)
    {
        if(cursor <= 6)
            m_annotPos[cursor-4] = val;
        super.pushValue(val);
    }

    @Override
    public byte getCurrentType()
    {
        if(cursor <= 3)
            return 'I';
        else if(cursor <= 6)
            return 'f';
        return 0;
    }

    @Override
    public int getMaxCursor()
    {
        return 6;
    }

    /** Get the target annotation position
     * @return the target annotation position the bound headset will have to anchor*/
    public float[] getAnnotationPos()
    {
        return m_annotPos;
    }

    /** Get the current tablet ID.
     * @return the tablet ID that should perform this trial*/
    public int getCurrentTabletID()
    {
        return m_curTabletID;
    }

    /** Get the current trial ID
     * @return the current trial ID*/
    public int getCurrentTrialID()
    {
        return m_curTrialID;
    }

    /** Get the current study ID. Either 1 or 2. 3 == finish
     * @return the current study ID*/
    public int getCurrentStudyID() { return m_curStudyID; }

    /** The pointing ID to use during this trial
     * @return the pointing ID*/
    public int getPointingID() { return m_pointingID; }
}
