#include "Graphics/FBO.h"
#include "utils.h"
#include <utility>
#include <algorithm>

namespace sereno
{
    /** \brief  Set the current texture parameters. This function is created for not copy/pasting parameters for depth and color textures */
    static void _setTextureParams()
    {
        //Bilinear filtering is enough
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //Cut at [0,1]
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    FBO::FBO(uint32_t width, uint32_t height, GLenum colorInternalFormat, bool hasDepthBuffer) : m_width(width), m_height(height)
    {
        //Create and bind FBO
        glGenFramebuffers(1, &m_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);
        {
            //Handle color assignment
            glGenTextures    (1, &m_colorBuffer);
            glBindTexture(GL_TEXTURE_2D, m_colorBuffer);
            {
                _setTextureParams();
                for(int i = 0; width != 1 && height != 1; i++)
                {
                    glTexStorage2D(GL_TEXTURE_2D, i, colorInternalFormat, width, height);
                    width = std::max(1U, (width / 2));
                    height = std::max(1U, (height / 2));
                }
            }
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer, 0); 
            m_hasColorBuffer = true;

            //Handle depth assignment
            if(hasDepthBuffer)
            {
                glGenTextures    (1, &m_depthBuffer);
                glBindTexture(GL_TEXTURE_2D, m_depthBuffer);
                {
                    _setTextureParams();
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
                }
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthBuffer, 0); 
                m_hasDepthBuffer = true;
            }

            //Set draw call buffers inds
            GLenum drawBuff[] = {GL_COLOR_ATTACHMENT0};
            glDrawBuffers(1, drawBuff);

            //Check framebuffer's status
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if(status != GL_FRAMEBUFFER_COMPLETE)
            {
                LOG_ERROR("The initialization of the framebuffer failed. Status : %d\n", status);
                clear();
            }
        }
        m_hasBuffer = true;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    FBO::~FBO()
    {
        clear();
    }

    void FBO::clear()
    {
        //Delete textures + FBO GL objects
        if(hasColorBuffer())
            glDeleteTextures(1, &m_colorBuffer);
        if(hasDepthBuffer())
            glDeleteTextures(1, &m_depthBuffer);
        if(m_hasBuffer)
            glDeleteFramebuffers(1, &m_buffer);

        m_hasColorBuffer = m_hasDepthBuffer = m_hasBuffer = false;
    }
    
    FBO::FBO(FBO&& mvt)
    {
        *this = std::move(mvt);
    }

    FBO& FBO::operator=(FBO&& mvt)
    {
        if(this != &mvt)
        {
            //Copy variables
            m_width = mvt.m_width;
            m_height = mvt.m_height;
            m_colorBuffer = mvt.m_colorBuffer;
            m_depthBuffer = mvt.m_depthBuffer;
            m_hasColorBuffer = mvt.m_hasColorBuffer;
            m_hasDepthBuffer = mvt.m_hasDepthBuffer;
            m_hasBuffer      = mvt.m_hasBuffer;

            //Set to false any "assignment"
            mvt.m_hasColorBuffer = false;
            mvt.m_hasDepthBuffer = false;
            mvt.m_hasBuffer      = false;
        }

        return *this;
    }
}
