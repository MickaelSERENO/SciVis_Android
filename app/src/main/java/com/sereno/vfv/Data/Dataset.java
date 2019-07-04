package com.sereno.vfv.Data;

import java.util.ArrayList;

/** The Dataset abstract class*/
public abstract class Dataset
{
    /** The native C++ handle*/
    protected long m_ptr;

    /** The Java name of this Dataset*/
    protected String m_name;

    /** The sub datasets of this object*/
    protected ArrayList<SubDataset> m_subDatasets = new ArrayList<>();

    /** The current ID of this Dataset*/
    protected int m_id = -1;

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
     * @param i the SubDataset number #i
     * @return the SubDataset object. Null if i is invalid*/
    public SubDataset getSubDataset(int i)
    {
        return m_subDatasets.get(i);
    }

    /** Get the list of SubDataset this Dataset is bound to
     * @return an ArrayList of SubDataset*/
    public ArrayList<SubDataset> getSubDatasets() {return m_subDatasets;}

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
        sd.inRemoving();
        m_subDatasets.remove(sd);
        nativeRemoveSubDataset(m_ptr, sd.getNativePtr());
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
}
