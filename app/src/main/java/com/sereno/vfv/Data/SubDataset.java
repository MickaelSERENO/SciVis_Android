package com.sereno.vfv.Data;

import android.graphics.Bitmap;

import com.sereno.color.ColorMode;
import com.sereno.gl.VFVSurfaceView;
import com.sereno.view.AnnotationData;

import java.lang.annotation.Annotation;
import java.util.ArrayList;
import java.util.List;

public class SubDataset
{
    /** Callback interface called when the SubDataset is modified*/
    public interface ISubDatasetListener
    {
        /** On range color change callback, called when the color changed
         * @param sd the SubDataset being modified
         * @param min the minimum clamping color (normalized : between 0.0f and 1.0f)
         * @param max the maximum clamping color (normalized : between 0.0f and 1.0f)
         * @param mode the ColorMode*/
        void onRangeColorChange(SubDataset sd, float min, float max, int mode);

        /** Method called when a new rotation on the current SubDataset is performed
         * @param dataset the dataset being rotated
         * @param quaternion the quaternion rotation*/
        void onRotationEvent(SubDataset dataset, float[] quaternion);

        /** Method called when a translation on the current SubDataset is performed
         * @param dataset the dataset being rotated
         * @param position the new 3D position*/
        void onPositionEvent(SubDataset dataset, float[] position);

        /** Method called when a new scaling on the current SubDataset is performed
         * @param dataset the dataset being rotated
         * @param scale the new scaling factors*/
        void onScaleEvent(SubDataset dataset, float[] scale);

        /** Method called when a new snapshot on the current SubDataset is performed
         * @param dataset the dataset creating a new snapshot
         * @param snapshot the snapshot created*/
        void onSnapshotEvent(SubDataset dataset, Bitmap snapshot);

        /** Method called when a new annotation has been added to this SubDataset
         * @param dataset the dataset receiving a new annotation
         * @param annotation the annotation added*/
        void onAddAnnotation(SubDataset dataset, AnnotationData annotation);
    }

    /** The native C++ handle*/
    protected long m_ptr;

    /** List of listeners bound to this SubDataset*/
    private List<ISubDatasetListener> m_listeners = new ArrayList<>();

    /** List of annotations bound to this SubDataset*/
    private List<AnnotationData> m_annotations = new ArrayList<>();

    /** The current headset owning this subdataset*/
    private int m_ownerHeadsetID = -1;


    /** Constructor. Link the Java object with the C++ native SubDataset object
     * @param ptr the native C++ pointer*/
    public SubDataset(long ptr)
    {
        m_ptr = ptr;
    }

    /** Add a new callback Listener
     * @param listener the listener to call for new events*/
    public void addListener(ISubDatasetListener listener)
    {
        m_listeners.add(listener);
    }

    /** Remove an old callback Listener
     * @param listener the listener to remove from the list*/
    public void removeListener(ISubDatasetListener listener)
    {
        m_listeners.remove(listener);
    }

    /** Tells whether this SubDataset is in a valid state or not
     * @return true if in a valid state, false otherwise*/
    public boolean isValid()
    {
        return nativeIsValid(m_ptr);
    }

    /** Get the minimum amplitude of this SubDataset. For vectorial data, return the minimum length
     * @return the minimum amplitude found in this SubDataset*/
    public float getMinAmplitude()
    {
        return nativeGetMinAmplitude(m_ptr);
    }

    /** Get the maximum amplitude of this SubDataset. For vectorial data, return the maximum length
     * @return the maximum amplitude found in this SubDataset*/
    public float getMaxAmplitude()
    {
        return nativeGetMaxAmplitude(m_ptr);
    }

    /** Get a snapshot of this SubDataset in the main rendering object
     * @return the snapshot of the subdataset*/
    public Bitmap getSnapshot()
    {
        return nativeGetSnapshot(m_ptr);
    }

    /** Get the rotation quaternion components. In order: w, i, j, k
     * @return the rotation quaternion components*/
    public float[] getRotation() {return nativeGetRotation(m_ptr);}

    /** Get the 3D position components. In order: x, y, z
     * @return the position quaternion components*/
    public float[] getPosition() {return nativeGetPosition(m_ptr);}

    /** Get the 3D scaling components. In order: x, y, z
     * @return the 3D scaling components*/
    public float[] getScale() {return nativeGetScale(m_ptr);}

    /** Get the native pointer of the SubDataset
     * @return the native pointer*/
    public long getNativePtr()
    {
        return m_ptr;
    }

    /** Set the range color of this SubDataset being displayed
     * @param min the minimum (between 0.0 and 1.0) value to display. Values lower than min will be discarded
     * @param max the maximum (between 0.0 and 1.0) value to display. Values greater than max will be discarded
     * @param mode the ColorMode to apply*/
    public void setRangeColor(float min, float max, int mode)
    {
        nativeSetRangeColor(m_ptr, min, max, mode);
        for(int i = 0; i < m_listeners.size(); i++)
            m_listeners.get(i).onRangeColorChange(this, min, max, mode);
    }

    /** Get the current minimum clamping color of this SubDataset
     * @return the current minimum clamping color  being displayed*/
    public float getMinClampingColor()
    {
        return nativeGetMinClampingColor(m_ptr);
    }

