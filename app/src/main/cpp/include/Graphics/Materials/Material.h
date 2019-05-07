#ifndef  MATERIAL_INC
#define  MATERIAL_INC

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Graphics/Shader.h"
#include "Graphics/GLRenderer.h"

#define MATERIAL_MAXTEXTURE 10

namespace sereno
{
    struct TextureBinding
    {
        GLuint tex   = 0;     /*!< The texture GPU ID*/
        bool   valid = false; /*!< Is this texture vlaid or not?*/
        GLenum dim;           /*!< The texture dimension*/
    };

    struct Blend
    {
        bool   enable  = false;   /*!< Enable the transparency*/
        GLenum sFactor = GL_ONE;  /*!< See https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBlendFunc.xhtml sFactor property*/
        GLenum dFactor = GL_ZERO; /*!< See https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBlendFunc.xhtml dFactor property*/
    };

    /* \brief Material class.*/
    class Material
    {
        public:
            /* \brief Material Constructor. Initialize the material via selecting a Shader in the GLRenderer object
             * \param glRenderer contains information about the OpenGL context, and the needed shaders
             * \param shader the shader to use. Must be cmpiled with the context available in glRenderer */
            Material(GLRenderer* glRenderer, Shader* shader);

            /* \brief Virtual destructor.*/
            virtual ~Material();

            /* \brief Get the Shader associated to this Material
             * \return the shader associated to this material */
            Shader* getShader() {return m_shader;}

            /* \brief Bind the material. This will bind the shader and the internal material parameter 
             * We ask so many matrix (which some can be calculated) for performance issue (not recomputing these in the material and in the object calling this method)
             * \param objMat the object matrix (model)
             * \param cameraMat the camera matrix (view)
             * \param projMat the projection matrix (projection)
             * \param mvpMat objMat*cameraMat
             * \param invMVPMat inverse(invMVPMat)*/
            void bindMaterial(const glm::mat4& objMat,    const glm::mat4& cameraMat,
                              const glm::mat4& projMat,   const glm::mat4& mvpMat,
                              const glm::mat4& invMVPMat, const glm::vec4& cameraParams);

            /**
             * \brief  Bind a texture to this material. Has to be called before "bindMaterial"
             * \param textureID the texture to bind
             * \param textureDim the texture dimension (1, 2 or 3. 1 and 2 are used as GL_TEXTURE_2D)
             * \param id the texture ID (GL_TEXTURE0, GL_TEXTURE1, etc.). Must be inferior than MATERIAL_MAXTEXTURE*/
            void bindTexture(GLuint textureID, uint8_t textureDim, uint8_t id);

            /** \brief UNbind the texture ID's id
             * \param id the texture ID to unbind*/
            void unbindTexture(uint8_t id);

            /** \brief Set the blending property
             *  \param b the new blending property*/
            void setBlend(const Blend& b) {m_blend = b;}

            /** \brief Get the blending property
             * \return reference to the blending property*/
            const Blend& getBlend() const {return m_blend;}

            /** \brief Set the depth writting property. See glDepthMask
             * \param w true if enabled, false otherwise*/
            void setDepthWrite(bool w) {m_depthWrite = w;}

            /** \brief Get the depth writting property. See glDepthMask
             * \return true if enabled, false otherwise*/
            bool getDepthWrite() const {return m_depthWrite;}
        protected:
            /* \brief Initialize the internal state of the Material
             * We ask so many matrix (which some can be calculated) for performance issue (not recomputing these in the material and in the object calling this method)
             * \param objMat the object matrix (model)
             * \param cameraMat the camera matrix (view)
             * \param projMat the projection matrix (projection)
             * \param mvpMat objMat*cameraMat
             * \param invMVPMat inverse(invMVPMat)
             * \param cameraParams the camera parameters. Only w is used for now. w == 1.0 -> orthographic, w == 0.0 -> perspective*/
            virtual void initMaterial(const glm::mat4& objMat,    const glm::mat4& cameraMat,
                                      const glm::mat4& projMat,   const glm::mat4& mvpMat,
                                      const glm::mat4& invMVPMat, const glm::vec4& cameraParams);
            
            /* \brief Get the material attributs from the shader. Mostly these attributes are uniform location */
            void getAttributs();

            Shader*     m_shader;     /*!< The Shader used by this Material*/
            GLRenderer* m_glRenderer; /*!< The OpenGL Context renderer. Note that the object will slightly being modified. 
                                           Indeed, the current shader information will be modified in order to not rebind multiple times the same Shader*/

            GLint       m_uProjMat;      /*!< The uniform ID of uProjMat*/
            GLint       m_uCameraMat;    /*!< The uniform ID of uCameraMat*/
            GLint       m_uObjMat;       /*!< The uniform ID of uObjMat*/
            GLint       m_uMVP;          /*!< The uniform ID of uMVP*/
            GLint       m_uInvMVP;       /*!< The uniform ID of uInvMVP*/
            GLint       m_uInvMV;        /*!< The uniform ID of uInvMV*/
            GLint       m_uInvP;         /*!< The uniform ID of uInvP*/
            GLint       m_uCameraParams; /*!< The uniform ID of the uCameraParams*/
            GLint       m_uTextures[MATERIAL_MAXTEXTURE]; /*!< The uniform IDs of uTexture<i>*/

            Blend m_blend; /*!< Blend property*/
            bool  m_depthWrite = true; /*!< the depth writting*/

            TextureBinding m_textures[MATERIAL_MAXTEXTURE]; /*!< List of texture binding*/
    };
}

#endif
