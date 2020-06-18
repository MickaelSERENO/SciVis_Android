package com.sereno.vfv.Data;

import java.io.File;

public class CloudPointDataset extends Dataset
{
    /**\brief Constructor
     * Read the data contained in file.
     * @param file the file to read at*/
    public CloudPointDataset(File file)
    {
        this(file.getAbsolutePath(), file.getName());
    }

    /**\brief Constructor
     * Read the data contained in file.
     * @param path the path to read at
     * @param name  the java name of this Dataset*/
    public CloudPointDataset(String path, String name)
    {
        super(nativeInitPtr(path), name);
    }

    @Override
    public void finalize() throws Throwable
    {
        super.finalize();
    }

    /** \brief native function. Initialized the C++ pointer of this object
     * @param path the path of the dataset to read at
     * \return the C++ ptr as a long*/
    private native static long nativeInitPtr(String path);
}
