package com.sereno.vfs.Data;

import java.io.File;

/* \brief Fluid dataset representation. Contains only the data, not the representation*/
public class FluidDataset
{
    /* \brief the C++ ptr of this dataset*/
    private long m_ptr = 0;

    /*\brief Constructor
     * Read the data contained in file.
     * \param file the file to read at*/
    public FluidDataset(File file)
    {
        this(file.getAbsolutePath());
    }

    /*\brief Constructor
     * Read the data contained in file.
     * \param path the path to read at*/
    public FluidDataset(String path)
    {
        m_ptr = nativeInitPtr(path);
    }

    /* \brief Get the 3D grid size of this dataset
     * \return the 3D grid size*/
    public int[] getSize()
    {
        return nativeGetSize(m_ptr);
    }

    /* \brief native function. Initialized the C++ pointer of this object
     * \param path the path of the dataset to read at
     * \return the C++ ptr as a long*/
    private native long nativeInitPtr(String path);

    /* \brief Get the 3D grid size of this dataset
     * \param ptr the C++ ptr generated using initPtr
     * \return the 3D grid size*/
    private native int[] nativeGetSize(long ptr);
}