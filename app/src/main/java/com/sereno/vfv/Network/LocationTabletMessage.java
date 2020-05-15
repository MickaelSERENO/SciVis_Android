package com.sereno.vfv.Network;

public class LocationTabletMessage extends ServerMessage{

    /** The new position to apply*/
    private float[] m_position = new float[3];
    /** The new rotation to apply*/
    private float[] m_rotation = new float[4];


    @Override
    public byte getCurrentType()
    {
        return 'f';
    }

    @Override
    public void pushValue(float val)
    {
        if(cursor < 3)
            m_position[cursor] = val;
        else
            m_rotation[cursor-3] = val;
        super.pushValue(val);
    }

    @Override
    public int getMaxCursor()
    {
        return 6;
    }


    /** The position parsed.
     * @return the Vector3 position parsed*/
    public float[] getPosition()
    {
        return m_position;
    }

    /** The rotation parsed.
     * @return the Vector4 rotation parsed*/
    public float[] getRotation()
    {
        return m_rotation;
    }
}
