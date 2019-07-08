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

    @Override
    public void pushValue(int val)
    {
        if(cursor == 0)
            m_curTabletID = val;
        else if(cursor == 1)
            m_curTrialID = val;
        super.pushValue(val);
    }

    @Override
    public void pushValue(float val)
    {
        if(cursor <= 4)
            m_annotPos[cursor-2] = val;
        super.pushValue(val);
    }

    @Override
    public byte getCurrentType()
    {
        if(cursor <= 1)
            return 'I';
        else if(cursor <= 4)
            return 'f';
        return 0;
    }

    @Override
    public int getMaxCursor()
    {
        return 4;
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
}
