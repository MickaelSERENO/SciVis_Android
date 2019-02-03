#ifndef  VTKSTRUCTUREDGRIDPOINTGAMEOBJECT_INC
#define  VTKSTRUCTUREDGRIDPOINTGAMEOBJECT_INC

#include "Graphics/SciVis/SciVis.h"
#include "Graphics/GLRenderer.h"
#include "Graphics/Materials/Material.h"
#include "VTKParser.h"

namespace sereno
{
    /** \brief  GameObject representing a VTKStructuredGridPointGameObject */
    class VTKStructuredGridPointGameObject : public SciVis
    {
        public:
            /**
             * \brief  Constructor, initialize a VTKStructuredrid GameObject representation for Point field
             *
             * \param renderer The OpenGL Context object
             * \param mtl The material to use
             * \param parent the GameObject parent
             * \param vtkParser the VTKParser to use
             * \param ptFieldValue the point field value to use
             * \param subDataset the subDataset to use
             */
            VTKStructuredGridGameObject(GLRenderer* renderer, Material* mtl, GameObject* parent, std::shared_ptr<VTKParser> vtkParser, const VTKFieldValue* ptFieldValue, SubDataset* subDataset);

            ~VTKStructuredGridPointGameObject();

            void setColorRange(float min, float max, ColorMOde colorMode);
        private:
            GLuint            m_vaoID;   /*!< VAO*/
            GLuint            m_vboID;   /*!< VBO*/
            const SubDataset* m_dataset; /*!< Pointer to the subdataset in use (for updating representation)*/
    };
}

#endif
