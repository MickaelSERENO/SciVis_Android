#include "Graphics/Materials/Material.h"

namespace sereno
{
    Material::Material(GLRenderer* glRenderer, Shader* shader) : m_glRenderer(glRenderer), m_shader(shader)
    {}

    Material::~Material()
    {}

    void Material::bindMaterial(const glm::mat4& objMat, const glm::mat4& cameraMat, 
                                const glm::mat4& mvpMat, const glm::mat4& invMVPMat)
    {
        //Check the shader
        if(m_shader == NULL)
        {
            LOG_ERROR("Could not bind this material because no shader was bound to it\n");
            return;
        }

        //Check if we have to change the current used program
        if(m_glRenderer->getCurrentShader() != m_shader)
            m_glRenderer->setCurrentShader(m_shader);

        //Init the internal state of the material (mostly set Uniform values)
        initMaterial(objMat, cameraMat, mvpMat, invMVPMat);
    }

    void Material::initMaterial(const glm::mat4& objMat, const glm::mat4& cameraMat, 
                                const glm::mat4& mvpMat, const glm::mat4& invMVPMat)
    {
        glUniformMatrix4fv(m_uObjMat,    0, false, glm::value_ptr(objMat));
        glUniformMatrix4fv(m_uCameraMat, 0, false, glm::value_ptr(cameraMat));
        glUniformMatrix4fv(m_uMVP,       0, false, glm::value_ptr(mvpMat));
        glUniformMatrix4fv(m_uInvMVP,    0, false, glm::value_ptr(invMVPMat));
    }

    void Material::getAttributs()
    {
        m_uCameraMat = glGetUniformLocation(m_shader->getProgramID(), "m_uCameraMat");
        m_uObjMat    = glGetUniformLocation(m_shader->getProgramID(), "m_uObjMat");
        m_uMVP       = glGetUniformLocation(m_shader->getProgramID(), "m_uMVP");
        m_uInvMVP    = glGetUniformLocation(m_shader->getProgramID(), "m_uInvMVP");
    }
}
