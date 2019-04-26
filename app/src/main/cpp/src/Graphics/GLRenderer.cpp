#include "Graphics/GLRenderer.h"
#include "GLSurfaceViewData.h"

namespace sereno
{
    GLRenderer::GLRenderer(GLSurfaceViewData* data) : Render(), m_surfaceData(data)
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
        LOG_INFO("Destroying surface");
    }

    void GLRenderer::createSurface(ANativeWindow* nativeWindow)
    {
        m_recreateWindow = nativeWindow;
        m_recreate       = true;
    }

    void GLRenderer::printEGLConfig() const
    {
        #define X(VAL) {VAL, #VAL}
            struct {EGLint attribute; const char* name;} names[] = {
            X(EGL_BUFFER_SIZE),
            X(EGL_ALPHA_SIZE),
            X(EGL_BLUE_SIZE),
            X(EGL_GREEN_SIZE),
            X(EGL_RED_SIZE),
            X(EGL_DEPTH_SIZE),
            X(EGL_STENCIL_SIZE),
            X(EGL_CONFIG_CAVEAT),
            X(EGL_CONFIG_ID),
            X(EGL_LEVEL),
            X(EGL_MAX_PBUFFER_HEIGHT),
            X(EGL_MAX_PBUFFER_PIXELS),
            X(EGL_MAX_PBUFFER_WIDTH),
            X(EGL_NATIVE_RENDERABLE),
            X(EGL_NATIVE_VISUAL_ID),
            X(EGL_NATIVE_VISUAL_TYPE),
            X(EGL_SAMPLES),
            X(EGL_SAMPLE_BUFFERS),
            X(EGL_SURFACE_TYPE),
            X(EGL_TRANSPARENT_TYPE),
            X(EGL_TRANSPARENT_RED_VALUE),
            X(EGL_TRANSPARENT_GREEN_VALUE),
            X(EGL_TRANSPARENT_BLUE_VALUE),
            X(EGL_BIND_TO_TEXTURE_RGB),
            X(EGL_BIND_TO_TEXTURE_RGBA),
            X(EGL_MIN_SWAP_INTERVAL),
            X(EGL_MAX_SWAP_INTERVAL),
            X(EGL_LUMINANCE_SIZE),
            X(EGL_ALPHA_MASK_SIZE),
            X(EGL_COLOR_BUFFER_TYPE),
            X(EGL_RENDERABLE_TYPE),
            X(EGL_CONFORMANT),
           };
        #undef X

        for(size_t j = 0; j < sizeof(names) / sizeof(names[0]); j++)
        {
            EGLint value = -1;
            EGLint returnVal = eglGetConfigAttrib(m_disp, m_conf[0], names[j].attribute, &value);
            EGLint error = eglGetError();
            if (returnVal && error == EGL_SUCCESS)
                LOG_INFO(" %s: %d (0x%x)", names[j].name, value, value);
        }
    }

    void GLRenderer::loadShaders()
    {
        //Initialize Shaders
        const char* shaders[] = {"color", "uniColor", "colorGrid", "vectorField", "simpleTexture", "planeVolumeRendering"};
        const bool  hasGeom[] = {false, false, false, false, false, false};
        for(uint32_t i = 0; i < sizeof(shaders)/sizeof(shaders[0]); i++)
        {
            Shader* shader = NULL;
            std::string vertDataPath = m_surfaceData->dataPath + "/Shaders/" + shaders[i] + ".vert";
            std::string fragDataPath = m_surfaceData->dataPath + "/Shaders/" + shaders[i] + ".frag";

            FILE* vertShadFile = fopen(vertDataPath.c_str(), "r");
            FILE* fragShadFile = fopen(fragDataPath.c_str(), "r");
            FILE* geomShadFile = NULL;

            if(vertShadFile == NULL || fragShadFile == NULL)
                goto error;

            if(hasGeom[i])
            {
                std::string geomDataPath = m_surfaceData->dataPath + "/Shaders/" + shaders[i] + ".geom";
                geomShadFile = fopen(geomDataPath.c_str(), "r");
                if(geomShadFile == NULL)
                    goto error;
            }

            shader = Shader::loadFromFiles(vertShadFile, fragShadFile, geomShadFile);

            if(shader == NULL)
                goto error;

            m_shaders.add(shaders[i], shader);
            goto endError;
error:
            LOG_ERROR("Could not initialize shader %s\n", shaders[i]);
            if(shader != NULL)
                delete shader;
endError:
            if(vertShadFile != NULL)
                fclose(vertShadFile);
            if(fragShadFile != NULL)
                fclose(fragShadFile);
            if(geomShadFile != NULL)
                fclose(geomShadFile);
        }
    }

    bool GLRenderer::internalCreateSurface(ANativeWindow* nativeWindow)
    {
        //Recreate the surface
        if(nativeWindow == NULL)
            return false;

        m_window = nativeWindow;
        ANativeWindow_setBuffersGeometry(m_window, 0, 0, m_format);

        if(!(m_surface = eglCreateWindowSurface(m_disp, m_conf[0], m_window, 0)))
        {
            LOG_ERROR("Can't create an EGL surface from this window. Error : %d", eglGetError());
            return false;
        }

        if(!eglMakeCurrent(m_disp, m_surface, m_surface, m_context))
        {
            LOG_ERROR("Can't make this surface current. Error : %d", eglGetError());
            return false;
        }

        eglQuerySurface(m_disp, m_surface, EGL_WIDTH, &m_width);
        eglQuerySurface(m_disp, m_surface, EGL_HEIGHT, &m_height);

        glViewport(0, 0, m_width, m_height);
        LOG_INFO("Creating surface with width=%d and height=%d\n", m_width, m_height);
        return true;
    }

    bool GLRenderer::initializeContext(ANativeWindow* nativeWindow)
    {
        {
            //Initialize the egl context
            const EGLint attribs[] =
            {
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_BLUE_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_RED_SIZE, 8,
                EGL_ALPHA_SIZE, 8,
                EGL_DEPTH_SIZE, 16,
                EGL_NONE, 0
            };

            if((m_disp = eglGetDisplay(EGL_DEFAULT_DISPLAY))==EGL_NO_DISPLAY)
            {
                LOG_ERROR("Can't get the default display. Error : %d", eglGetError());
                return false;
            }

            if(eglInitialize(m_disp, NULL, NULL) != EGL_TRUE)
            {
                LOG_ERROR("Can't init the egl buffer. Error : %d", eglGetError());
                return false;
            }

            if(!eglChooseConfig(m_disp, attribs, NULL, 0, &m_nbConf) || m_nbConf == 0)
            {
                LOG_ERROR("Cannot find any buffer configuration.");
                goto error3;
            }

            if(!eglChooseConfig(m_disp, attribs, m_conf, sizeof(m_conf)/sizeof(m_conf[0]), &m_nbConf))
            {
                LOG_ERROR("Cannot configure the EGL buffer. Error : %d", eglGetError());
                return false;
            }

            if(!eglGetConfigAttrib(m_disp, m_conf[0], EGL_NATIVE_VISUAL_ID, &m_format))
            {
                LOG_ERROR("Can't get an attributes. Error : %d", eglGetError());
                return false;
            }

            const EGLint eglAttribs[] =
            {
                EGL_CONTEXT_MAJOR_VERSION_KHR, 3,
                EGL_CONTEXT_MINOR_VERSION_KHR, 2,
                EGL_NONE, 0
            };

            if((m_context = eglCreateContext(m_disp, m_conf[0], EGL_NO_CONTEXT, eglAttribs)) == EGL_NO_CONTEXT)
            {
                LOG_ERROR("Can't create an EGL context. Error : %d", eglGetError());
                goto error3;
            }
            m_glVersion = 3;
            goto endError3;
        }
error3:
        {
            LOG_ERROR("Error creating OpenGL ES3 API... loading OpenGL ES2");

            const EGLint attribs[] =
            {
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_BLUE_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_RED_SIZE, 8,
                EGL_ALPHA_SIZE, 8,
                EGL_DEPTH_SIZE, 16,
                EGL_NONE, 0
            };

            const EGLint eglAttribs[] =
            {
                EGL_CONTEXT_CLIENT_VERSION, 2,
                EGL_NONE, 0
            };

            if(!eglChooseConfig(m_disp, attribs, NULL, 0, &m_nbConf) || m_nbConf == 0)
            {
                LOG_ERROR("Cannot find any buffer configuration.");
                goto error2;
            }

            if(!eglChooseConfig(m_disp, attribs, m_conf, sizeof(m_conf)/sizeof(m_conf[0]), &m_nbConf))
            {
                LOG_ERROR("Cannot configure the EGL buffer. Error : %d", eglGetError());
                goto error2;
            }

            if(!eglGetConfigAttrib(m_disp, m_conf[0], EGL_NATIVE_VISUAL_ID, &m_format))
            {
                LOG_ERROR("Can't get an attributes. Error : %d", eglGetError());
                goto error2;
            }

            if((m_context = eglCreateContext(m_disp, m_conf[0], EGL_NO_CONTEXT, eglAttribs)) == EGL_NO_CONTEXT)
            {
                LOG_ERROR("Can't create an EGL context. Error : %d", eglGetError());
                goto error2;
            }
            m_glVersion = 2;
            goto endError2;
        }
error2:
        LOG_ERROR("Can't load opengl es2 API... no api is loaded");
        m_glVersion = -1;
        return false;
endError2:
endError3:

        if(nativeWindow)
        {
            if(!internalCreateSurface(nativeWindow))
                return false;
        }
        else
        {
            if(!eglMakeCurrent(m_disp, EGL_NO_SURFACE, EGL_NO_SURFACE, m_context))
            {
                LOG_ERROR("Can't bind this context. Error : %d", eglGetError());
                return false;
            }
        }

        LOG_INFO("OpenGL Version : %s", glGetString(GL_VERSION));
        printEGLConfig();
        loadShaders();

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
    //    eglMakeCurrent(m_disp, m_surface, m_surface, m_context);
        Render::render();
    }

    void GLRenderer::setCurrentShader(Shader* shader)
    {
        m_currentShader = shader; 
        if(shader != NULL) 
            glUseProgram(shader->getProgramID());
    }

    Shader* GLRenderer::getShader(const std::string& shaderName)
    {
        return m_shaders.get(shaderName);
    }

    void GLRenderer::setSize(uint32_t width, uint32_t height)
    {
        m_width = width;
        m_height = height;
    }
}
