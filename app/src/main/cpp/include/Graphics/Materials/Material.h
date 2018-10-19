#ifndef  MATERIAL_INC
#define  MATERIAL_INC

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Graphics/Shader.h"
#include "Graphics/GLRenderer.h"

namespace sereno
{
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
             * \param objMat the object matrix
             * \param cameraMat the camera matrix
             * \param mvpMat objMat*cameraMat
             * \param invMVPMat inverse(invMVPMat)*/
            void bindMaterial(const glm::mat4& objMat, const glm::mat4& cameraMat, 
                              const glm::mat4& mvpMat, const glm::mat4& invMVPMat);
        protected:
            /* \brief Initialize the internal state of the Material
             * We ask so many matrix (which some can be calculated) for performance issue (not recomputing these in the material and in the object calling this method)
             * \param objMat the object matrix
             * \param cameraMat the camera matrix
             * \param mvpMat objMat*cameraMat
             * \param invMVPMat inverse(invMVPMat)*/
            virtual void initMaterial(const glm::mat4& objMat, const glm::mat4& cameraMat, 
                                      const glm::mat4& mvpMat, const glm::mat4& invMVPMat);
            
            /* \brief Get the material attributs from the shader. Mostly these attributes are uniform location */
            virtual void getAttributs();

            Shader*     m_shader;     /*!< The Shader used by this Material*/
            GLRenderer* m_glRenderer; /*!< The OpenGL Context renderer. Note that the object will slightly being modified. 
                                           Indeed, the current shader information will be modified in order to not rebind multiple times the same Shader*/

            GLint       m_uCameraMat; /*!< The uniform ID of uCameraMat*/
            GLint       m_uObjMat;    /*!< The uniform ID of uObjMat*/
            GLint       m_uMVP;       /*!< The uniform ID of uMVP*/
            GLint       m_uInvMVP;    /*!< The uniform ID of uInvMVP*/
    };
}

#endif
