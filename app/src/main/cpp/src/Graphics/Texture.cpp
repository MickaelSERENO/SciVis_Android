#include "Graphics/Texture.h"

namespace sereno
{
    Texture::Texture(uint32_t width, uint32_t height, void* data)
    {
        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        {
            //Bilinear filtering is enough
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            //Repeat the texture if needed
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
                                           GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        glBindTexture(GL_TEXTURE_2D, 0);

        m_width  = width;
        m_height = height;
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &m_textureID);
    }
}
