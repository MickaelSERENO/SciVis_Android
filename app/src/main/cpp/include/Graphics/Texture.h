#ifndef  TEXTURE_INC
#define  TEXTURE_INC

#include <cstdint>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace sereno
{
    /** \brief  Class is an OpenGL texture wrapper. */
    class Texture
    {
        public:
            /** \brief  Constructor. 
             * \param texture The texture OpenGL object 
             * \param shouldDelete should this object free the texture object?*/
            Texture(GLuint texture, bool shouldDelete = true);

            /** \brief  Movement constructor.
             * \param texture the texture to move */
            Texture(Texture&& texture);

            /** \brief  constructor copy. This will not be implemented because the class should NOT be copied
             * \param tex the copy parameter */
            Texture(const Texture& tex) = delete;

            /** \brief  Assignment operator. This will not be implemented because the class should NOT be copied
             * \param tex the copy parameter */
            Texture& operator=(const Texture& tex) = delete;

            /** \brief  Move assignment operator
             * \param texture the Texture to move
             * \return   *this */
            Texture& operator=(Texture&& texture);

            /** \brief Destructor, delete the OpenGL texture object */
            virtual ~Texture();

            /* \brief  Get the OpenGL Texture ID
             * \return   The OpenGL texture ID*/
            GLuint getTextureID() {return m_textureID;}
        private:
            /** \brief  The texture ID */
            GLuint   m_textureID = 0;

            /** \brief  Is the texture a valid one? */
            bool m_isTextureValid = true;

            /** \brief  Should this class delete the texture? */
            bool m_shouldDelete = true;

    };

    class Texture2D : public Texture
    {
        public:
            /* \brief  Texture Constructor, construct a Texture from a RGBA8888 data
             * \param width the data width
             * \param height the data height
             * \param data the raw data
             * \param internalFormat see glTexImage2D::internalFormat parameter
             * \param format see glTexImage2D::format parameter
             * \param type see glTexImage2D::type parameter */
            Texture2D(uint32_t width, uint32_t height, void* data, 
                      GLint internalFormat = GL_RGBA, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);

            /** \brief  Create a Texture2D via an existing one
             * \param textureID the already created texture 2D. In GLES 2.0, width and height cannot get extracted from this texture...
             * \param texWidth the texture Width
             * \param texHeight the texture Height
             * \param shouldDelete Should we delete this texture at the end of this object life-time?  */
            Texture2D(GLuint textureID, uint32_t texWidth, uint32_t texHeight, bool shouldDelete=true);

            /* \brief  Get the texture width
             * \return   The texture width*/
            uint32_t getWidth() {return m_width;}

            /* \brief  Get the texture height
             * \return   The texture height*/
            uint32_t getHeight() {return m_height;}
        private:
            /** \brief  The texture Width */
            uint32_t m_width;

            /** \brief  The texture height */
            uint32_t m_height;
    };
}

#endif
