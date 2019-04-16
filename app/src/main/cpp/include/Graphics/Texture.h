#ifndef  TEXTURE_INC
#define  TEXTURE_INC

#include <cstdint>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace sereno
{
    class Texture
    {
        public:
            /* \brief  Texture Constructor, construct a Texture from a RGBA8888 data
             * \param width the data width
             * \param height the data height
             * \param data the RGBA8888 data */
            Texture(uint32_t width, uint32_t height, void* data);

            virtual ~Texture();

            /* \brief  Get the OpenGL Texture ID
             * \return   The OpenGL texture ID*/
            GLuint getTextureID() {return m_textureID;}

            /* \brief  Get the texture width
             * \return   The texture width*/
            uint32_t getWidth() {return m_width;}

            /* \brief  Get the texture height
             * \return   The texture height*/
            uint32_t getHeight() {return m_height;}
        private:
            /** \brief  The texture ID */
            GLuint   m_textureID = 0;

            /** \brief  The texture Width */
            uint32_t m_width;

            /** \brief  The texture height */
            uint32_t m_height;
    };
}

#endif
