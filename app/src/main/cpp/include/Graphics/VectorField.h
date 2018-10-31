#ifndef  VECTORFIELD_INC
#define  VECTORFIELD_INC

#define GL_GLEXT_PROTOTYPES

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "Graphics/GameObject.h"
#include "FluidDataset.h"
#include "MeshLoader.h"
#include "ColorMode.h"
#include "Graphics/Color.h"

#define MAX_VECTOR_ALONG_AXIS 25

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
                        const FluidDataset* dataset, const MeshLoader* arrowLoader);

            /* \brief Destructor*/
            ~VectorField();

            /* \brief Draw on screen the vector field
             * \param cameraMat the camera matrix */
            void draw(const glm::mat4& cameraMat);

            /* \brief Set the color range to display. All the color OUTSIDE [min, max] will be discarded (i.e transparent)
             * \param min the minimum value (ratio : 0.0, 1.0)
             * \param max the maximum value (ratio : 0.0, 1.0)*/
            void setColorRange(const FluidDataset* dataset, float min, float max, ColorMode colorMode);
        private:
            GLuint   m_vaoID;              /*!< Vertex Array Object*/
            GLuint   m_vboID;              /*!< Vertex Buffer Object*/
            uint32_t m_displayableSize[3]; /*!< The displayable size (how many vectors are being displayed ?)*/
            uint32_t m_dataStep;           /*!< The step at reading the data*/
            uint32_t m_nbPoints;           /*!< The number of points to display on screen*/
            uint32_t m_nbVerticesPerArrow; /*!< The number of vertices per arrow*/
    };
}

#endif
