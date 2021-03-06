#include "Graphics/Materials/UniColorMaterial.h"

namespace sereno
{
    UniColorMaterial::UniColorMaterial(GLRenderer* renderer, const Color& color) : Material(renderer, renderer->getShader("uniColor")),
                                                                                   m_color(NULL)
    {
        setColor(color);
        if(!getShader())
            return;
        getAttributs();
    }

    UniColorMaterial::UniColorMaterial(GLRenderer* renderer, const float* color) : Material(renderer, renderer->getShader("uniColor")),
                                                                                   m_color(NULL)
    {
        setColor(color);
        if(!getShader())
            return;
        getAttributs();
    }

    UniColorMaterial::~UniColorMaterial()
    {
        if(m_color)
            free(m_color);
    }

    void UniColorMaterial::initMaterial(const glm::mat4& objMat,  const glm::mat4& cameraMat,
                                        const glm::mat4& projMat, const glm::mat4& mvpMat,
                                        const glm::mat4& invMVPMat, const glm::vec4& cameraParams)
    {
        Material::initMaterial(objMat, cameraMat, projMat, mvpMat, invMVPMat, cameraParams);
        if(m_shader)
        {
            glUniform4fv(m_uColor, 1, m_color);
            glUniform1i(m_uUseTexture, false);
        }
    }

    void UniColorMaterial::setColor(const Color& color)
    {
        if(m_color)
            free(m_color);
        m_color = (float*)malloc(4*sizeof(float));
        color.getFloatArray(m_color);
    }

    void UniColorMaterial::setColor(const float* color)
    {
        if(m_color)
            free(m_color);
        m_color = (float*)malloc(4*sizeof(float));
        memcpy(m_color, color, 4*sizeof(float));
    }

    Color UniColorMaterial::getColor() const
    {
        return Color(m_color);
    }

    void UniColorMaterial::getAttributs()
    {
        if(m_shader)
        {
            m_uColor       = glGetUniformLocation(m_shader->getProgramID(), "uUniColor");
            m_uUseTexture  = glGetUniformLocation(m_shader->getProgramID(), "uUseTexture");
        }
    }
}
