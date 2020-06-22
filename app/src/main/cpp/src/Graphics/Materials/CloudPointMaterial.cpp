#include "Graphics/Materials/CloudPointMaterial.h"

namespace sereno
{
    CloudPointMaterial::CloudPointMaterial(GLRenderer* renderer, float pointSize) : Material(renderer, renderer->getShader("cloudPoint")), m_pointSize(pointSize)
    {
        if(!getShader())
            return;
        getAttributs();
    }

    CloudPointMaterial::~CloudPointMaterial()
    {}

    void CloudPointMaterial::initMaterial(const glm::mat4& objMat,  const glm::mat4& cameraMat,
                                          const glm::mat4& projMat, const glm::mat4& mvpMat,
                                          const glm::mat4& invMVPMat, const glm::vec4& cameraParams)
    {
        Material::initMaterial(objMat, cameraMat, projMat, mvpMat, invMVPMat, cameraParams);

        if(getShader())
            glUniform1f(m_uPointSize, m_pointSize);
    }

    void CloudPointMaterial::getAttributs()
    {
        if(getShader())
            m_uPointSize = glGetUniformLocation(m_shader->getProgramID(), "uPointSize");
    }
}
