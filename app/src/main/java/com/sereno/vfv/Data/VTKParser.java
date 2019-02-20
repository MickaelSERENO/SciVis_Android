package com.sereno.vfv.Data;

import java.io.File;

public class VTKParser
{
    /** Pointer to the native handle of the VTKParser C++ object*/
    private long m_ptr;

    /** Constructor. Parse the VTK file object defined by f
     * @param f the VTK file to open*/
    public VTKParser(File f) {this(f.getAbsolutePath());}

    /** Constructor. Parse the VTK file object defined by f
     * @param path the VTK file path to open*/
    public VTKParser(String path)
    {
        m_ptr = nativeInitPtr(path);
    }

    @Override
    protected void finalize() throws Throwable
    {
        nativeDelPtr(m_ptr);
        super.finalize();
    }

    /** Get the point field values descriptors
     * @return an array of VTKFieldValue*/
    public VTKFieldValue[] getPointFieldValues()
    {
        return createVTKFieldValueFromHandles(nativeGetPointFieldValues(m_ptr));
    }

    /** Get the cell field values descriptors
     * @return an array of VTKFieldValue*/
    public VTKFieldValue[] getCellFieldValues()
    {
        return createVTKFieldValueFromHandles(nativeGetCellFieldValues(m_ptr));
    }

    /** @brief Get the native pointer of the VTK parser object
     * @return the native C/C++ Pointer*/
    public long getPtr()
    {
        return m_ptr;
    }

    /** Create Java VTKFieldValue array from C++ native handles array of VTKFieldValue*
     * @param nativeFieldValues the array of the C++ native handles
     * @return the Java VTKFieldValue array*/
    private VTKFieldValue[] createVTKFieldValueFromHandles(long[] nativeFieldValues)
    {
        VTKFieldValue[] values   = new VTKFieldValue[nativeFieldValues.length];

        for(int i = 0; i < nativeFieldValues.length; i++)
            values[i] = new VTKFieldValue(nativeFieldValues[i]);

        return values;
    }

    /** Create a native handle C++ pointer for the VTKParser object
     * @param path the path to open
     * @return the C++ pointer*/
    private static native long nativeInitPtr(String path);

    /** Delete the native C++ handle
     * @param ptr the C++ handle*/
    private static native void nativeDelPtr(long ptr);

    /** Get the native C++ handles for the VTKFieldValue regarding point data
     * @param ptr the native C++ VTKParser handle
     * @return an array of VTKFieldValue C++ handles*/
    private static native long[] nativeGetPointFieldValues(long ptr);

    /** Get the native C++ handles for the VTKFieldValue regarding cell data
     * @param ptr the native C++ VTKParser handle
     * @return an array of VTKFieldValue C++ handles*/
    private static native long[] nativeGetCellFieldValues(long ptr);
}
