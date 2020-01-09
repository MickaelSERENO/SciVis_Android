package com.sereno.vfv.Data;

import android.graphics.Bitmap;
import android.graphics.PointF;

import com.sereno.color.ColorMode;
import com.sereno.gl.VFVSurfaceView;
import com.sereno.view.AnnotationData;
import com.sereno.view.GTFData;

import java.lang.annotation.Annotation;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class SubDataset
{
    public static final int VISIBILITY_PUBLIC  = 1;
    public static final int VISIBILITY_PRIVATE = 0;

    public static final int TRANSFER_FUNCTION_NONE = 0;
    public static final int TRANSFER_FUNCTION_GTF  = 1;
    public static final int TRANSFER_FUNCTION_TGTF = 2;

    /** Callback interface called when the SubDataset is modified*/
    public interface ISubDatasetListener
    {
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

        /** Method called when a SubDataset is being removed
         * @param dataset the subdataset being removed*/
        void onRemove(SubDataset dataset);

        /** Method called when an annotation is being removed from the SubDataset
         * @param dataset the dataset bound to the annotation
         * @param annotation the annotation being removed*/
        void onRemoveAnnotation(SubDataset dataset, AnnotationData annotation);

        /** Method called when the SubDataset transfer function has been updated
         * @param dataset the dataset that has been updated*/
        void onUpdateTF(SubDataset dataset);

        /** Method called when the headset ID locking this SubDataset has changed
         * @param dataset the dataset calling this method
         * @param headsetID the new headset ID. -1 == no headset ID*/
        void onSetCurrentHeadset(SubDataset dataset, int headsetID);

        /** Method called when the modificability status of this SubDataset has changed
         * @param dataset the dataset calling this method
         * @param status true if the subdataset can be modified, false otherwise*/
        void onSetCanBeModified(SubDataset dataset, boolean status);
    }

    /** The native C++ handle*/
    protected long m_ptr;

    /** The parent dataset*/
    private Dataset m_parent;

    /** List of listeners bound to this SubDataset*/
    private List<ISubDatasetListener> m_listeners = new ArrayList<>();

    /** List of annotations bound to this SubDataset*/
    private List<AnnotationData> m_annotations = new ArrayList<>();

    /** The current headset owning this subdataset. -1 == public subDataset*/
    private int m_ownerHeadsetID = -1;

    /** The current headset modifying this subdataset. -1 == no one is modifying this subdataset*/
    private int m_currentHeadsetID = -1;

    /** The type of the subdataset current transfer function*/
    private int m_tfType;

    /** Tells whether this application can modify or not this SubDataset*/
    private boolean m_canBeModified = true;

    /** Constructor. Link the Java object with the C++ native SubDataset object
     * @param ptr the native C++ pointer
     * @param parent the java object Dataset parent
     * @param ownerID the headset ID owning this SubDataset. -1 == public SubDataset*/
    public SubDataset(long ptr, Dataset parent, int ownerID)
    {
        m_ptr = ptr;
        m_parent = parent;
        m_ownerHeadsetID = ownerID;
        setTransferFunctionType(SubDataset.TRANSFER_FUNCTION_GTF);
    }

    @Override
    public Object clone()
    {
        if(m_ptr == 0)
            return null;
        return new SubDataset(nativeClone(m_ptr), m_parent, m_ownerHeadsetID);
    }

    /** Create a new SubDataset from basic information. It still needs to be attached to the parent afterward
     * @param parent the parent of this SubDataset
     * @param id the ID of this SubDataset
     * @param name the name of this SubDataset
     * @param ownerID the headset ID owning this SubDataset. -1 == public SubDataset
     * @return the newly created SubDataset. It is not automatically added to the parent though.*/
    static public SubDataset createNewSubDataset(Dataset parent, int id, String name, int ownerID)
    {
        return new SubDataset(nativeCreateNewSubDataset(parent.getPtr(), id, name), parent, ownerID);
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
        if(m_ptr == 0)
            return false;
        return nativeIsValid(m_ptr);
    }

    /** Get a snapshot of this SubDataset in the main rendering object
     * @return the snapshot of the subdataset*/
    public Bitmap getSnapshot()
    {
        if(m_ptr == 0)
            return null;
        return nativeGetSnapshot(m_ptr);
    }

    /** Get the rotation quaternion components. In order: w, i, j, k
     * @return the rotation quaternion components*/
    public float[] getRotation()
    {
        if(m_ptr == 0)
            return null;
        return nativeGetRotation(m_ptr);
    }

    /** Get the 3D position components. In order: x, y, z
     * @return the position quaternion components*/
    public float[] getPosition()
    {
        if(m_ptr == 0)
            return null;
        return nativeGetPosition(m_ptr);
    }

    /** Get the 3D scaling components. In order: x, y, z
     * @return the 3D scaling components*/
    public float[] getScale()
    {
        if(m_ptr == 0)
            return null;
        return nativeGetScale(m_ptr);
    }

    /** Get the native pointer of the SubDataset
     * @return the native pointer*/
    public long getNativePtr()
    {
        return m_ptr;
    }

    /** Get the current color mode of this SubDataset
     * @return the current color mode being displayed*/
    public int getColorMode()
    {
        if(m_ptr == 0)
            return ColorMode.RAINBOW;
        return nativeGetColorMode(m_ptr);
    }

    /** Function called frm C++ code when a new snapshot has been created
     * @param bmp the new bitmap snapshot*/
    public void onSnapshotEvent(Bitmap bmp)
    {
        for(int i = 0; i < m_listeners.size(); i++)
            m_listeners.get(i).onSnapshotEvent(this, bmp);
    }

    /** Set the rotation of this SubDataset
     * @param quaternion the new rotation to apply (w, x, y, z)*/
    public void setRotation(float[] quaternion)
    {
        if(m_ptr == 0)
            return;
        nativeSetRotation(m_ptr, quaternion);
        for(int i = 0; i < m_listeners.size(); i++)
            m_listeners.get(i).onRotationEvent(this, quaternion);
    }

    /** Set the position of this SubDataset
     * @param position the new position to apply*/
    public void setPosition(float[] position)
    {
        if(m_ptr == 0)
            return;
        nativeSetPosition(m_ptr, position);
        for(int i = 0; i < m_listeners.size(); i++)
            m_listeners.get(i).onPositionEvent(this, position);
    }

    /** Set the scaling of this SubDataset
     * @param scale the new scale to apply*/
    public void setScale(float[] scale)
    {
        if(m_ptr == 0)
            return;
        nativeSetScale(m_ptr, scale);
        for(int i = 0; i < m_listeners.size(); i++)
            m_listeners.get(i).onScaleEvent(this, scale);
    }

    /** Get the SubDataset name
     * @return the SubDataset name*/
    public String getName()
    {
        if(m_ptr == 0)
            return "";
        return nativeGetName(m_ptr);
    }

    /** Get the SubDataset ID
     * @return the SubDataset ID*/
    public int getID()
    {
        if(m_ptr == 0)
            return -1;
        return nativeGetID(m_ptr);
    }

    /** Get the parent dataset
     * @return the parent dataset*/
    public Dataset getParent() {return m_parent;}

    /** Get the ID of the Headset owning this SubDataset. -1 == public SubDataset
     * @return the headset ID as defined by the application (server)*/
    public int getOwnerID() {return m_ownerHeadsetID;}

    /** Get the boolean status telling whether or not this SubDataset can be modified by this client
     * @return true if yes, false otherwise. Pay attention that this variable is regarding this SubDataset states on the running server*/
    public boolean getCanBeModified() {return m_canBeModified;}

    /** Set the boolean status telling whether or not this SubDataset can be modified by this client
     * @param value true if yes, false otherwise. Pay attention that this variable is regarding this SubDataset states on the running server*/
    public void setCanBeModified(boolean value)
    {
        if (value != m_canBeModified)
        {
            m_canBeModified = value;
            for(int i = 0; i < m_listeners.size(); i++)
                m_listeners.get(i).onSetCanBeModified(this, value);
        }
    }

    /** Set the headset ID currently modifying/locking this subdataset
     * @param hmdID the headset ID to use. -1 == no owner*/
    public void setCurrentHeadset(int hmdID)
    {
        if(hmdID != m_currentHeadsetID)
        {
            m_currentHeadsetID = hmdID;
            for(int i = 0; i < m_listeners.size(); i++)
                m_listeners.get(i).onSetCurrentHeadset(this, hmdID);
        }
    }

    /** Get the headset ID currently modifying/locking this subdataset
     * @return the current headset ID locking this subdataset. -1 == no owner*/
    public int getCurrentHeadset()
    {
        return m_currentHeadsetID;
    }

    /** Get the transfer function type in use
     * @return the integer representing the transfer function type being used. See SubDataset.TRANSFER_FUNCTION_**/
    public int getTransferFunctionType()
    {
        return m_tfType;
    }

    /** Set the transfer function type in use
     * @return the transfer function type to use*/
    public void setTransferFunctionType(int tfType)
    {
        if(m_tfType != tfType)
        {
            nativeSetTFType(m_ptr, tfType);

            m_tfType = tfType;

            for (int j = 0; j < m_listeners.size(); j++)
                m_listeners.get(j).onUpdateTF(this);
        }
    }

    /** Update the Gaussian Transfer Function ranges
     * @param ranges the ranges to use. Key == point field ID. Values should be normalized.*/
    public void setGTFRanges(HashMap<Integer, GTFData.GTFPoint> ranges)
    {
        //Parse the HashMap in a easy-to-send data to C++
        int[]   pIDs    = new int[ranges.size()];
        float[] centers = new float[ranges.size()];
        float[] scales = new float[ranges.size()];
        int i = 0;
        for (Integer pID : ranges.keySet())
        {
            pIDs[i] = pID;
            centers[i] = ranges.get(pID).center;
            scales[i]  = ranges.get(pID).scale;
            i++;
        }

        //Update in C++
        nativeSetGTFRanges(m_ptr, m_tfType, pIDs, centers, scales);

        //Call listeners
        for(int j = 0; j < m_listeners.size(); j++)
            m_listeners.get(j).onUpdateTF(this);
    }

    /** Set the color mode of the SubDataset
     * @param mode the new color mode*/
    public void setColorMode(int mode)
    {
        nativeSetColorMode(m_ptr, mode);

        //Call listeners
        for(int j = 0; j < m_listeners.size(); j++)
            m_listeners.get(j).onUpdateTF(this);
    }

    /** Add a new annotation
     * @param annot the annotation to add*/
    public void addAnnotation(AnnotationData annot)
    {
        m_annotations.add(annot);
        for(int i = 0; i < m_listeners.size(); i++)
            m_listeners.get(i).onAddAnnotation(this, annot);
    }

    /** Get the list of annotations
     * @return the list of annotations. Please, do not modify the list (list item can however be modified)*/
    public List<AnnotationData> getAnnotations()
    {
        return m_annotations;
    }

    /** unlink the SubDataset*/
    public void inRemoving()
    {
        /* Remove the annotation*/
        while(m_annotations.size() > 0)
            removeAnnotation(m_annotations.get(m_annotations.size()-1));

        Object[] listeners = m_listeners.toArray(); //Do a copy because on "onRemove", objects may want to get removed from the list of listeners

        for(int i = 0; i < listeners.length; i++)
            ((ISubDatasetListener)listeners[i]).onRemove(this);

        m_ptr = 0;
    }

    /** Remove an annotation from this SubDataset
     * @param annot the annotation to remove. This function does nothing if the annotation cannot be found*/
    public void removeAnnotation(AnnotationData annot)
    {
        if(m_annotations.contains(annot))
        {
            for(int i = 0; i < m_listeners.size(); i++)
                m_listeners.get(i).onRemoveAnnotation(this, annot);
            m_annotations.remove(annot);
        }
    }

    /** Free the internal data. Do that only on CLONED SubDataset*/
    public void free()
    {
        if(m_ptr == 0)
            return;
        nativeDelPtr(m_ptr);
    }

    /** Create a new SubDataset native C++ ptr
     * @param datasetPtr the dataset native C++ pointer
     * @param id the SubDataset ID
     * @param name the SubDataset name*/
    private static native long nativeCreateNewSubDataset(long datasetPtr, int id, String name);

    /** Free the resources allocated for the native C++ pointer data
     * @param ptr the native C++ pointer data to free*/
    private native void nativeDelPtr(long ptr);

    /** Native code clonning a current SubDataset
     * @param ptr the native pointer to clone
     * @return the new native pointer cloned*/
    private native int nativeClone(long ptr);

    /** Native code telling is this SubDataset is in a valid state
     * @param ptr the native pointer
     * @return true if in a valid state, false otherwise*/
    private native boolean nativeIsValid(long ptr);

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
     * @param s the 3D scale vector*/
    private native void nativeSetScale(long ptr, float[] s);

    /** Native code to set the clamping of this SubDataset being displayed
     * @param ptr the native pointer
     * @param min the minimum (between 0.0 and 1.0) value to display. Values lower than min will be discarded
     * @param max the maximum (between 0.0 and 1.0) value to display. Values greater than max will be discarded*/
    private native void nativeSetClamping(long ptr, float min, float max);

    /** Set the transfer function type of the native C++ SD object
     * @param ptr the native pointer
     * @param tfType the new transfer function type*/
    private native void nativeSetTFType(long ptr, int tfType);

    /** Native code to set the Gaussian Transfer Function ranges
     * pIDs, minVals, and maxVals should be coherent (same size and correspond to each one)
     * @param ptr the native pointer
     * @param tfType the transfer function type
     * @param pIDs the list of pIDs
     * @param centers the list of centers normalized
     * @param scales the list of scales normalized*/
    private native void nativeSetGTFRanges(long ptr, int tfType, int[] pIDs, float[] centers, float[] scales);

    /** Native code to set the color mode applied to this SubDataset
     * @param ptr the native pointer
     * @param mode the new color mode to apply*/
    private native void nativeSetColorMode(long ptr, int mode);

    /** Native code to get the SubDataset name
     * @param ptr the native pointer
     * @return the SubDataset name*/
    private native String nativeGetName(long ptr);

    /** Native code to get the SubDataset ID
     * @param ptr the native pointer
     * @return the SubDataset id*/
    private native int nativeGetID(long ptr);
}