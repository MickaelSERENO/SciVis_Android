#ifndef  COLORGRIDMATERIAL_INC
#define  COLORGRIDMATERIAL_INC

#include "Graphics/Materials/Material.h"

namespace sereno
{
    class ColorGridMaterial : public Material
    {
        public:
            /** \brief Constructor associated with a Color object
             * \param renderer the opengl context object*/
            ColorGridMaterial(GLRenderer* renderer);

            /** \brief  Destructor, does nothing */
            ~ColorGridMaterial();

            /* \brief  Set the spacing between the point in the grid in OpenGL coordinate system
             * \param spacing the spacing values (x, y, z)*/
            void setSpacing(const float* spacing)
            {
                for(uint32_t i = 0; i < 3; i++)
                    m_spacing[i] = spacing[i];
            }

            /* \brief  Set the dimension of the grid in OpenGL coordinate system
             * \param dimension the dimension values(x, y, z)*/
            void setDimension(const float* dimension)
            {
                for(uint32_t i = 0; i < 3; i++)
                    m_dimension[i] = dimension[i];
            }
        protected:
            void initMaterial(const glm::mat4& objMat,    const glm::mat4& cameraMat,
                              const glm::mat4& projMat,   const glm::mat4& mvpMat,
                              const glm::mat4& invMVPMat, const glm::vec4& cameraParams);

            void getAttributs();

            GLint m_uSpacing   = -1; /*!< Location of variable uSpacing*/
            GLint m_uDimension = -1; /*!< Location of variable uDimension*/
            float m_spacing[3];      /*!< The spacing value to use between points*/
            float m_dimension[3];    /*!< The dimension of the whole grid object*/

    };
}

#endif
