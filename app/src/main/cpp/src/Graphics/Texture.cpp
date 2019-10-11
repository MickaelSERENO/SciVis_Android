#include "Graphics/Texture.h"
#include <utility>

namespace sereno
{
    Texture::Texture(GLuint texture, bool shouldDelete) : m_textureID(texture), m_shouldDelete(shouldDelete)
    {}

    Texture::Texture(Texture&& texture) 
    {
        *this = std::move(texture);
    }

    Texture& Texture::operator=(Texture&& texture)
    {
        if(this != &texture)
        {
            m_textureID      = texture.m_textureID;
            m_isTextureValid = true;
            texture.m_isTextureValid = false;
            texture.m_textureID      = 0;
        }

        return *this;
    }

    Texture::~Texture()
    {
        if(m_isTextureValid && m_shouldDelete)
            glDeleteTextures(1, &m_textureID);
    }

    static GLuint createTexture2D(uint32_t width, uint32_t height, void* data, GLint internalFormat, GLenum format, GLenum type)
    {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        {
            //Bilinear filtering is enough
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            //Repeat the texture if needed
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
                                           format, type, (GLvoid*)data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        glBindTexture(GL_TEXTURE_2D, 0);

        return textureID;
    }

    Texture2D::Texture2D(uint32_t width, uint32_t height, void* data, GLint internalFormat, GLenum format, GLenum type) : Texture(createTexture2D(width, height, data, internalFormat, format, type)), m_width(width), m_height(height)
    {
    }

    Texture2D::Texture2D(GLuint textureID, uint32_t texWidth, uint32_t texHeight, bool shouldDelete) : Texture(textureID, shouldDelete), m_width(texWidth), m_height(texHeight)
    {
    }
}
