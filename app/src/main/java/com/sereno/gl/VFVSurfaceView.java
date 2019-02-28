package com.sereno.gl;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.AttributeSet;

import com.sereno.vfv.Data.ApplicationModel;
import com.sereno.vfv.Data.BinaryDataset;
import com.sereno.vfv.Data.Dataset;
import com.sereno.vfv.Data.SubDataset;
import com.sereno.vfv.Data.VTKDataset;

import java.util.ArrayList;

public class VFVSurfaceView extends GLSurfaceView implements ApplicationModel.IDataCallback, SubDataset.ISubDatasetCallback
{
    /** @brief Listener interface for VFVSurfaceView*/
    public interface VFVSurfaceViewListener
    {
        /** @brief Method called when a new rotation on the current SubDataset is performed
         * @param dataset the dataset being rotated (called AFTER rotation is performed)
         * @param dRoll the delta roll applied
         * @param dPitch the delta pitch applied
         * @param dYaw the delta yaw applied*/
        void onRotationEvent(SubDataset dataset, float dRoll, float dPitch, float dYaw);
    }

    /** The native C++ pointer*/
    private long m_ptr;

    /** List of listeners to call when the state changed*/
    private ArrayList<VFVSurfaceViewListener> m_listeners = new ArrayList<>();

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

    /** @brief Add the Listener l in the listener list
     * @param l the listener to add*/
    public void addListener(VFVSurfaceViewListener l)
    {
        m_listeners.add(l);
    }

    /** @brief Remove the Listener l in the listener list
     * @param l the listener to remove*/
    public void removeListener(VFVSurfaceViewListener l)
    {
        m_listeners.remove(l);
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
        onAddDataset(model, fd);
    }

    @Override
    public void onAddVTKDataset(ApplicationModel model, VTKDataset d)
    {
        //C++ callback
        nativeAddVTKDataset(m_ptr, d.getPtr());
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

    /** @brief Function called from C++ code when a rotation is performed
     * @param sdPtr the SubDataset C++ native pointer being rotated (already rotated)
     * @param dRoll the delta roll applied
     * @param dPitch the delta pitch applied
     * @param dYaw the delta yaw applied*/
    public void onRotationEvent(long sdPtr, float dRoll, float dPitch, float dYaw)
    {
        SubDataset sd = new SubDataset(sdPtr);
        for(VFVSurfaceViewListener l : m_listeners)
            l.onRotationEvent(sd, dRoll, dPitch, dYaw);
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
     * @param sdPtr the SubDataset native pointer*/
    private native void nativeOnRangeColorChange(long ptr, float min, float max, int mode, long sdPtr);
}
