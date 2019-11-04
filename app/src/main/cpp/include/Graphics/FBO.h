#ifndef  FBO_INC
#define  FBO_INC

#include <cstdint>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace sereno
{
    /** \brief  class representing a Frame Buffer Object */
    class FBO
    {
        public:
            /** \brief  Constructor, create a FBO
             *
             * \param width the FBO width
             * \param height the FBO height
             * \param colorInternalFormat the internal format of the color texture
             * \param hasDepthBuffer should this FBO has any depth information? */
            FBO(uint32_t width, uint32_t height, GLenum colorInternalFormat = GL_RGBA, bool hasDepthBuffer = true);

            /** \brief  Copy constructor. This class is NOT assignable
             * \param copy the copy variable */
            FBO(const FBO& copy) = delete;

            /** \brief  Copy assignment. This class is NOT assignable
             * \param copy the copy variable
             * \return reference to this object */
            FBO& operator=(const FBO& copy) = delete;

            /** \brief  Movement constructor.
             * \param mvt the object to move */
            FBO(FBO&& mvt);

            /** \brief  Movement assignment.
             * \param mvt the object to move
             * \return *this */
            FBO& operator=(FBO&& mvt);

            /** \brief  Destructor, free all texture resources */
            virtual ~FBO();

            /** \brief  Get the width of this FBO
             * \return  The width of the FBO. Both the Color and Depth buffers shared this width */
            GLsizei getWidth()  const {return m_width;}

            /** \brief  Get the height of this FBO
             * \return  The height of the FBO. Both the Color and Depth buffers shared this height */
            GLsizei getHeight() const {return m_height;}

            /** \brief  Get the color buffer. Use "hasColorBuffer" beforehand to check if this object is valid or not
             * \return  The color buffer */
            GLuint getColorBuffer() const {return m_colorBuffer;}

            /** \brief  Get the depth buffer. Use "hasDepthBuffer" beforehand to check if this object is valid or not
             * \return  The depth buffer */
            GLuint getDepthBuffer() const {return m_depthBuffer;}

            /** \brief  Get the Framebuffer OpenGL object
             * \return  The OPengl Framebuffer object */
            GLuint getBuffer() const {return m_buffer;}

            /** \brief  Steal the color buffer from this FBO. Draw call associated to this FBO after that is undefined behavior.
             * \return  the Color buffer. It will not be destroyed at the end of this object life-time */
            GLuint stealColorBuffer() {m_hasColorBuffer = false; return m_colorBuffer;}

            /** \brief  Steal the depth buffer from this FBO. Draw call associated to this FBO after that is undefined behavior.
             * \return  the Depth buffer. It will not be destroyed at the end of this object life-time */
            GLuint stealDepthBuffer() {m_hasDepthBuffer = false; return m_depthBuffer;}

            /** \brief  Has this FBO any color information?
             * \return  true if yes, false otherwise */
            bool hasColorBuffer() const {return m_hasColorBuffer;}

            /** \brief  Has this FBO any depth information?
             * \return  true if yes, false otherwise */
            bool hasDepthBuffer() const {return m_hasDepthBuffer;}

            /** \brief  Has this object a Framebuffer OpenGL valid object?
             * \return  true if yes, false otherwise*/
            bool hasBuffer() const {return m_hasBuffer;}
        private:
            /** \brief  Clear the FBO objects */
            void clear();

            GLsizei m_width;       /*!< The width */
            GLsizei m_height;      /*!< The height */
            GLuint  m_colorBuffer; /*!< The color texture buffer created with glGenTextures*/
            GLuint  m_depthBuffer; /*!< The depth texture buffer created with glGenTextures*/
            GLuint  m_buffer;      /*!< The framebuffer id created with glGenFrameBuffers*/

            bool    m_hasBuffer      = false; /*!< Does this object has any FBO? (used because of movement constructor)*/
            bool    m_hasColorBuffer = false; /*!< Is the color buffer created?*/
            bool    m_hasDepthBuffer = false; /*!< Is the depth buffer created?*/
    };
}

#endif
