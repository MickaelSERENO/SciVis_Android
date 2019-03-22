package com.sereno.vfv.Network;

public class HeadsetBindingInfoMessage extends ServerMessage
{
    /** The Headset ID*/
    private int m_headsetID;

    /** The displayed headset color. R = (m_headsetColor >> 16) & 0xff, G = (m_headsetColor >> 8) & 0xff, B = m_headsetColor & 0xff*/
    private int m_headsetColor;

    /** Is the tablet connected ? (should be always true)*/
    private boolean m_tabletConnected;

    /** Is this headset the first headset connected?*/
    private boolean m_firstConnected;

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
    public void pushValue(byte v)
    {
        if(cursor == 2)
            m_tabletConnected = (v!=0);
        else if(cursor == 3)
            m_firstConnected = (v!=0);
        super.pushValue(v);
    }

    @Override
    public byte getCurrentType()
    {
        if(cursor == 0)
            return 'I';
        else if(cursor == 1)
            return 'I';
        else if(cursor == 2 || cursor == 3)
            return 'b';
        return 0;
    }

    @Override
    public int getMaxCursor()
    {
        return 3;
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
