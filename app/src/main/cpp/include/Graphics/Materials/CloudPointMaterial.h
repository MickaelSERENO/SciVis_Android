#ifndef  CLOUDPOINTMATERIAL_INC
#define  CLOUDPOINTMATERIAL_INC

#include "Graphics/Materials/Material.h"

namespace sereno
{
    /** \brief  Cloud Point Material, used to display point datasets. Each point will be displayed as a cube with a given programable size */
    class CloudPointMaterial : public Material
    {
        public:
            /** \brief  Constructor associated to a cloud point objects (i.e., objects relying on points)
             * \param renderer the opengl context object
             * \param pointSize the size of the cubes created, in local space, for each point */
            CloudPointMaterial(GLRenderer* renderer, float pointSize=0.05f);

            /** \brief  Destructor */
            ~CloudPointMaterial();

            /** \brief  Set the cube size for each primitive point
             * \param pointSize the new cube size to use per point */
            void setPointSize(float pointSize) {m_pointSize = pointSize;}

            /** \brief  Get the cube size for each primitive point
             * \return the cube size in use per point */
            float getPointSize() const {return m_pointSize;}

        protected:
            void initMaterial(const glm::mat4& objMat,  const glm::mat4& cameraMat,
                              const glm::mat4& projMat, const glm::mat4& mvpMat,
                              const glm::mat4& invMVPMat, const glm::vec4& cameraParams);
            void getAttributs();

            float m_pointSize;  /*!< The point size in use*/
            GLint m_uPointSize; /*!< The uniform attribute of "uPointSize"*/
    };
}

#endif
