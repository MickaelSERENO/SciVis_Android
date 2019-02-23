package com.sereno.gl;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.AttributeSet;

import com.sereno.vfv.Data.ApplicationModel;
import com.sereno.vfv.Data.BinaryDataset;
import com.sereno.vfv.Data.VTKDataset;

public class VFVSurfaceView extends GLSurfaceView implements ApplicationModel.IDataCallback
{
    private long m_ptr;

    public VFVSurfaceView(Context context)
    {
        super(context);
        m_ptr = nativeCreateMainArgs();
    }

    public VFVSurfaceView(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        m_ptr = nativeCreateMainArgs();
    }

    public VFVSurfaceView(Context context, AttributeSet attrs, int defStyle)
    {
        super(context, attrs, defStyle);
        m_ptr = nativeCreateMainArgs();
    }

    @Override
    protected void finalize() throws Throwable
    {
        super.finalize();
        nativeDeleteMainArgs(m_ptr);
    }

    /** \brief Function which has for aim to be overrided.
     * \return the cpp argument to send to the main function*/
    @Override
    protected long getMainArgument()
    {
        return m_ptr;
    }

    @Override
    public void onAddBinaryDataset(ApplicationModel model, BinaryDataset fd)
    {
        nativeAddBinaryDataset(m_ptr, fd.getPtr());
    }

    @Override
    public void onAddVTKDataset(ApplicationModel model, VTKDataset d)
    {
        //C++ callback
        nativeAddVTKDataset(m_ptr, d.getPtr());
    }

    /** \brief Function that set the current range color for the displayed dataset
     * @param min the minimum value
     * @param max the maximum value
     * @param mode the color mode to apply (see ColorMode)*/
    public void setCurrentRangeColor(float min, float max, int mode)
    {
        nativeOnRangeColorChange(m_ptr, min, max, mode);
    }

    /** \brief Create the argument to send to the main function
     * \return the main argument as a ptr (long value)*/
    private native long nativeCreateMainArgs();

    /** \brief Delete the arguments sent to the main function
     * @param ptr the ptr to delete*/
    private native long nativeDeleteMainArgs(long ptr);

    /** \brief Add the dataset into the cpp application
     * @param ptr the ptr associated with the main Argument
     * @param fd the BinaryDataset to add*/
    private native void nativeAddBinaryDataset(long ptr, long fd);

    /** @brief Add a VTKParser into the cpp application
     * @param ptr the ptr associated with the main Argument
     * @param vtkDataPtr the vtk dataset C++ pointer object*/
    private native void nativeAddVTKDataset(long ptr, long vtkDataPtr);

    /** \brief Remove the dataset index i into the cpp application
     * @param ptr the ptr associated with the main argument
     * @param idx the BinaryDataset index*/
    private native void nativeRemoveData(long ptr, int idx);

    /** \brief Set the color of the current dataset
     * @param ptr the ptr associated with the main Argument
     * @param min the minimum range (0.0, 1.0)
     * @param max the maximum range (0.0, 1.0)
     * @param mode the color mode to apply (See ColorMode)
     */
    private native void nativeOnRangeColorChange(long ptr, float min, float max, int mode);
}
