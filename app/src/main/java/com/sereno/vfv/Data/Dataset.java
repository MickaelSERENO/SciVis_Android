package com.sereno.vfv.Data;

/** The Dataset abstract class*/
public abstract class Dataset
{
    /** The native C++ handle*/
    protected long m_ptr;

    /** The constructor. Private because the class is abstract
     * @param ptr the native C++ handle created in derived class. The handle must inherits from Dataset C++ object*/
    protected Dataset(long ptr)
    {
        m_ptr = ptr;
    }

    @Override
    protected void finalize() throws Throwable
    {
        m_ptr = 0;
        nativeDelPtr(m_ptr);
        super.finalize();
    }

    /** \brief Get the C++ pointer handler of the BinaryDataset
     * @param m_ptr the pointer handler*/
    public long getPtr() {return m_ptr;}

    /** Delete a native pointer
     * @param ptr the native pointer to destroy*/
    static native void nativeDelPtr(long ptr);
}
