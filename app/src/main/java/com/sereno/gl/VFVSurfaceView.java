package com.sereno.gl;

import android.content.Context;
import android.util.AttributeSet;

public class VFVSurfaceView extends GLSurfaceView
{
    public VFVSurfaceView(Context context)
    {
        super(context);
    }

    public VFVSurfaceView(Context context, AttributeSet attrs)
    {
        super(context, attrs);
    }

    public VFVSurfaceView(Context context, AttributeSet attrs, int defStyle)
    {
        super(context, attrs, defStyle);
    }

    /* \brief Function which has for aim to be overrided.
     * \return the cpp argument to send to the main function*/
    @Override
    protected long getMainArgument()
    {
        return nativeCreateMainArgs();
    }

    /* \brief Create the argument to send to the main function
     * \return the main argument as a ptr (long value)*/
    private native long nativeCreateMainArgs();
}
