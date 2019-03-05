#include "Graphics/Materials/ColorMaterial.h"

namespace sereno
{
    ColorMaterial::ColorMaterial(GLRenderer* renderer) : Material(renderer, renderer->getShader("color"))
    {
        if(!getShader())
            return;
        getAttributs();
    }

    ColorMaterial::~ColorMaterial()
    {
    }

    void ColorMaterial::initMaterial(const glm::mat4& objMat,  const glm::mat4& cameraMat,
                                     const glm::mat4& projMat, const glm::mat4& mvpMat,
                                     const glm::mat4& invMVPMat)
    {
        Material::initMaterial(objMat, cameraMat, projMat, mvpMat, invMVPMat);
        if(m_shader)
        {
            glUniform1i(m_uUseUniColor, false);
            glUniform1i(m_uUseTexture, false);
        }
    }

    void ColorMaterial::getAttributs()
    {
        if(m_shader)
        {
            m_uUseUniColor = glGetUniformLocation(m_shader->getProgramID(), "uUseUniColor");
            m_uUseTexture  = glGetUniformLocation(m_shader->getProgramID(), "uUseTexture");
        }
    }
}

