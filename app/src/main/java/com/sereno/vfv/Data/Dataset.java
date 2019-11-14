package com.sereno.vfv.Data;

import android.graphics.Bitmap;

import com.sereno.gl.VFVSurfaceView;

import java.util.ArrayList;

/** The Dataset abstract class*/
public abstract class Dataset
{
    /** Interface Listener for basic operation on Dataset objects*/
    public interface IDatasetListener
    {
        /** Method called just before a SubDataset is getting removed
         * @param dataset the Dataset calling this method
         * @param sd the SubDataset getting removed*/
        void onRemoveSubDataset(Dataset dataset, SubDataset sd);

        /** Method called after having added a new SubDataset
         * @param dataset the Dataset calling this method
         * @param sd the SubDataset being added*/
        void onAddSubDataset(Dataset dataset, SubDataset sd);

        /** Method called when a Dataset has been loaded asynchronously
         * @param dataset the Dataset that has been loaded
         * @param success the result of the loading. True on success, False on failure*/
        void onLoadDataset(Dataset dataset, boolean success);

        /** Method called when a Dataset has loaded a new CPCPTexture asynchronously
         * @param dataset the Dataset that has loaded the CPCPTexture
         * @param texture the CPCPTexture generated*/
        void onLoadCPCPTexture(Dataset dataset, CPCPTexture texture);
    }

    /** The native C++ handle*/
    protected long m_ptr;

    /** The Java name of this Dataset*/
    protected String m_name;

    /** The sub datasets of this object*/
    protected ArrayList<SubDataset> m_subDatasets = new ArrayList<>();

    /** The current ID of this Dataset*/
    protected int m_id = -1;

    /** The Listener to calls on operation on this Dataset*/
    protected ArrayList<IDatasetListener> m_listeners = new ArrayList<>();

    /** All the generated CPCPTexture*/
    protected ArrayList<CPCPTexture> m_cpcpTextures = new ArrayList<>();

    /** Has this Dataset finished to load?*/
    protected boolean m_isLoaded = false;

    /** The constructor. Private because the class is abstract
     * @param ptr the native C++ handle created in derived class. The handle must inherits from Dataset C++ object
     * @param name  the java name of this Dataset*/
    protected Dataset(long ptr, String name)
    {
        m_ptr  = ptr;
        m_name = name;

        for(int i = 0; i < nativeGetNbSubDatasets(m_ptr); i++)
            m_subDatasets.add(new SubDataset(nativeGetSubDataset(m_ptr, i), this));
    }

    @Override
    public void finalize() throws Throwable
    {
        m_ptr = 0;
        nativeDelPtr(m_ptr);
        super.finalize();
    }

    /** Add a new callback Listener
     * @param listener the listener to call for new events*/
    public void addListener(IDatasetListener listener)
    {
        m_listeners.add(listener);
    }

    /** Remove an old callback Listener
     * @param listener the listener to remove from the list*/
    public void removeListener(IDatasetListener listener)
    {
        m_listeners.remove(listener);
    }

    /** @brief Get the C++ pointer handler of the BinaryDataset
     * @return the native pointer handler*/
    public long getPtr() {return m_ptr;}

    /** @brief The java name given to this dataset
     * @return the name caracterizing the dataset (e.g., the dataset path)*/
    public String getName() {return m_name;}

    /** @brief Get the number of sub dataset this dataset is bound to
     * @return the number of sub datasets*/
    public int getNbSubDataset() {return m_subDatasets.size();}

    /** @brief Get the SubDataset at indice i
     * @param i the SubDataset ID #i
     * @return the SubDataset object. Null if i is invalid*/
    public SubDataset getSubDataset(int i)
    {
        for(SubDataset sd : m_subDatasets)
            if(sd.getID() == i)
                return sd;
        return null;
    }

    /** Get the list of SubDataset this Dataset is bound to
     * @return an ArrayList of SubDataset*/
    public ArrayList<SubDataset> getSubDatasets() {return m_subDatasets;}

    /** Get the list of generated CPCPTexture. The list if full once isLoaded() == true
     * @return an ArrayList of CPCPTexture*/
    public ArrayList<CPCPTexture> getCPCPTextures() {return m_cpcpTextures;}

