#include "Graphics/Materials/CPCPMaterial.h"

namespace sereno
{
    CPCPMaterial::CPCPMaterial(GLRenderer* renderer, uint32_t nbSamples) : Material(renderer, renderer->getShader("cpcp")), m_nbSamples(nbSamples)
    {
        if(!getShader())
            return;
        getAttributs();
    }

    CPCPMaterial::~CPCPMaterial()
    {}

    void CPCPMaterial::initMaterial(const glm::mat4& objMat,  const glm::mat4& cameraMat,
                                    const glm::mat4& projMat, const glm::mat4& mvpMat,
                                    const glm::mat4& invMVPMat, const glm::vec4& cameraParams)
    {
        Material::initMaterial(objMat, cameraMat, projMat, mvpMat, invMVPMat, cameraParams);
        if(m_shader)
        {
            glUniform1i(m_uNBSamples, m_nbSamples);
        }
    }

    void CPCPMaterial::getAttributs()
    {
        if(m_shader)
            m_uNBSamples = glGetUniformLocation(m_shader->getProgramID(), "uNBSamples");
    }
}
