#ifndef DEF_MATERIAL_INCLUDE
#define DEF_MATERIAL_INCLUDE

#include "Color.h"
#include "Graphics/Materials/Material.h"

namespace sereno
{
    /** \brief UniColorMaterial is a material for all objects which need to be colored by an unique color.*/
    class ColorMaterial : public Material
    {
        public:
            /** \brief Constructor associated with a Color object
             * \param renderer the opengl context object*/
            ColorMaterial(GLRenderer* renderer);

            ~ColorMaterial();

        protected:
            void initMaterial(const glm::mat4& objMat,  const glm::mat4& cameraMat,
                              const glm::mat4& projMat, const glm::mat4& mvpMat,
                              const glm::mat4& invMVPMat);
            void getAttributs();

            GLint  m_uUseUniColor;
            GLint  m_uUseTexture;
    };
}

#endif
