package com.sereno.vfv.Event;

/** NativeEvent class. Handles event received from native code
 * Following the event ID, this object must be converted to another event type*/
public class NativeEvent
{
    /** The native pointer*/
    private long m_ptr = 0;

    /** Constructor. Link java to native code
     * @param ptr the native pointer received.*/
    public NativeEvent(long ptr)
    {
        m_ptr = ptr;
    }

    @Override
    protected void finalize() throws Throwable
    {
        nativeDelete(m_ptr);
        super.finalize();
    }

    /** Delete the native event
     * @param nativePtr the native pointer to delete*/
    static native void nativeDelete(long nativePtr);

    /** Get the event ID of the native code
     * @param nativePtr the native pointer holding the data*/
    static native long nativeGetEventID(long nativePtr);
}
