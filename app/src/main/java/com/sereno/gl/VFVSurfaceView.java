package com.sereno.gl;

import android.content.Context;
import android.util.AttributeSet;

import com.sereno.vfs.Data.ApplicationModel;
import com.sereno.vfs.Data.FluidDataset;

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
    public void finalize() throws Throwable
    {
        super.finalize();
        nativeDeleteMainArgs(m_ptr);
    }

    /* \brief Function which has for aim to be overrided.
     * \return the cpp argument to send to the main function*/
    @Override
    protected long getMainArgument()
    {
        return m_ptr;
    }

    @Override
    public void onAddDataset(ApplicationModel model, FluidDataset fd)
    {
        nativeAddData(m_ptr, fd.getPtr());
    }

    @Override
    public void onDeleteDataset(ApplicationModel model, int idx)
    {
        nativeRemoveData(m_ptr, idx);
    }

    /* \brief Function that set the current range color for the displayed dataset
     * \param min the minimum value
     * \param max the maximum value*/
    public void setCurrentRangeColor(float min, float max)
    {
        nativeOnRangeColorChange(m_ptr, min, max);
    }

    /* \brief Create the argument to send to the main function
     * \return the main argument as a ptr (long value)*/
    private native long nativeCreateMainArgs();

    /* \brief Delete the arguments sent to the main function
     * \param ptr the ptr to delete*/
    private native long nativeDeleteMainArgs(long ptr);

    /* \brief Add the dataset into the cpp application
     * \param ptr the ptr associated with the main Argument
     * \param fd the FluidDataset to add*/
    private native void nativeAddData(long ptr, long fd);

    /* \brief Remove the dataset index i into the cpp application
     * \param ptr the ptr associated with the main argument
     * \param idx the FluidDataset index*/
    private native void nativeRemoveData(long ptr, int idx);

    /* \brief Set the color of the current dataset
     * \param ptr the ptr associated with the main Argument
     * \param min the minimum range (0.0, 1.0)
     * \param max the maximum range (0.0, 1.0)
     */
    private native void nativeOnRangeColorChange(long ptr, float min, float max);
}
