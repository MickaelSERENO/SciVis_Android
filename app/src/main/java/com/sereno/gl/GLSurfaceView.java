package com.sereno.gl;

import android.content.Context;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.util.ArrayList;

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

    public interface GLSurfaceViewListener
    {
        /** Function called when the surface of the GL View changed
         * @param view the surface view calling this method
         * @param format the new surface view format
         * @param width the surface view width to use
         * @param height the surface view height to use*/
        void onSurfaceChanged(GLSurfaceView view, int format, int width, int height);

        /** Function called when the surface of the GL View is destroyed
         * @param view the surface view calling this method*/
        void onSurfaceDestroyed(GLSurfaceView view);

        /** Function called when the surface of the GL View is created
         * @param view the surface view calling this method*/
        void onSurfaceCreated(GLSurfaceView view);
    }

    protected Thread        m_thread        = null;                                  /**!< Represents the Drawing thread*/
    protected Boolean       m_isCreated     = false;                                 /**!< Is the surface created ?*/
    protected InternalState m_internalState = InternalState.SURFACE_NOT_INITIALIZED; /**!< The InternalState of the application*/
    protected long          m_internalData  = 0;
    private   ArrayList<GLSurfaceViewListener> m_listeners = new ArrayList<>();      /**!< The listener to use*/

    /**!< Pointer to a C++ object permitting to send data to the main thread*/

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

    /** Add a GLSurfaceViewListener to call during events
     * @param listener the listener to add*/
    public void addListener(GLSurfaceViewListener listener)
    {
        if(!m_listeners.contains(listener))
            m_listeners.add(listener);
    }


    /** Remove an already registered GLSurfaceViewListener to call during events
     * @param listener the listener to remove*/
    public void removeListener(GLSurfaceViewListener listener)
    {
        m_listeners.remove(listener);
    }

    /** \brief Finalize method. Close the thread*/
    protected void finalize() throws Throwable
    {
        try
        {
            if(m_thread != null)
            {
                nativeCloseEvent(m_internalData);
                m_thread.join();
            }
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

        for(GLSurfaceViewListener l : m_listeners)
            l.onSurfaceCreated(this);
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int format, int width, int height)
    {
        nativeOnSurfaceChanged(m_internalData, format, width, height);
        setInternalState(InternalState.SURFACE_CHANGED);

        for(GLSurfaceViewListener l : m_listeners)
            l.onSurfaceChanged(this, format, width, height);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder)
    {
        setInternalState(InternalState.SURFACE_DESTROYED);

        for(GLSurfaceViewListener l : m_listeners)
            l.onSurfaceDestroyed(this);
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

        nativeMain(m_internalData, mainLibrary, getMainFunction(), getHolder().getSurface(), getMainArgument());
    }

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        int width  = getWidth();
        int height = getHeight();

        int pID = event.getActionIndex();
        int action;
        switch(event.getActionMasked())
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
            case MotionEvent.ACTION_POINTER_DOWN:
                action = 0;
                pID = event.getActionIndex();
                break;
            case MotionEvent.ACTION_POINTER_UP:
                action = 1;
                pID = event.getActionIndex();
                break;
            default:
                super.onTouchEvent(event);
                return true;
        }

        if(action != 2)
        {
            try
            {
                float x = 2 * event.getX(pID) / width - 1;
                float y = -2 * event.getY(pID) / height + 1;
                nativeOnTouchEvent(m_internalData, action, event.getPointerId(pID), x, y);
            }
            catch(Exception e){}
        }
        else
        {
            for(int i = 0; i < event.getPointerCount(); i++)
            {
                try
                {
                    pID = i;
                    float x = 2 * event.getX(pID) / width - 1;
                    float y = -2 * event.getY(pID) / height + 1;
                    nativeOnTouchEvent(m_internalData, action, event.getPointerId(pID), x, y);
                }
                catch(Exception e){}
            }
        }

        super.onTouchEvent(event);
        return true;
    }

    public void setRenderVisibility(int visibility)
    {
        if(visibility == GONE || visibility == INVISIBLE)
            nativeOnHidden(m_internalData);
        else
            nativeOnVisible(m_internalData);
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
                else
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

    /** @brief Close the rendering thread
     * @param data the native pointer object*/
    private native void nativeCloseEvent(long data);

    /** \brief Main C++ function being called
     * @param data the internal data to pass on
     * @param mainLibrary the main library to read containing the main function
     * @param mainFunc the name of the mainFunction. Must have the following proptotype : void mainFunc(long data, long arg)
     * @param arg the argument to pass*/
    private native void nativeMain(long data, String mainLibrary, String mainFunc, Surface surface, long arg);

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

    /** Native function called when this view is gone / invisible
     * @param data the C++ internal data pointer*/
    private native void nativeOnHidden(long data);

    /** Native function called when this view is visible
     * @param data the C++ internal data pointer*/
    private native void nativeOnVisible(long data);

    static
    {
        System.loadLibrary("native-lib");
    }
}
