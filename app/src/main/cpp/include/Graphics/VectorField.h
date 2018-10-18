#ifndef  VECTORFIELD_INC
#define  VECTORFIELD_INC

#include "Graphics/GameObject.h"
#include "FluidDataset.h"

namespace sereno
{
    /* \brief VectorField GameObject. Draw on screen a VectorField */
    class VectorField : public GameObject
    {
        public:
            /* \brief Constructor.
             * \param renderer the OpenGL Context objects
             * \param parent the parent GameObject
             * \param dataset the dataset to load in the graphic card
             * \param arrowLoader the arrow mesh */
            VectorField(GLRenderer* renderer, Material* mtl, GameObject* parent,
                        FluidDataset* dataset, MeshLoader* arrowLoader);

            /* \brief Destructor*/
            ~VectorField();

            /* \brief Draw on screen the vector field
             * \param cameraMat the camera matrix */
            void onDraw(const glm::mat4& cameraMat);
        private:
            GLint m_vaoID; /*!< Vertex Array Object*/
            GLint m_vboID; /*!< Vertex Buffer Object*/
            GLint m_eboID; /*!< Element Buffer Object*/
    };
}

#endif
