package com.sereno.vfv.Data;

/** The Dataset abstract class*/
public abstract class Dataset
{
    /** The native C++ handle*/
    protected long m_ptr;

    /** The Java name of this Dataset*/
    protected String m_name;

    /** The constructor. Private because the class is abstract
     * @param ptr the native C++ handle created in derived class. The handle must inherits from Dataset C++ object
     * @param name  the java name of this Dataset*/
    protected Dataset(long ptr, String name)
    {
        m_ptr  = ptr;
        m_name = name;
    }

    @Override
    protected void finalize() throws Throwable
    {
        m_ptr = 0;
        nativeDelPtr(m_ptr);
        super.finalize();
    }

    /** @brief Get the C++ pointer handler of the BinaryDataset
     * @param m_ptr the pointer handler*/
    public long getPtr() {return m_ptr;}

    /** @brief The java name given to this dataset
     * @return the name caracterizing the dataset (e.g., the dataset path)*/
    public String getName() {return m_name;}

    /** @brief Get the number of sub dataset this dataset is bound to
     * @return the number of sub datasets*/
    public int getNbSubDataset() {return nativeGetNbSubDataset(m_ptr);}

    /** Delete a native pointer
     * @param ptr the native pointer to destroy*/
    private static native void nativeDelPtr(long ptr);

    /** @brief Get the number of sub dataset from the native C++ object
     * @param ptr the native C++ pointer
     * @return the number of sub datasets*/
    private native int nativeGetNbSubDataset(long ptr);
}
