#ifndef  CPCPMATERIAL_INC
#define  CPCPMATERIAL_INC

#include "Graphics/Materials/Material.h"

namespace sereno
{
    /** \brief  Class representing the Continuous Parallel Coordinate Plot material (needed information) */
    class CPCPMaterial : public Material
    {
        public:
            /** \brief  Constructor.
             * \param renderer the Opengl context object 
             * \param nbSamples the number of samples to gather along the dual-line*/
            CPCPMaterial(GLRenderer* renderer, uint32_t nbSamples);

            /** \brief  Destructor */
            ~CPCPMaterial();

            /** \brief  Get the number of samples gathered along the dual-line
             * \return   the number of samples gathered along the dual-line */
            uint32_t getNbSamples() const {return m_nbSamples;}

            /** \brief            Set the number of samples gathered along the dual-line
             * \param nbSamples   the number of samples gathered along the dual-line */
            void setNbSamples(uint32_t nbSamples) {m_nbSamples = nbSamples;}
        private:
            void initMaterial(const glm::mat4& objMat,  const glm::mat4& cameraMat,
                              const glm::mat4& projMat, const glm::mat4& mvpMat,
                              const glm::mat4& invMVPMat, const glm::vec4& cameraParams);
            void getAttributs();

            uint32_t m_nbSamples; /*!< The nb samples to gather along the dual-line*/

            GLint  m_uNBSamples;

    };
}

#endif
