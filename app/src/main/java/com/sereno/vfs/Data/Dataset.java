package com.sereno.vfs.Data;

/** The Dataset abstract class*/
public abstract class Dataset
{
    protected long m_ptr;

    protected Dataset(long ptr)
    {
        m_ptr = ptr;
    }

    @Override
    public void finalize() throws Throwable
    {
        m_ptr = 0;
        nativeDelPtr(m_ptr);
    }

    /** Delete a native pointer*/
    static native void nativeDelPtr(long ptr);
}
