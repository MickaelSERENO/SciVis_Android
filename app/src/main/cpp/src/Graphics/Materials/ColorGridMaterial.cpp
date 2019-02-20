#include "Graphics/Materials/ColorGridMaterial.h"

namespace sereno
{
    ColorGridMaterial::ColorGridMaterial(GLRenderer* renderer) : Material(renderer, renderer->getShader("colorGrid"))
    {
        if(!getShader())
            return;
        getAttributs();
    }

    ColorGridMaterial::~ColorGridMaterial()
    {}

    void ColorGridMaterial::initMaterial(const glm::mat4& objMat, const glm::mat4& cameraMat, 
                                         const glm::mat4& mvpMat, const glm::mat4& invMVPMat)
    {
        Material::initMaterial(objMat, cameraMat, mvpMat, invMVPMat);
        if(m_shader)
        {
            glUniform3fv(m_uSpacing,   1, m_spacing);
            glUniform3fv(m_uDimension, 1, m_dimension);
        }
    }

    void ColorGridMaterial::getAttributs()
    {
        if(m_shader)
        {
            m_uSpacing   = glGetUniformLocation(m_shader->getProgramID(), "uSpacing");
            m_uDimension = glGetUniformLocation(m_shader->getProgramID(), "uDimension");
        }
    }
}
