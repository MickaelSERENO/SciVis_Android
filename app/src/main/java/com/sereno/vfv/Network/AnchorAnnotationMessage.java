package com.sereno.vfv.Network;

public class AnchorAnnotationMessage extends ServerMessage
{
    /** The dataset ID*/
    private int m_datasetID;

    /**The SubDataset ID*/
    private int m_subDatasetID;

    /**The annotation ID*/
    private int m_annotationID;

    /**The headset ID*/
    private int m_headsetID;

    /**Do this annotation belongs in the subdataset public space?*/
    private boolean m_inPublic;

    /** The annotation's 3D position*/
    private float[] m_position = new float[3];

    @Override
    public byte getCurrentType()
    {
        if(cursor <= 3)
            return 'I';
        else if(cursor == 4)
            return 'b';
        return 'f';
    }

    @Override
    public void pushValue(int val)
    {
        if(cursor == 0)
            m_datasetID = val;
        else if(cursor == 1)
            m_subDatasetID = val;
        else if(cursor == 2)
            m_annotationID = val;
        else if(cursor == 3)
            m_headsetID = val;
        super.pushValue(val);
    }

    @Override
    public void pushValue(byte val)
    {
        m_inPublic = (val == 0 ? false : true);
        super.pushValue(val);
    }

    @Override
    public void pushValue(float val)
    {
        m_position[cursor-5] = val;
        super.pushValue(val);
    }

    @Override
    public int getMaxCursor()
    {
        return 7;
    }

    /** Get the dataset ID parsed
     * @return the dataset ID to update. Need to be used with getSubDatasetID()*/
    public int getDatasetID()
    {
        return m_datasetID;
    }

    /** Get the subdataset ID parsed
     * @return the subdataset ID to update. Update dataset ID = getDatasetID() .getSubDataset[getSubDatasetID()].*/
    public int getSubDatasetID()
    {
        return m_subDatasetID;
    }

    /** Get the headset ID parsed.
     * @return the headset ID moving this dataset. -1 == server call*/
    public int getHeadsetID() {return m_headsetID;}

    /** Get the annotation ID parsed
     * @return the annotation ID*/
    public int getAnnotationID() {return m_annotationID;}

    /** The position parsed.
     * @return the Vector3 position parsed*/
    public float[] getPosition()
    {
        return m_position;
    }

    /** Was the new position given in the public space?
     * @return true if in the public space, false if in the private space*/
    public boolean doneIntoPublicSpace() {return m_inPublic;}
}
