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
            callOnUpdateListeners();
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
            callOnUpdateListeners();
        }
    }

    public void setClippingValues(float min, float max)
    {
        boolean changed = (min != getMinClipping() || max != getMaxClipping());
        if(changed)
        {
            min = Math.max(Math.min(min, 1.0f), 0.0f);
            max = Math.max(Math.min(max, 1.0f), 0.0f);

            if(min > max)
            {
                float _t = min;
                min = max;
                max = _t;
            }

            nativeSetClipping(getNativeTransferFunction(), min, max);
            callOnUpdateListeners();
        }
    }

    /** Call every listeners for the method "onUpdateTransferFunction"*/
    protected void callOnUpdateListeners()
    {
        for(ITransferFunctionListener l : m_listeners)
            l.onUpdateTransferFunction(this);
    }

    /** Get The minimal clipping values in value domain (between 0.0f and 1.0f). Default: 0.0f
     * @return the current minimum clipping value*/
    public float getMinClipping() {return nativeGetMinClipping(getNativeTransferFunction());}

    /** Get The maximal clipping values in value domain (between 0.0f and 1.0f). Default: 1.0f
     * @return the current maximal clipping value*/
    public float getMaxClipping() {return nativeGetMaxClipping(getNativeTransferFunction());}

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

    /** Get the minimum clipping of the transfer function
     * @param tfPtr the native transfer function pointer
     * @return the clipping to apply*/
    protected native float nativeGetMinClipping(long tfPtr);

    /** Get the maximum clipping of the transfer function
     * @param tfPtr the native transfer function pointer
     * @return the clipping to apply*/
    protected native float nativeGetMaxClipping(long tfPtr);

    /** Set the clipping of the transfer function
     * @param tfPtr the native transfer function pointer
     * @param min the new minimum clipping to apply
     * @param max the new maximum clipping to apply*/
    protected native float nativeSetClipping(long tfPtr, float min, float max);

    /** Delete the native C++ std::shared_ptr\<TF\> object
     * @param tfPtr the pointer to delete*/
    protected native void nativeDeleteTF(long tfPtr);
}