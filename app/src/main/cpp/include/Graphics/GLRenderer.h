#ifndef  GLRENDERER_INC
#define  GLRENDERER_INC

#include <pthread.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <android/native_window.h>
#include <utils.h>

#include "Render.h"

namespace sereno
{
    /* \brief the Renderer which can be modified by both Java and C++ */
    class GLRenderer : public Render
    {
        public:
            /* \brief Constructor. Does not yet create the surface */
            GLRenderer();

            /* \brief Destructor. */
            ~GLRenderer();

            /* \brief Function called by Java when the surface is being destroyed. 
             * Lock any futur draw call to this surface (e.g swapsurface)*/
            void destroySurface();

            /* \brief Initialize the EGL Context
             * \return wheter or not the context is initialized */
            bool initializeContext();

            /* \brief Function called by Java when the surface is being created 
             * \param nativeWindow the nativeWindow associated with the surface*/
            void createSurface(ANativeWindow* nativeWindow);

            /* \brief Swap the buffers for displaying into the SurfaceView
             * the function render does it automatically */
            void swapBuffers();

            /* \brief Render all the known Drawable on screen */
            void render();
        private: 
            /* \brief Destroy the EGL surface without locking any mutex */
            void internalEglDestroySurface();

            /* \brief Create the EGL surface in the correct thread*/
            void internalCreateSurface(ANativeWindow* nativeWindow);

            EGLDisplay m_disp    = EGL_NO_DISPLAY; /*!< The EGL display*/
            EGLSurface m_surface = EGL_NO_SURFACE; /*!< The EGL surface*/
            EGLContext m_context = EGL_NO_CONTEXT; /*!< The EGL context*/
            EGLConfig  m_conf;                     /*!< The EGL configuration*/

            EGLint m_nbConf = 0; /*!< Number of available EGL configuration */
            EGLint m_format = 0; /*!< EGL_NATIVE_VISUAL_ID value*/

            int m_width  = 0; /*!< The width of the surface (pixels)*/
            int m_height = 0; /*!< The height of the surface (pixels)*/

            bool            m_destroy      = false; /*!< Destroy the surface ?*/
            bool            m_recreate     = false; /*!< Recreate the surface ?*/
            ANativeWindow*  m_window       = NULL;  /*!< The android native window*/
            ANativeWindow*  m_recreateWindow = NULL; /*!< The increation native window*/
    };
}

#endif
