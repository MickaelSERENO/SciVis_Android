package com.sereno.vfv.Data;

/** The representation of the VTKFieldValue C++ object*/
public class VTKFieldValue
{
    /** The C++ pointer handle to a VTKFieldValue*/
    private long m_ptr;

    /** The name of the field value*/
    private String m_name;

    /** Constructor, initialize the VTKFieldValue with a C++ VTKFieldValue object
     * @param ptr  the C++ pointer to a VTKFieldValue object*/
    public VTKFieldValue(long ptr)
    {
        m_ptr  = ptr;
        m_name = nativeGetName(ptr);
    }

    /** Get the native C++ pointer (type : VTKFieldValue*)
     * @return the native C++ pointer*/
    public long   getPtr()  {return m_ptr;}

    /** Get the name of this field value
     * @return the name of the field value*/
    public String getName() {return m_name;}

    /** Get the name of the VTKFieldValue object
     * @param ptr the native handles
     * @return the name*/
    private static native String nativeGetName(long ptr);
}