#ifndef  NORMALIZEMATERIAL_INC
#define  NORMALIZEMATERIAL_INC

#include "Graphics/Materials/Material.h"

namespace sereno
{
    /** \brief  Material used to normalize a texture from min--max to 0--1 */
    class NormalizeMaterial : public Material
    {
        public:
            /** \brief  Constructor
             * \param renderer the Opengl context object 
             * \param min the minimum value 
             * \param max the maximum value */
            NormalizeMaterial(GLRenderer* renderer, float min=0, float max=1);

            /** \brief  Set the new range
             * \param min the new minimum value
             * \param max the new maximum value */
            void setRange(float min, float max) {m_min = min; m_max = max;}

            /** \brief Get the minimum value applied
             * \return the minimum value */
            float getMin() const {return m_min;}

            /** \brief Get the maximum value applied
             * \return the maximum value */
            float getMax() const {return m_max;}
        private:
            void initMaterial(const glm::mat4& objMat,  const glm::mat4& cameraMat,
                              const glm::mat4& projMat, const glm::mat4& mvpMat,
                              const glm::mat4& invMVPMat, const glm::vec4& cameraParams);
            void getAttributs();

            float m_min; /*!< The minimum value*/
            float m_max; /*!< The maximum value*/

            GLint m_uMin; /*!< The location of uMin in the shader*/
            GLint m_uMax; /*!< The location of uMax in the shader*/
    };
}

#endif
