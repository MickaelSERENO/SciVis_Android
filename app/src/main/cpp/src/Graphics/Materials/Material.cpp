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
                                const glm::mat4& invMVPMat, const glm::vec4& cameraParams)
    {
        //Check the shader
        if(m_shader == NULL)
        {
          //  LOG_ERROR("Could not bind this material because no shader was bound to it\n");
            return;
        }

        //Check if we have to change the current used program
        if(m_glRenderer->getCurrentShader() != m_shader)
            m_glRenderer->setCurrentShader(m_shader);

        //Init the internal state of the material (mostly set Uniform values)
        initMaterial(objMat, cameraMat, projMat, mvpMat, invMVPMat, cameraParams);
    }

    void Material::initMaterial(const glm::mat4& objMat,    const glm::mat4& cameraMat,
                                const glm::mat4& projMat,   const glm::mat4& mvpMat,  
                                const glm::mat4& invMVPMat, const glm::vec4& cameraParams)
    {
        if(m_shader)
        {
            //Send matrices
            glUniformMatrix4fv(m_uObjMat,    1, false, glm::value_ptr(objMat));
            glUniformMatrix4fv(m_uCameraMat, 1, false, glm::value_ptr(cameraMat));
            glUniformMatrix4fv(m_uMVP,       1, false, glm::value_ptr(mvpMat));
            glUniformMatrix4fv(m_uInvMVP,    1, false, glm::value_ptr(invMVPMat));
            glUniformMatrix4fv(m_uProjMat,   1, false, glm::value_ptr(projMat));
            if(m_uInvP != -1)
            {
                glm::mat4 temp = glm::inverse(projMat);
                glUniformMatrix4fv(m_uInvP, 1, false, glm::value_ptr(temp));
            }
            if(m_uInvMV != -1)
            {
                glm::mat4 temp = invMVPMat * projMat;
                glUniformMatrix4fv(m_uInvMV, 1, false, glm::value_ptr(temp));
            }
            glUniform4fv(m_uCameraParams, 1, glm::value_ptr(cameraParams));

            //Send textures
            for(int i = 0; i < MATERIAL_MAXTEXTURE; i++)
            {
                if(m_textures[i].valid)
                {
                    glActiveTexture(GL_TEXTURE0+i);
                    glBindTexture(m_textures[i].dim, m_textures[i].tex);
                    glUniform1i(m_uTextures[i], i);
                }
            }

            //Handle blending
            if(m_blend.enable)
            {
                glEnable(GL_BLEND);
                glBlendFunc(m_blend.sFactor, m_blend.dFactor);
            }
            else
                glDisable(GL_BLEND);

            if(m_depthWrite)
                glDepthMask(GL_TRUE);
            else
                glDepthMask(GL_FALSE);
        }
    }

    void Material::bindTexture(GLuint textureID, uint8_t textureDim, uint8_t id)
    {
        if(id < MATERIAL_MAXTEXTURE && m_shader)
        {
            m_textures[id].valid = true;
            m_textures[id].tex   = textureID;
            if(textureDim == 1 || textureDim == 2)
                m_textures[id].dim = GL_TEXTURE_2D;
            else if(textureDim == 3)
                m_textures[id].dim = GL_TEXTURE_3D;
        }
    }

    void Material::unbindTexture(uint8_t id)
    {
        m_textures[id].valid = false;
    }

    void Material::getAttributs()
    {
        if(m_shader)
        {
            m_uCameraMat    = glGetUniformLocation(m_shader->getProgramID(), "uCameraMat");
            m_uObjMat       = glGetUniformLocation(m_shader->getProgramID(), "uObjMat");
            m_uMVP          = glGetUniformLocation(m_shader->getProgramID(), "uMVP");
            m_uInvMVP       = glGetUniformLocation(m_shader->getProgramID(), "uInvMVP");
            m_uProjMat      = glGetUniformLocation(m_shader->getProgramID(), "uProjMat");
            m_uInvMV        = glGetUniformLocation(m_shader->getProgramID(), "uInvMV");
            m_uInvP         = glGetUniformLocation(m_shader->getProgramID(), "uInvP");
            m_uCameraParams = glGetUniformLocation(m_shader->getProgramID(), "uCameraParams");

            for(uint32_t i = 0; i < MATERIAL_MAXTEXTURE; i++)
                m_uTextures[i] = glGetUniformLocation(m_shader->getProgramID(), ("uTexture"+std::to_string(i)).c_str());
        }
    }
}
