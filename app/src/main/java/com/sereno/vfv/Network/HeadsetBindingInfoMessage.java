package com.sereno.vfv.Network;

public class HeadsetBindingInfoMessage extends ServerMessage
{
    /** The Headset ID*/
    private int m_headsetID;

    /** The displayed headset color. R = (m_headsetColor >> 16) & 0xff, G = (m_headsetColor >> 8) & 0xff, B = m_headsetColor & 0xff*/
    private int m_headsetColor;

    @Override
    public void pushValue(int value)
    {
        if(cursor == 0)
            m_headsetID = value;
        else if(cursor == 1)
            m_headsetColor = value;
        super.pushValue(value);
    }

    @Override
    public byte getCurrentType()
    {
        if(cursor == 0)
            return 'I';
        else if(cursor == 1)
            return 'I';
        return 0;
    }

    @Override
    public int getMaxCursor()
    {
        return 1;
    }

    /** Get the headset ID bound with this tablet
     * @return the headset ID as defined by the server*/
    public int getHeadsetID()
    {
        return m_headsetID;
    }

    /** Get the headset color reprenting the headset bound with this tablet
     * @return the RGB headset color. R = (m_headsetColor >> 16) & 0xff, G = (m_headsetColor >> 8) & 0xff, B = m_headsetColor & 0xff*/
    public int getHeadsetColor()
    {
        return m_headsetColor;
    }
}
