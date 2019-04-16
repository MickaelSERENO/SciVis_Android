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
            void bindMaterial(const glm::mat4& objMat, const glm::mat4& cameraMat,
                              const glm::mat4& projMat, const glm::mat4& mvpMat,
                              const glm::mat4& invMVPMat);

            /**
             * \brief  Bind a texture to this material. Has to be called before "bindMaterial"
             * \param textureID the texture to bind
             * \param textureDim the texture dimension (1, 2 or 3. 1 and 2 are used as GL_TEXTURE_2D)
             * \param id the texture ID (GL_TEXTURE0, GL_TEXTURE1, etc.). Must be inferior than MATERIAL_MAXTEXTURE*/
            void bindTexture(GLuint textureID, uint8_t textureDim, uint8_t id);

            /** \brief UNbind the texture ID's id
             * \param id the texture ID to unbind*/
            void unbindTexture(uint8_t id);
        protected:
            /* \brief Initialize the internal state of the Material
             * We ask so many matrix (which some can be calculated) for performance issue (not recomputing these in the material and in the object calling this method)
             * \param objMat the object matrix (model)
             * \param cameraMat the camera matrix (view)
             * \param projMat the projection matrix (projection)
             * \param mvpMat objMat*cameraMat
             * \param invMVPMat inverse(invMVPMat)*/
            virtual void initMaterial(const glm::mat4& objMat,  const glm::mat4& cameraMat,
                                      const glm::mat4& projMat, const glm::mat4& mvpMat,
                                      const glm::mat4& invMVPMat);
            
            /* \brief Get the material attributs from the shader. Mostly these attributes are uniform location */
            void getAttributs();

            Shader*     m_shader;     /*!< The Shader used by this Material*/
            GLRenderer* m_glRenderer; /*!< The OpenGL Context renderer. Note that the object will slightly being modified. 
                                           Indeed, the current shader information will be modified in order to not rebind multiple times the same Shader*/

            GLint       m_uProjMat;   /*!< The uniform ID of uProjMat*/
            GLint       m_uCameraMat; /*!< The uniform ID of uCameraMat*/
            GLint       m_uObjMat;    /*!< The uniform ID of uObjMat*/
            GLint       m_uMVP;       /*!< The uniform ID of uMVP*/
            GLint       m_uInvMVP;    /*!< The uniform ID of uInvMVP*/
            GLint       m_uTextures[MATERIAL_MAXTEXTURE]; /*!< The uniform IDs of uTexture<i>*/

            TextureBinding m_textures[MATERIAL_MAXTEXTURE]; /*!< List of texture binding*/
    };
}

#endif
