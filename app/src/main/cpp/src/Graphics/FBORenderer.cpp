#include "Graphics/FBORenderer.h"

namespace sereno
{
    FBORenderer::FBORenderer(FBO* fbo) : Render(), m_fbo(fbo)
    {
        //Reinitialize the viewport
        if(m_fbo)
            setViewport(Rectangle2i(0, 0, m_fbo->getWidth(), m_fbo->getHeight()));
    }

    void FBORenderer::setFBO(FBO* fbo, bool resetViewport)
    {
        m_fbo = fbo;

        //Reinitialize the viewport
        if(m_fbo && resetViewport)
            setViewport(Rectangle2i(0, 0, m_fbo->getWidth(), m_fbo->getHeight()));
    }

    void FBORenderer::render()
    {
        if(m_fbo)
        {
            GLint curFBO;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &curFBO);

            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo->getBuffer());
                Render::render();
            glBindFramebuffer(GL_FRAMEBUFFER, curFBO);
        }
    }
}
