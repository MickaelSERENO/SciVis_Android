#include "Graphics/GLRenderer.h"

namespace sereno
{
    GLRenderer::GLRenderer() : Render()
    {
    }

    GLRenderer::~GLRenderer()
    {
        internalEglDestroySurface();
        eglDestroyContext(m_disp, m_context);
        eglTerminate(m_disp);
    }

    void GLRenderer::internalEglDestroySurface()
    {
        if(m_surface != EGL_NO_SURFACE)
        {
            eglMakeCurrent(m_disp, EGL_NO_SURFACE, EGL_NO_SURFACE, m_context);
            eglDestroySurface(m_disp, m_surface);
            m_surface = EGL_NO_SURFACE;
        }
    }

    void GLRenderer::destroySurface()
    {
        m_destroy = true;
    }

    void GLRenderer::createSurface(ANativeWindow* nativeWindow)
    {
        m_recreateWindow = nativeWindow;
        m_recreate       = true;
    }

    void GLRenderer::internalCreateSurface(ANativeWindow* nativeWindow)
    {
        //Recreate the surface
        if(nativeWindow == NULL)
            return;

        m_window = nativeWindow;
        ANativeWindow_setBuffersGeometry(m_window, 0, 0, m_format);

        if(!(m_surface = eglCreateWindowSurface(m_disp, m_conf, m_window, 0)))
        {
            LOG_ERROR("Can't create an EGL surface from this window. Error : %d", eglGetError());
            return;
        }

        if(!eglMakeCurrent(m_disp, m_surface, m_surface, m_context))
        {
            LOG_ERROR("Can't make this surface current. Error : %d", eglGetError());
            return;
        }

        eglQuerySurface(m_disp, m_surface, EGL_WIDTH, &m_width);
        eglQuerySurface(m_disp, m_surface, EGL_HEIGHT, &m_height);
    }

    bool GLRenderer::initializeContext()
    {
        //Initialize the egl context
        const EGLint attribs[] = 
        {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_NONE
        };

        const EGLint eglAttribs[] =
        {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };

        if((m_disp = eglGetDisplay(EGL_DEFAULT_DISPLAY))==EGL_NO_DISPLAY)
        {
            LOG_ERROR("Can't get the default display. Error : %d", eglGetError());
            return false;
        }

        if(!eglInitialize(m_disp, NULL, NULL))
        {
            LOG_ERROR("Can't init the egl buffer. Error : %d", eglGetError());
            return false;
        }

        if(!eglChooseConfig(m_disp, attribs, &m_conf, 1, &m_nbConf))
        {
            LOG_ERROR("Can't configure the buffer. Error : %d", eglGetError());
            return false;
        }

        if(!eglGetConfigAttrib(m_disp, m_conf, EGL_NATIVE_VISUAL_ID, &m_format))
        {
            LOG_ERROR("Can't get an attributes. Error : %d", eglGetError());
            return false;
        }

        if(!(m_context = eglCreateContext(m_disp, m_conf, EGL_NO_CONTEXT, eglAttribs)))
        {
            LOG_ERROR("Can't create an EGL context. Error : %d", eglGetError());
            return false;
        }

        if(!eglMakeCurrent(m_disp, EGL_NO_SURFACE, EGL_NO_SURFACE, m_context))
        {
            LOG_ERROR("Can't bind this context. Error : %d", eglGetError());
            return false;
        }

        return true;
    }

    void GLRenderer::swapBuffers()
    {
        //Check destruction and creation of the current surface
        if(m_destroy)
        {
            internalEglDestroySurface();
            ANativeWindow_release(m_window);
            m_window = NULL;
        }

        if(m_recreate && m_recreateWindow)
        {
            internalCreateSurface(m_recreateWindow);
            m_recreateWindow = NULL;
        }

        m_destroy = m_recreate = false;

        if(m_disp != EGL_NO_DISPLAY && m_surface != EGL_NO_SURFACE)
        {
            eglSwapBuffers(m_disp, m_surface);
        }
    }

    void GLRenderer::render()
    {
        eglMakeCurrent(m_disp, m_surface, m_surface, m_context);
        swapBuffers();
    }
}