    /** @brief Get the ID of this Dataset. The ID is shared with the Server
     * @return the Dataset ID*/
    public int getID(){return m_id;}

    /** Set the ID of the Dataset. Must coincide with the Server ID*/
    public void setID(int id) {m_id = id;}

    /** Remove the subdataset 'sd' from the known subdataset
     * @param sd the subdataset to remove*/
    public void removeSubDataset(SubDataset sd)
    {
        if(!m_subDatasets.contains(sd))
            return;

        for(IDatasetListener listener : m_listeners)
            listener.onRemoveSubDataset(this, sd);
        sd.inRemoving();
        m_subDatasets.remove(sd);
        nativeRemoveSubDataset(m_ptr, sd.getNativePtr());
    }

    /** Add a SubDataset to this Dataset
     * @param sd the SubDataset to add
     * @param changeID should this function update the SubDataset ID? Please, do not mix "true" and "false" since conflicts can be created (and will not be detected)*/
    public void addSubDataset(SubDataset sd, boolean changeID)
    {
        if(m_subDatasets.contains(sd))
            return;
        nativeAddSubDataset(m_ptr, sd.getNativePtr(), changeID);
        m_subDatasets.add(sd);

        for(IDatasetListener listener : m_listeners)
            listener.onAddSubDataset(this, sd);
    }

    /** Get whether or not this Dataset has finished to load
     * @return true if yes, false otherwise*/
    public boolean isLoaded() {return m_isLoaded;}

    /** Function called from the native code when the native code has loaded values of a given Dataset
     * Pay attention that this is done asynchronously
     * @param success true on success, false on failure*/
    private void onLoadDataset(boolean success)
    {
        m_isLoaded = success;
        for(IDatasetListener l : m_listeners)
            l.onLoadDataset(this, success);
    }

    /** Method called from the native code when a Dataset has loaded a new CPCPTexture asynchronously
     * @param bitmap the bitmap generated
     * @param pIDLeft the point field ID represented on the left axis
     * @param pIDRight the point field ID represented on the right axis*/
    private void onLoadCPCPTexture(Bitmap bitmap, int pIDLeft, int pIDRight)
    {
        CPCPTexture texture = new CPCPTexture(this, bitmap, pIDLeft, pIDRight);
        m_cpcpTextures.add(texture);

        for(IDatasetListener l : m_listeners)
            l.onLoadCPCPTexture(this, texture);
    }

    /** Get the point field descriptor of this Dataset.
     * This function does recreate the array at each call, so better to store the result
     * @return an array of the point field descriptor loaded by the Dataset*/
    public PointFieldDesc[] getPointFieldDescs()
    {
        return nativeGetPointFieldDescs(m_ptr);
    }

    /** Delete a native pointer
     * @param ptr the native pointer to destroy*/
    private static native void nativeDelPtr(long ptr);

    /** @brief Get the number of sub dataset from the native C++ object
     * @param ptr the native C++ pointer
     * @return the number of sub datasets*/
    private native int nativeGetNbSubDatasets(long ptr);

    /** @brief Get the SubDataset at indice i
     * @param ptr the native C++ pointer
     * @param i the SubDataset number #i
     * @return the SubDataset native pointer. 0 if i is invalid*/
    private native long nativeGetSubDataset(long ptr, int i);

    /** Native code removing the subdataset 'sdPtr' from the C++ memory
     * @param ptr the dataset native pointer
     * @param sdPtr the subdataset native pointer*/
    private native void nativeRemoveSubDataset(long ptr, long sdPtr);

    /** Native code adding the subdataset 'sdPtr' from the C++ memory
     * @param ptr the dataset native pointer
     * @param sdPtr the subdataset native pointer
     * @param changeID should this function update the SubDataset ID? Please, do not mix "true" and "false" since conflicts can be created (and will not be detected)*/
    private native void nativeAddSubDataset(long ptr, long sdPtr, boolean changeID);

    /** Native code creating a Java array of PointFieldDesc point to this Dataset
     * @param ptr the Dataset native pointer
     * @return the newly created point field descriptors*/
    private native PointFieldDesc[] nativeGetPointFieldDescs(long ptr);
}
