#include "Graphics/Materials/NormalizeMaterial.h"

namespace sereno
{
    NormalizeMaterial::NormalizeMaterial(GLRenderer* renderer, float min, float max) : Material(renderer, renderer->getShader("normalize1D")), m_min(min), m_max(max)
    {
        if(!getShader())
            return;
        getAttributs();
    }

    void NormalizeMaterial::initMaterial(const glm::mat4& objMat,  const glm::mat4& cameraMat,
                                         const glm::mat4& projMat, const glm::mat4& mvpMat,
                                         const glm::mat4& invMVPMat, const glm::vec4& cameraParams)
    {
        Material::initMaterial(objMat, cameraMat, projMat, mvpMat, invMVPMat, cameraParams);
        if(m_shader)
        {
            glUniform1f(m_uMin, m_min);
            glUniform1f(m_uMax, m_max);
        }
    }

    void NormalizeMaterial::getAttributs()
    {
        if(m_shader)
        {
            m_uMin = glGetUniformLocation(m_shader->getProgramID(), "uMin");
            m_uMax = glGetUniformLocation(m_shader->getProgramID(), "uMax");
        }
    }
}
