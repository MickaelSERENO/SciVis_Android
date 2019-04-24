package com.sereno.vfv.Network;

import android.util.Log;

public class HeadsetsStatusMessage extends ServerMessage
{
    public static final int HEADSET_CURRENT_ACTION_NOTHING   = 0;
    public static final int HEADSET_CURRENT_ACTION_MOVING    = 1;
    public static final int HEADSET_CURRENT_ACTION_SCALING   = 2;
    public static final int HEADSET_CURRENT_ACTION_ROTATING  = 3;
    public static final int HEADSET_CURRENT_ACTION_SKETCHING = 4;

    public static class HeadsetStatus
    {
        /**  The X, Y and Z 3D position */
        public float[] position = new float[3];

        /**  The W, X, Y and Z 3D Quaternion rotation */
        public float[] rotation = new float[4];

        /**  The headset ID */
        public int id;

        /**  The headset Color */
        public int color;

        /**  The headset current action */
        public int currentAction = HEADSET_CURRENT_ACTION_NOTHING;
    }

    /** Array of the headset status*/
    private HeadsetStatus[] m_status = null;

    @Override
    public void pushValue(float value)
    {
        int headset = (cursor-1)/10;
        int id      = (cursor-1)%10;

        if(id < 6) //Position
            m_status[headset].position[id - 3] = value;
        else if(id < 10) //Rotation
            m_status[headset].rotation[id - 6] = value;
        super.pushValue(value);
    }

    @Override
    public void pushValue(int value)
    {
        //Number of headsets
        if(cursor == 0)
        {
            m_status = new HeadsetStatus[value];
            for(int i = 0; i < value; i++)
                m_status[i] = new HeadsetStatus();
        }
        else
        {
            int id = (cursor-1)%10;
            int headset = (cursor-1)/10;

            if(id == 0)
                m_status[headset].id = value;
            else if(id == 1)
                m_status[headset].color = value;
            else if(id == 2)
                m_status[headset].currentAction = value;
        }
        super.pushValue(value);
    }

    @Override
    public byte getCurrentType()
    {
        if(cursor == 0) //Number of headsets
            return (byte)'I';
        else
        {
            int id = (cursor-1) % 10;

            if(id < 3) //Color / ID / current action
                return (byte)'I';

            if(id < 6) //Position
                return (byte)'f';

            else if(id < 10) //Rotation
                return (byte)'f';
        }
        return 0;
    }

    @Override
    public int getMaxCursor() { return (m_status != null ? m_status.length*10: 0); }

    /** Get the headsets status parsed
     * @return array of headsets status parsed*/
    public HeadsetStatus[] getStatus() {return m_status;}
}
