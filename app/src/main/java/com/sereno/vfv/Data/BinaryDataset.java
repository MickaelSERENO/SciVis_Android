package com.sereno.vfv.Data;

import java.io.File;

/* \brief Fluid dataset representation. Contains only the data, not the representation*/
public class BinaryDataset extends Dataset
{
    /**\brief Constructor
     * Read the data contained in file.
     * @param file the file to read at*/
    public BinaryDataset(File file)
    {
        this(file.getAbsolutePath());
    }

    /**\brief Constructor
     * Read the data contained in file.
     * @param path the path to read at*/
    public BinaryDataset(String path)
    {
        super(nativeInitPtr(path));
    }

    @Override
    public void finalize() throws Throwable
    {
        super.finalize();
    }

    /** \brief Get the 3D grid size of this dataset
     * \return the 3D grid size*/
    public int[] getSize()
    {
        return nativeGetSize(m_ptr);
    }

    /** \brief Get the C++ pointer handler of the BinaryDataset
     * @param m_ptr the pointer handler*/
    public long getPtr() {return m_ptr;}

    /** \brief native function. Initialized the C++ pointer of this object
     * @param path the path of the dataset to read at
     * \return the C++ ptr as a long*/
    private native static long nativeInitPtr(String path);

    /** \brief Get the 3D grid size of the dataset index idx
     * @param ptr the C++ ptr generated using initPtr
     * @param idx the dataset index
     * \return the 3D grid size*/
    private native static int[] nativeGetSize(long ptr);
}