    /** Get the current maximum clamping color of this SubDataset
     * @return the current maximum clamping color  being displayed*/
    public float getMaxClampingColor()
    {
        return nativeGetMaxClampingColor(m_ptr);
    }


    /** Get the current color mode of this SubDataset
     * @return the current color mode being displayed*/
    public int getColorMode()
    {
        return nativeGetColorMode(m_ptr);
    }

    /** Function called frm C++ code when a new snapshot has been created
     * @param bmp the new bitmap snapshot*/
    public void onSnapshotEvent(Bitmap bmp)
    {
        for(ISubDatasetListener l : m_listeners)
            l.onSnapshotEvent(this, bmp);
    }

    /** Set the rotation of this SubDataset
     * @param quaternion the new rotation to apply (w, x, y, z)*/
    public void setRotation(float[] quaternion)
    {
        nativeSetRotation(m_ptr, quaternion);
        for(ISubDatasetListener l : m_listeners)
            l.onRotationEvent(this, quaternion);
    }

    /** Set the position of this SubDataset
     * @param position the new position to apply*/
    public void setPosition(float[] position)
    {
        nativeSetPosition(m_ptr, position);
        for(ISubDatasetListener l : m_listeners)
            l.onPositionEvent(this, position);
    }

    /** Set the scaling of this SubDataset
     * @param scale the new scale to apply*/
    public void setScale(float[] scale)
    {
        nativeSetScale(m_ptr, scale);
        for(ISubDatasetListener l : m_listeners)
            l.onScaleEvent(this, scale);
    }

    /** Get the SubDataset name
     * @return the SubDataset name*/
    public String getName()
    {
        return nativeGetName(m_ptr);
    }

    /** Add a new annotation
     * @param annot the annotation to add*/
    public void addAnnotation(AnnotationData annot)
    {
        m_annotations.add(annot);
        for(ISubDatasetListener l : m_listeners)
            l.onAddAnnotation(this, annot);
    }

    /** Get the list of annotations
     * @return the list of annotations. Please, do not modify the list (list item can however be modified)*/
    public List<AnnotationData> getAnnotations()
    {
        return m_annotations;
    }

    /** Native code telling is this SubDataset is in a valid state
     * @param ptr the native pointer
     * @return true if in a valid state, false otherwise*/
    private native boolean nativeIsValid(long ptr);

    /** Native code to get the minimum amplitude of this SubDataset. For vectorial data, return the minimum length
     * @param ptr the native pointer
     * @return the minimum amplitude found in this SubDataset*/
    private native float nativeGetMinAmplitude(long ptr);

    /** Native code to get the maximum amplitude of this SubDataset. For vectorial data, return the maximum length
     * @param ptr the native pointer
     * @return the maximum amplitude found in this SubDataset*/
    private native float nativeGetMaxAmplitude(long ptr);

    /** Get the current minimum clamping color of this SubDataset
     * @param ptr  the native pointer
     * @return the current minimum clamping color  being displayed*/
    private native float nativeGetMinClampingColor(long ptr);

    /** Get the current maximum clamping color of this SubDataset
     * @param ptr  the native pointer
     * @return the current maximum clamping color  being displayed*/
    private native float nativeGetMaxClampingColor(long ptr);

    /** Get the current color mode of this SubDataset
     * @param ptr  the native pointer
     * @return the current color mode being displayed*/
    private native int nativeGetColorMode(long ptr);

    /** Native code to get a snapshot of this SubDataset in the main rendering object
     * @param ptr the native pointer
     * @return the snapshot of the subdataset*/
    private native Bitmap nativeGetSnapshot(long ptr);

    /** Get the rotation quaternion components. In order: w, i, j, k
     * @param ptr the native pointer
     * @return the rotation quaternion components*/
    private native float[] nativeGetRotation(long ptr);

    /** Set the rotation quaternion components. In order: w, i, j, k
     * @param ptr the native pointer
     * @param q the quaternion rotation*/
    private native void nativeSetRotation(long ptr, float[] q);

    /** Get the 3D position components. In order: x, y, z
     * @param ptr the native pointer
     * @return the 3D position vector*/
    private native float[] nativeGetPosition(long ptr);


    /** Set the 3D position components. In order: x, y, z
     * @param ptr the native pointer
     * @param p the 3D position vector*/
    private native void nativeSetPosition(long ptr, float[] p);

    /** Get the 3D scale components. In order: x, y, z
     * @param ptr the native pointer
     * @return the 3D scale vector*/
    private native float[] nativeGetScale(long ptr);

    /** Set the 3D scale components. In order: x, y, z
     * @param ptr the native pointer
     * @param p the 3D scale vector*/
    private native void nativeSetScale(long ptr, float[] s);

    /** Native code to set the range color of this SubDataset being displayed
     * @param ptr the native pointer
     * @param min the minimum (between 0.0 and 1.0) value to display. Values lower than min will be discarded
     * @param max the maximum (between 0.0 and 1.0) value to display. Values greater than max will be discarded
     * @param mode the ColorMode to apply*/
    private native void nativeSetRangeColor(long ptr, float min, float max, int mode);

    /** Native code to get the SubDataset name
     * @param ptr the native pointer
     * @return the SubDataset name*/
    private native String nativeGetName(long ptr);
}