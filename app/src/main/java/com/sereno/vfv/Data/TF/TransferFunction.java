package com.sereno.vfv.Data.TF;

import com.sereno.color.ColorMode;
import com.sereno.vfv.Data.SubDataset;
import com.sereno.vfv.Network.MessageBuffer;

import java.util.ArrayList;

public abstract class TransferFunction
{
    /** Interface useful when the associated transfer function is being updated*/
    public interface ITransferFunctionListener
    {
        /** Called when the transfer function "tf" is being updated
         * @param tf the transfer function being updated*/
        void onUpdateTransferFunction(TransferFunction tf);
    }

    /** The listener to call when the transfer function has changed*/
    private ArrayList<ITransferFunctionListener> m_listeners = new ArrayList<>();

    /** The color mode in application*/
    private int m_colorMode = ColorMode.RAINBOW;

    public void addListener(ITransferFunctionListener listener)
    {
        if(!m_listeners.contains(listener))
            m_listeners.add(listener);
    }

    public void removeListener(ITransferFunctionListener listener)
    {
        m_listeners.remove(listener);
    }

    /** Get the current timer this transfer function is set to
     * @return the current timestep this transfer function is currently applying. The time is based on the timeline of the associated visualization
     * (i.e., it does not represent absolute seconds/minutes/whatsoever)*/
    public float getTimestep() {return nativeGetTimestep(getNativeTransferFunction());}

    /** Set the current timer this transfer function is set to
     * @param t the new timestep this transfer function should apply. The time is based on the timeline of the associated visualization
     * (i.e., it does not represent absolute seconds/minutes/whatsoever)*/
    public void setTimestep(float t)
    {
        if(t != getTimestep())
        {
            nativeSetTimestep(getNativeTransferFunction(), t);
            for(int i = 0; i < m_listeners.size(); i++)
                m_listeners.get(i).onUpdateTransferFunction(this);
        }
    }

    /** Get the color mode to apply to the visualization widget
     * @return the color mode to apply (see ColorMode static fields)*/
    public int getColorMode()
    {
        return m_colorMode;
    }

    /** Set the color mode to apply to the visualization widget
     * @param mode the color mode to apply (see ColorMode static fields)*/
    public void setColorMode(int mode)
    {
        boolean changed = (mode != getColorMode());
        if(changed)
        {
            m_colorMode = mode;
            nativeSetColorMode(getNativeTransferFunction(), mode);
            for(int i = 0; i < m_listeners.size(); i++)
                m_listeners.get(i).onUpdateTransferFunction(this);
        }
    }

    /** Call every listeners for the method "onUpdateTransferFunction"*/
    protected void callOnUpdateListeners()
    {
        for(ITransferFunctionListener l : m_listeners)
            l.onUpdateTransferFunction(this);
    }

    public int getType()
    {
        return SubDataset.TRANSFER_FUNCTION_NONE;
    }

    /** Create a native std::shared_ptr\<TF\> associated with this TransferFunction java counterpart
     * @return the native pointer, or 0 if an issue occured*/
    public abstract long getNativeTransferFunction();

    /** Get the colormode associated to this transfer function
     * @param tfPtr the native transfer function pointer*/
    protected native int nativeGetColorMode(long tfPtr);

    /** Set the colormode associated to this transfer function
     * @param tfPtr the native transfer function pointer
     * @param mode the new color mode*/
    protected native void nativeSetColorMode(long tfPtr, int mode);

    /** Set the timestep of the transfer function
     * @param tfPtr the native transfer function pointer
     * @param timestep the new timestep to apply*/
    protected native void nativeSetTimestep(long tfPtr, float timestep);

    /** Get the timestep of the transfer function
     * @param tfPtr the native transfer function pointer
     * @return the timestep to apply*/
    protected native float nativeGetTimestep(long tfPtr);

    /** Delete the native C++ std::shared_ptr\<TF\> object
     * @param tfPtr the pointer to delete*/
    protected native void nativeDeleteTF(long tfPtr);
}