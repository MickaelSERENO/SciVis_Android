#include "Graphics/Materials/PhongMaterial.h"

namespace sereno
{
    PhongMaterial::PhongMaterial(GLRenderer* renderer, const Color& color, float ambientCoeff, float diffuseCoeff, float specularCoeff, float shininess, const Color& lightColor, const glm::vec3& lightDir) : Material(renderer, renderer->getShader("colorPhong")), m_color(color.r, color.g, color.b), m_phongCoeffs(ambientCoeff, diffuseCoeff, specularCoeff, shininess), m_lightColor(lightColor.r, lightColor.g, lightColor.b), m_lightDir(lightDir)
    {
        if(!getShader())
            return;
        getAttributs();
    }

    PhongMaterial::~PhongMaterial()
    {}

    void PhongMaterial::initMaterial(const glm::mat4& objMat,  const glm::mat4& cameraMat,
                                     const glm::mat4& projMat, const glm::mat4& mvpMat,
                                     const glm::mat4& invMVPMat, const glm::vec4& cameraParams)
    {
        Material::initMaterial(objMat, cameraMat, projMat, mvpMat, invMVPMat, cameraParams);
        if(m_shader)
        {
            glUniform3fv(m_uColor,       1, glm::value_ptr(m_color));
            glUniform4fv(m_uPhongCoeffs, 1, glm::value_ptr(m_phongCoeffs));
            glUniform3fv(m_uLightColor,  1, glm::value_ptr(m_lightColor));
            glUniform3fv(m_uLightDir,    1, glm::value_ptr(m_lightDir));
        }
    }

    void PhongMaterial::getAttributs()
    {
        if(m_shader)
        {
            m_uColor       = glGetUniformLocation(m_shader->getProgramID(), "uColor");
            m_uPhongCoeffs = glGetUniformLocation(m_shader->getProgramID(), "uPhongCoeffs");
            m_uLightColor  = glGetUniformLocation(m_shader->getProgramID(), "uLightColor");
            m_uLightDir    = glGetUniformLocation(m_shader->getProgramID(), "uLightDir");
        }
    }
}
