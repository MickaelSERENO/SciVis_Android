#ifndef DEF_UNIMATERIAL_INCLUDE
#define DEF_UNIMATERIAL_INCLUDE

#include "Color.h"
#include "Graphics/Materials/Material.h"

namespace sereno
{
    /** \brief UniColorMaterial is a material for all objects which need to be colored by an unique color.*/
    class UniColorMaterial : public Material
    {
        public:
            /** \brief Constructor associated with a Color object
             * \param renderer the opengl context object
             * \param color the color associated with this material.*/
            UniColorMaterial(GLRenderer* renderer, const Color& color);

            /** \brief Constructor associated with a color component array
             * \param renderer the opengl context object
             * \param color the color associated with this material. The 3 first values must contain (r, g, b) component.*/
            UniColorMaterial(GLRenderer* renderer, const float* color);

            ~UniColorMaterial();

            /** \brief Set the alpha component of the color
             * \param alpha the new value for the alpha component*/
            void setAlpha(float alpha) {m_color[3] = alpha;}

            /** \brief set the color of this material
             * \param color the new color*/
            void setColor(const Color& color);

            /** \brief set the color of this material
             * \param color the (r, g, b) new color array*/
            void setColor(const float* color);

            /** \brief get the color used by this material
             * \return the material color.*/
            Color getColor() const;

            /** \brief get the alpha component value
             * \return the alpha component value*/
            float getAlpha() const{return m_color[3];}

        protected:
            void initMaterial(const glm::mat4& objMat, const glm::mat4& cameraMat, 
                              const glm::mat4& mvpMat, const glm::mat4& invMVPMat);
            void getAttributs();

            float* m_color;
            GLint  m_uColor;
            GLint  m_uUseTexture;
    };
}

#endif
