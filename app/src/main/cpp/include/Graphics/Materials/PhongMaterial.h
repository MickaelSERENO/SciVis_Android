#ifndef  PHONGMATERIAL_INC
#define  PHONGMATERIAL_INC

#include "Color.h"
#include "Graphics/Materials/Material.h"

namespace sereno
{
    class PhongMaterial : public Material
    {
        public:
            /** Initialize a material using phong shading
             * \param renderer the opengl context object 
             * \param color the material's color
             * \param ambientCoeff the ambient coefficient
             * \param diffuseColor the diffuse color component
             * \param specularCoeff the specular coefficient
             * \param shininess the shininess component
             * \param lightColor the light color
             * \param lightDir the light direction*/
            PhongMaterial(GLRenderer* renderer, const Color& color, float ambientCoeff, float diffuseCoeff, float specularCoeff, float shininess, const Color& lightColor = Color::WHITE_COLOR, const glm::vec3& lightDir = glm::vec3(1.0f, -1.0f, 1.0f));

            ~PhongMaterial();

        protected:
            void initMaterial(const glm::mat4& objMat,  const glm::mat4& cameraMat,
                              const glm::mat4& projMat, const glm::mat4& mvpMat,
                              const glm::mat4& invMVPMat, const glm::vec4& cameraParams);
            void getAttributs();

            GLint m_uColor;
            GLint m_uLightColor;
            GLint m_uLightDir;
            GLint m_uPhongCoeffs;

            glm::vec3 m_color;
            glm::vec4 m_phongCoeffs;
            glm::vec3 m_lightColor;
            glm::vec3 m_lightDir;
    };
}

#endif
