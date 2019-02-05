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

            /**
             * \brief  Set the spacing between the point in the grid in OpenGL coordinate system
             * \param spacing the spacing values (x, y, z)
             */
            void setSpacing(const float* spacing)
            {
                for(uint32_t i = 0; i < 3; i++)
                    m_spacing[i] = spacing[i];
            }
        protected:
            void initMaterial(const glm::mat4& objMat, const glm::mat4& cameraMat, 
                              const glm::mat4& mvpMat, const glm::mat4& invMVPMat);
            void getAttributs();

            GLint m_uSpacing   = -1; /*!< Location of variable uSpacing*/
            float m_spacing[3];      /*!< The spacing value to use between points*/

    };
}

#endif
