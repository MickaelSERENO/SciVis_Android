package com.sereno.gl;

import android.content.Context;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/** \brief The GLSurface object, represent an OpenGL Surface associated with an OpenGL Context*/
public class GLSurfaceView extends SurfaceView implements SurfaceHolder.Callback, Runnable
{
    /** \brief Represents the possible InternalState of this GLSurface*/
    public enum InternalState
    {
        SURFACE_NOT_INITIALIZED,
        SURFACE_CREATED,
        SURFACE_CHANGED,
        SURFACE_DESTROYED
    }

    protected Thread        m_thread        = null;                                  /**!< Represents the Drawing thread*/
    protected Boolean       m_isCreated     = false;                                 /**!< Is the surface created ?*/
    protected InternalState m_internalState = InternalState.SURFACE_NOT_INITIALIZED; /**!< The InternalState of the application*/
    protected long          m_internalData  = 0;                                     /**!< Pointer to a C++ object permitting to send data to the main thread*/

    /** \brief Initialize the view*/
    public void init(Context context)
    {
        getHolder().addCallback(this);
        getHolder().setFormat(PixelFormat.RGBA_8888);
        m_internalData = nativeInitInternalData(context.getExternalFilesDir(null).getPath());
    }

    public GLSurfaceView(Context context)
    {
        super(context);
        init(context);
    }

    public GLSurfaceView(Context context, AttributeSet attrs) 
    {
        super(context, attrs);
        init(context);
    }

    public GLSurfaceView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        init(context);
    }

    /** \brief Finalize method. Close the thread*/
    public void finalize() throws Throwable
    {
        try
        {
            if(m_thread != null)
                m_thread.join();
        }
        catch(InterruptedException e)
        {

        }
        finally
        {
            super.finalize();
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder)
    {
        setInternalState(InternalState.SURFACE_CREATED);
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int format, int width, int height)
    {
        nativeOnSurfaceChanged(m_internalData, format, width, height);
        setInternalState(InternalState.SURFACE_CHANGED);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder)
    {
        setInternalState(InternalState.SURFACE_DESTROYED);
    }

    @Override
    public void run()
    {
        String mainLibrary = getMainLibrary();
        try
        {
            System.loadLibrary(mainLibrary);
        }catch(Exception e)
        {}

        nativeMain(m_internalData, mainLibrary, getMainFunction(), getMainArgument());
    }

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        int width  = getWidth();
        int height = getHeight();

        for(int i = 0; i < event.getPointerCount(); i++)
        {
            int action = 0;
            switch(event.getAction() & MotionEvent.ACTION_MASK)
            {
                case MotionEvent.ACTION_DOWN:
                    action = 0;
                    break;
                case MotionEvent.ACTION_UP:
                    action = 1;
                    break;
                case MotionEvent.ACTION_MOVE:
                    action = 2;
                    break;
            }
            int pID = event.getPointerId(i);
            float x = 2*event.getX(pID) / width - 1;
            float y = -2*event.getY(pID) / height + 1;

            nativeOnTouchEvent(m_internalData, action, pID, x, y);
        }
        super.onTouchEvent(event);
        return true;
    }

    protected String getMainLibrary()
    {
        return "native-lib";
    }

    /** \brief Function which has for aim to be overrided. Defines the main function for this SurfaceView
     * \return the name of the main function */
    protected String getMainFunction()
    {
        return "GLSurface_main";
    }

    /** \brief Function which has for aim to be overrided.
     * \return the cpp argument to send to the main function*/
    protected long getMainArgument()
    {
        return 0;
    }

    /** \brief Set the internal state of this object
     * @param state the new internal state*/
    private void setInternalState(InternalState state)
    {
        if(state == m_internalState)
            return;


        switch(state)
        {
            case SURFACE_CREATED:
            {
                if(m_internalState == InternalState.SURFACE_NOT_INITIALIZED)
                {
                    m_thread = new Thread(this);
                    m_thread.setName("NativeThread - getMainFunction()");
                    m_thread.start();
                    m_isCreated = true;
                }
                nativeOnSurfaceCreated(m_internalData, getHolder().getSurface());
                break;
            }

            case SURFACE_DESTROYED:
            {
                nativeOnSurfaceDestroyed(m_internalData);
                break;
            }
        }
        m_internalState = state;
    }

    /** \brief Permit to init the internal data of this view. This data will mostly contain events sent from Java
     * \return C++ pointer*/
    private native long nativeInitInternalData(String internalData);

    /** \brief Main C++ function being called
     * @param data the internal data to pass on
     * @param mainLibrary the main library to read containing the main function
     * @param mainFunc the name of the mainFunction. Must have the following proptotype : void mainFunc(long data, long arg)
     * @param arg the argument to pass*/
    private native void nativeMain(long data, String mainLibrary, String mainFunc, long arg);

    /** \brief OnSurfaceDestroyed handled in C++
     * @param data the C++ internal data*/
    private native void nativeOnSurfaceDestroyed(long data);

    /** \brief OnCreateSurface handled in C++
     * @param data the C++ internal data*/
    private native void nativeOnSurfaceCreated(long data, Surface surface);

    /** \brief OnSurfaceChanged handled in C++
     * @param data the C++ internal data
     * @param format the new surface format
     * @param width the new surface width
     * @param height the new surface height*/
    private native void nativeOnSurfaceChanged(long data, int format, int width, int height);

    /** \brief ONTouchEvent handled in C++. Send the event in the C++ application. x and y are in OpenGL coordinate system
     * @param data the C++ internal data pointer
     * @param action the action to use. 0 == DOWN, 1 == UP, 2 == MOVE
     * @param finger the finger ID
     * @param x the x position [-1, +1]
     * @param y the y position [-1, +1].*/
    private native void nativeOnTouchEvent(long data, int action, int finger, float x, float y);

    static
    {
        System.loadLibrary("native-lib");
    }
}
