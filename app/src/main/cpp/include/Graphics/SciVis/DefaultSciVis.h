#ifndef  DEFAULTSCIVIS_INC
#define  DEFAULTSCIVIS_INC

#include "Graphics/SciVis/SciVis.h"
#include "Graphics/Shapes/Cube.h"

namespace sereno
{
    /** \brief  SciVis representing a default (dummy) dataset. */
    class DefaultSciVis : public SciVis
    {
        public:
            /**
             * \brief  Constructor
             * \param parent The parent GameObject 
             * \param renderer the GLRenderer containing the OpenGL Context
             * \param mtl the material to use
             * \paremt model the model to use
             * \param tfTexture the transfert function texture to apply
             * \param tfTextureDim the transfert function texture dimension*/
            DefaultSciVis(GLRenderer* renderer, Material* mtl, GameObject* parent, SubDataset* model, GLuint tfTexture, uint8_t tfTextureDim);

            /** \brief  Destructor, clear graphical objects */
            ~DefaultSciVis();

            void draw(const Render& render);

            void setColorRange(float min, float max) {}
        private:
            GLuint m_vaoID;
            GLuint m_vboID;
            uint32_t m_nbPoints;
    };
}

#endif
