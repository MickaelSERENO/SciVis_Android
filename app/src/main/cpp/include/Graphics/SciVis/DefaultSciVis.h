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
             * \paremt model the model to use*/
            DefaultSciVis(GLRenderer* renderer, Material* mtl, GameObject* parent, SubDataset* model);

            /** \brief  Destructor, clear graphical objects */
            ~DefaultSciVis();

            void draw(const Render& render);

            /** \brief The default visualization does not depend on the transfer function in use*/
            void onTFChanged() {}
        private:
            GLuint m_vaoID;
            GLuint m_vboID;
            uint32_t m_nbPoints;
    };
}

#endif
