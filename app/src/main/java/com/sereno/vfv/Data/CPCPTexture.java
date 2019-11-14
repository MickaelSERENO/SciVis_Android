package com.sereno.vfv.Data;

import android.graphics.Bitmap;

/** Class containing CPCPTexture data*/
public class CPCPTexture
{
    /** The Dataset owning this texture*/
    private Dataset m_dataset;

    /** The ARGB texture*/
    private Bitmap m_bitmap;

    /** the point field ID represented on the left axis*/
    private int    m_pIDLeft;

    /** the point field ID represented on the right axis*/
    private int    m_pIDRight;

    /** Method called when a Dataset has loaded a new CPCPTexture
     * @param dataset the Dataset that has loaded the CPCPTexture
     * @param bitmap the bitmap generated
     * @param pIDLeft the point field ID represented on the left axis
     * @param pIDRight the point field ID represented on the right axis*/
    public CPCPTexture(Dataset dataset, Bitmap bitmap, int pIDLeft, int pIDRight)
    {
        m_dataset  = dataset;
        m_bitmap   = bitmap;
        m_pIDLeft  = pIDLeft;
        m_pIDRight = pIDRight;
    }

    /** Get the point field ID represented on the texture left axis
     * @return the point field ID*/
    public int getPIDLeft()
    {
        return m_pIDLeft;
    }

    /** Get the point field ID represented on the texture right axis
     * @return the point field ID*/
    public int getPIDRight()
    {
        return m_pIDRight;
    }

    /** Get the Dataset owning this CPCPTexture
     * @return the Dataset owner*/
    public Dataset getDataset()
    {
        return m_dataset;
    }

    /** Get the texture Bitmap object
     * @return the Bitmap texture*/
    public Bitmap getBitmap()
    {
        return m_bitmap;
    }
}