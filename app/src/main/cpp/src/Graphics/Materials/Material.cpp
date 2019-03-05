#include "Graphics/Materials/Material.h"

namespace sereno
{
    Material::Material(GLRenderer* glRenderer, Shader* shader) : m_glRenderer(glRenderer), m_shader(shader)
    {
        getAttributs();
    }

    Material::~Material()
    {}

    void Material::bindMaterial(const glm::mat4& objMat,  const glm::mat4& cameraMat,
                                const glm::mat4& projMat, const glm::mat4& mvpMat,
                                const glm::mat4& invMVPMat)
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
        initMaterial(objMat, cameraMat, projMat, mvpMat, invMVPMat);
    }

    void Material::initMaterial(const glm::mat4& objMat,  const glm::mat4& cameraMat,
                                const glm::mat4& projMat, const glm::mat4& mvpMat,
                                const glm::mat4& invMVPMat)
    {
        if(m_shader)
        {
            glUniformMatrix4fv(m_uObjMat,    1, false, glm::value_ptr(objMat));
            glUniformMatrix4fv(m_uCameraMat, 1, false, glm::value_ptr(cameraMat));
            glUniformMatrix4fv(m_uMVP,       1, false, glm::value_ptr(mvpMat));
            glUniformMatrix4fv(m_uInvMVP,    1, false, glm::value_ptr(invMVPMat));
            glUniformMatrix4fv(m_uProjMat,   1, false, glm::value_ptr(projMat));
        }
    }

    void Material::bindTexture(GLuint textureID, uint8_t textureDim, uint8_t id)
    {
        if(id < MATERIAL_MAXTEXTURE && m_shader)
        {
            glActiveTexture(GL_TEXTURE0+id);
            if(textureDim == 1 || textureDim == 2)
                glBindTexture(GL_TEXTURE_2D, textureID);
            else if(textureDim == 3)
                glBindTexture(GL_TEXTURE_3D, textureID);
            glUniform1i(m_uTextures[id], id);
        }
    }

    void Material::getAttributs()
    {
        if(m_shader)
        {
            m_uCameraMat = glGetUniformLocation(m_shader->getProgramID(), "uCameraMat");
            m_uObjMat    = glGetUniformLocation(m_shader->getProgramID(), "uObjMat");
            m_uMVP       = glGetUniformLocation(m_shader->getProgramID(), "uMVP");
            m_uInvMVP    = glGetUniformLocation(m_shader->getProgramID(), "uInvMVP");
            m_uProjMat   = glGetUniformLocation(m_shader->getProgramID(), "uProjMat");
            for(uint32_t i = 0; i < MATERIAL_MAXTEXTURE; i++)
                m_uTextures[i] = glGetUniformLocation(m_shader->getProgramID(), ("uTexture"+std::to_string(i)).c_str());
        }
    }
}
