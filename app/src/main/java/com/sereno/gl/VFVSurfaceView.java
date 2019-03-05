package com.sereno.gl;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.AttributeSet;

import com.sereno.vfv.Data.ApplicationModel;
import com.sereno.vfv.Data.BinaryDataset;
import com.sereno.vfv.Data.Dataset;
import com.sereno.vfv.Data.SubDataset;
import com.sereno.vfv.Data.VTKDataset;

public class VFVSurfaceView extends GLSurfaceView implements ApplicationModel.IDataCallback, SubDataset.ISubDatasetCallback
{
    /** The native C++ pointer*/
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

    /** Change the current SubDataset being displayed
     * @param sd the new SubDataset to display*/
    public void changeCurrentSubDataset(SubDataset sd)
    {
        nativeChangeCurrentSubDataset(m_ptr, sd.getNativePtr());
    }

    @Override
    public void onAddBinaryDataset(ApplicationModel model, BinaryDataset fd)
    {
        nativeAddBinaryDataset(fd, m_ptr, fd.getPtr());
        onAddDataset(model, fd);
    }

    @Override
    public void onAddVTKDataset(ApplicationModel model, VTKDataset d)
    {
        //C++ callback
        nativeAddVTKDataset(d, m_ptr, d.getPtr());
        onAddDataset(model, d);
    }

    public void onAddDataset(ApplicationModel model, Dataset d)
    {
        for(int i = 0; i < d.getNbSubDataset(); i++)
            d.getSubDataset(i).addListener(this);
    }

    @Override
    public void onRangeColorChange(SubDataset sd, float min, float max, int mode)
    {
        nativeOnRangeColorChange(m_ptr, min, max, mode, sd.getNativePtr());
    }

    @Override
    public void onRotationEvent(SubDataset dataset, float dRoll, float dPitch, float dYaw)
    {
        //We generated this event
    }

    @Override
    public void onRotationEvent(SubDataset dataset, float[] quaternion)
    {
        nativeOnRotationChange(m_ptr, dataset.getNativePtr());
    }

    @Override
    public void onSnapshotEvent(SubDataset dataset, Bitmap snapshot)
    {
        //We generated this event
    }

    /** \brief Create the argument to send to the main function
     * \return the main argument as a ptr (long value)*/
    private native long nativeCreateMainArgs();

    /** \brief Delete the arguments sent to the main function
     * @param ptr the ptr to delete*/
    private native long nativeDeleteMainArgs(long ptr);

    /** Change the current sub dataset to display
     * @param ptr the ptr associated with the main Argument
     * @param sdPtr the SubDataset native ptr*/
    private native void nativeChangeCurrentSubDataset(long ptr, long sdPtr);

    /** \brief Add the dataset into the cpp application
     * @param bd the binary dataset bound to the fd pointer
     * @param ptr the ptr associated with the main Argument
     * @param fd the BinaryDataset to add*/
    private native void nativeAddBinaryDataset(BinaryDataset bd, long ptr, long fd);

    /** @brief Add a VTKParser into the cpp application
     * @param vtk the VTKDataset bound to the vtkDataPtr
     * @param ptr the ptr associated with the main Argument
     * @param vtkDataPtr the vtk dataset C++ pointer object*/
    private native void nativeAddVTKDataset(VTKDataset vtk, long ptr, long vtkDataPtr);

    /** \brief Remove the dataset index i into the cpp application
     * @param ptr the ptr associated with the main argument
     * @param idx the BinaryDataset index*/
    private native void nativeRemoveData(long ptr, int idx);

    /** \brief Set the color of the current dataset
     * @param ptr the ptr associated with the main Argument
     * @param min the minimum range (0.0, 1.0)
     * @param max the maximum range (0.0, 1.0)
     * @param mode the color mode to apply (See ColorMode)
     * @param sdPtr the SubDataset native pointer*/
    private native void nativeOnRangeColorChange(long ptr, float min, float max, int mode, long sdPtr);

    /** Send an event regarding an update from a SubDataset roation
     * @param ptr the ptr associated with the main Argument
     * @param sdPtr the SubDataset native pointer*/
    private native void nativeOnRotationChange(long ptr, long sdPtr);
}
