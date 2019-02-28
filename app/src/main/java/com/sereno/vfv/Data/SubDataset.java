package com.sereno.vfv.Data;

import android.graphics.Bitmap;

import com.sereno.color.ColorMode;
import com.sereno.gl.VFVSurfaceView;

import java.util.ArrayList;
import java.util.List;

public class SubDataset
{
    /** @brief Callback interface called when the SubDataset is modified*/
    public interface ISubDatasetCallback
    {
        /** @brief On range color change callback, called when the color changed
         * @param sd the SubDataset being modified
         * @param min the minimum clamping color (normalized : between 0.0f and 1.0f)
         * @param max the maximum clamping color (normalized : between 0.0f and 1.0f)
         * @param mode the ColorMode
         */
        void onRangeColorChange(SubDataset sd, float min, float max, int mode);


        /** @brief Method called when a new rotation on the current SubDataset is performed
         * @param dataset the dataset being rotated (called AFTER rotation is performed)
         * @param dRoll the delta roll applied
         * @param dPitch the delta pitch applied
         * @param dYaw the delta yaw applied*/
        void onRotationEvent(SubDataset dataset, float dRoll, float dPitch, float dYaw);
    }

    /** The native C++ handle*/
    protected long m_ptr;

    private List<ISubDatasetCallback> m_listeners = new ArrayList<>();

    /** @brief Constructor. Link the Java object with the C++ native SubDataset object
     * @param ptr the native C++ pointer*/
    public SubDataset(long ptr)
    {
        m_ptr = ptr;
    }

    /** @brief Add a new callback Listener
     * @param listener the listener to call for new events*/
    public void addListener(ISubDatasetCallback listener)
    {
        m_listeners.add(listener);
    }

    /** @brief Remove an old callback Listener
     * @param listener the listener to remove from the list*/
    public void removeListener(ISubDatasetCallback listener)
    {
        m_listeners.remove(listener);
    }

    /** @brief Tells whether this SubDataset is in a valid state or not
     * @return true if in a valid state, false otherwise*/
    public boolean isValid()
    {
        return nativeIsValid(m_ptr);
    }

    /** @brief Get the minimum amplitude of this SubDataset. For vectorial data, return the minimum length
     * @return the minimum amplitude found in this SubDataset*/
    public float getMinAmplitude()
    {
        return nativeGetMinAmplitude(m_ptr);
    }

    /** @brief Get the maximum amplitude of this SubDataset. For vectorial data, return the maximum length
     * @return the maximum amplitude found in this SubDataset*/
    public float getMaxAmplitude()
    {
        return nativeGetMaxAmplitude(m_ptr);
    }

    /** @brief Get a snapshot of this SubDataset in the main rendering object
     * @return the snapshot of the subdataset*/
    public Bitmap getSnapshot()
    {
        return nativeGetSnapshot(m_ptr);
    }

    /** @brief Get the rotation quaternion components. In order: w, i, j, k
     * @return the rotation quaternion components*/
    public float[] getRotation() {return nativeGetRotation(m_ptr);}

    /** @brief Get the native pointer of the SubDataset
     * @return the native pointer*/
    public long getNativePtr()
    {
        return m_ptr;
    }

    /** @brief Set the range color of this SubDataset being displayed
     * @param min the minimum (between 0.0 and 1.0) value to display. Values lower than min will be discarded
     * @param max the maximum (between 0.0 and 1.0) value to display. Values greater than max will be discarded
     * @param mode the ColorMode to apply*/
    public void setRangeColor(float min, float max, int mode)
    {
        nativeSetRangeColor(m_ptr, min, max, mode);
        for(ISubDatasetCallback clbk : m_listeners)
            clbk.onRangeColorChange(this, min, max, mode);
    }


    /** @brief Function called from C++ code when a rotation is performed
     * @param dRoll the delta roll applied
     * @param dPitch the delta pitch applied
     * @param dYaw the delta yaw applied*/
    public void onRotationEvent(float dRoll, float dPitch, float dYaw)
    {
        for(ISubDatasetCallback l : m_listeners)
            l.onRotationEvent(this, dRoll, dPitch, dYaw);
    }

    /** @brief Native code telling is this SubDataset is in a valid state
     * @param ptr the native pointer
     * @return true if in a valid state, false otherwise*/
    private native boolean nativeIsValid(long ptr);

    /** @brief Native code to get the minimum amplitude of this SubDataset. For vectorial data, return the minimum length
     * @param ptr the native pointer
     * @return the minimum amplitude found in this SubDataset*/
    private native float nativeGetMinAmplitude(long ptr);

    /** @brief Native code to get the maximum amplitude of this SubDataset. For vectorial data, return the maximum length
     * @param ptr the native pointer
     * @return the maximum amplitude found in this SubDataset*/
    private native float nativeGetMaxAmplitude(long ptr);

    /** @brief Native code to get a snapshot of this SubDataset in the main rendering object
     * @param ptr the native pointer
     * @return the snapshot of the subdataset*/
    private native Bitmap nativeGetSnapshot(long ptr);

    /** @brief Get the rotation quaternion components. In order: w, i, j, k
     * @return the rotation quaternion components*/
    private native float[] nativeGetRotation(long ptr);

    /** @brief Native code to set the range color of this SubDataset being displayed
     * @param ptr the native pointer
     * @param min the minimum (between 0.0 and 1.0) value to display. Values lower than min will be discarded
     * @param max the maximum (between 0.0 and 1.0) value to display. Values greater than max will be discarded
     * @param mode the ColorMode to apply*/
    private native void nativeSetRangeColor(long ptr, float min, float max, int mode);
}
