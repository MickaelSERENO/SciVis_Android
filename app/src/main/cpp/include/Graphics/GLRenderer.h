#ifndef  GLRENDERER_INC
#define  GLRENDERER_INC

#include <pthread.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <android/native_window.h>
#include <utils.h>

#include "Render.h"
#include "Shader.h"
#include "ResourcesManager.h"

namespace sereno
{
    class GLSurfaceViewData;

    /* \brief the Renderer which can be modified by both Java and C++ */
    class GLRenderer : public Render
    {
        public:
            /* \brief Constructor. Does not yet create the surface
             * \param data the data sent from Java when creating this Renderer */
            GLRenderer(GLSurfaceViewData* data);

            /* \brief Destructor. */
            ~GLRenderer();

            /* \brief Function called by Java when the surface is being destroyed. 
             * Lock any futur draw call to this surface (e.g swapsurface)*/
            void destroySurface();

            /* \brief Initialize the EGL Context
             * \param nativeWindow the native window to draw on. If NULL, no surface will be bound yet (EGL_NO_SURFACE)
             * \return wheter or not the context is initialized */
            bool initializeContext(ANativeWindow* nativeWindow);

            /* \brief Function called by Java when the surface is being created 
             * \param nativeWindow the nativeWindow associated with the surface*/
            void createSurface(ANativeWindow* nativeWindow);

            /* \brief Swap the buffers for displaying into the SurfaceView
             * the function render does it automatically */
            void swapBuffers();

            /* \brief Render all the known Drawable on screen */
            void render();

            /* \brief Retrieve a Shader based on its name.
             * \param shaderName the shader name
             * \return the Shader corresponding, or NULL if not found */
            Shader* getShader(const std::string& shaderName);

            /* \brief Return the current in used shader */
            Shader* getCurrentShader() {return m_currentShader;}

            /* \brief Set the current in used shader */
            void setCurrentShader(Shader* shader);

            /* \brief Get the OpenGL Version loaded. -1 == error
             * \param the opengl es version loaded*/
            int getGLESVersion() const {return m_glVersion;}
        private:
            /* \brief Destroy the EGL surface without locking any mutex */
            void internalEglDestroySurface();

            /* \brief Create the EGL surface in the correct thread
             * \return true on success, false on failure*/
            bool internalCreateSurface(ANativeWindow* nativeWindow);

            /* \brief Set the Surface size
             * \param the new width
             * \param the new height*/
            void setSize(uint32_t width, uint32_t height);

            /* \brief Print the EGL config*/
            void printEGLConfig() const;

            /* \brief Load every shaders*/
            void loadShaders();

            GLSurfaceViewData* m_surfaceData = NULL; /*!< The data sent from Java at the creation of this Renderer*/

            EGLDisplay m_disp    = EGL_NO_DISPLAY; /*!< The EGL display*/
            EGLSurface m_surface = EGL_NO_SURFACE; /*!< The EGL surface*/
            EGLContext m_context = EGL_NO_CONTEXT; /*!< The EGL context*/
            EGLConfig  m_conf[1];                  /*!< The EGL configuration*/

            EGLint m_nbConf = 0; /*!< Number of available EGL configuration */
            EGLint m_format = 0; /*!< EGL_NATIVE_VISUAL_ID value*/

            bool            m_destroy        = false; /*!< Destroy the surface ?*/
            bool            m_recreate       = false; /*!< Recreate the surface ?*/
            ANativeWindow*  m_window         = NULL;  /*!< The android native window*/
            ANativeWindow*  m_recreateWindow = NULL;  /*!< The increation native window*/

            ResourcesManager<Shader*> m_shaders;              /*!< The shaders loaded by this OpenGL Renderer*/
            Shader*                   m_currentShader = NULL; /*!< The current in used shader program*/

            int m_glVersion = -1;

            friend GLSurfaceViewData;
    };
}

#endif
