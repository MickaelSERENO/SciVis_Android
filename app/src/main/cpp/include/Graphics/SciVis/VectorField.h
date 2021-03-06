#ifndef  VECTORFIELD_INC
#define  VECTORFIELD_INC

#define GL_GLEXT_PROTOTYPES

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <memory>
#include "SciVis.h"
#include "Datasets/VectorFieldDataset.h"
#include "MeshLoader.h"
#include "ColorMode.h"
#include "Color.h"

#define MAX_VECTOR_ALONG_AXIS 25

namespace sereno
{
    /* \brief VectorField GameObject. Draw on screen a VectorField */
    class VectorField : public SciVis
    {
        public:
            /* \brief Constructor.
             * \param renderer the OpenGL Context objects
             * \param mtl The material to use
             * \param parent the parent GameObject
             * \param dataset the dataset to load in the graphic card
             * \param arrowLoader the arrow mesh */
            VectorField(GLRenderer* renderer, Material* mtl, GameObject* parent,
                        const std::shared_ptr<VectorFieldDataset> dataset, const MeshLoader* arrowLoader);

            /* \brief Destructor*/
            ~VectorField();

            /* \brief Get the binary dataset model bound to this vector field
             * \return the binary dataset model of this VectorField*/
            std::shared_ptr<VectorFieldDataset> getModel() const {return m_data;}

            void draw(const Render& render);

            void onTFChanged();
        private:
            const std::shared_ptr<VectorFieldDataset> m_data = NULL; /*!< The fluid dataset model*/

            GLuint        m_vaoID;                         /*!< Vertex Array Object*/
            GLuint        m_vboID;                         /*!< Vertex Buffer Object*/
            uint32_t      m_displayableSize[3];            /*!< The displayable size (how many vectors are being displayed ?)*/
            uint32_t      m_dataStep;                      /*!< The step at reading the data*/
            uint32_t      m_nbPoints;                      /*!< The number of points to display on screen*/
            uint32_t      m_nbVerticesPerArrow;            /*!< The number of vertices per arrow*/
    };
}

#endif
