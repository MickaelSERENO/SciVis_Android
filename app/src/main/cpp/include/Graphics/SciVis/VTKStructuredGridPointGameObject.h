#ifndef  VTKSTRUCTUREDGRIDPOINTGAMEOBJECT_INC
#define  VTKSTRUCTUREDGRIDPOINTGAMEOBJECT_INC

#define GL_GLEXT_PROTOTYPES

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "Graphics/SciVis/SciVis.h"
#include "Graphics/GLRenderer.h"
#include "Graphics/Materials/Material.h"
#include "VTKParser.h"

namespace sereno
{
    class VTKStructuredGridPointGameObject;

    /** \brief  For memory efficiency, this class creates one VBO for all its sub property */
    class VTKStructuredGridPointVBO
    {
        public:
            /** \brief  Constructor, initialize the VBO with enough space
             * \param renderer the OpenGL context
             * \param vtkParser the VTKParser object containing VTK parameters
             * \param nbPtFields the number of points fields 
             * \param desiredDensity the desired dimension*/
            VTKStructuredGridPointVBO(GLRenderer* renderer, std::shared_ptr<VTKParser> vtkParser, uint32_t nbPtFields, uint32_t desiredDensity);

            /** \brief  Destructor, destroy the VBO */
            ~VTKStructuredGridPointVBO();
        private:
            std::shared_ptr<VTKParser> m_vtkParser; /*!< The VTKParser in use*/
            GLuint   m_vboID;                       /*!< The VBO generated. Will be shared among VTKStructuredGridGameObject objects*/
            uint32_t m_dimensions[3];               /*!< The dimension in use*/
            friend class VTKStructuredGridPointGameObject;
    };

    /** \brief  GameObject representing a VTKStructuredGridPointGameObject */
    class VTKStructuredGridPointGameObject : public SciVis
    {
        public:
            /** \brief  Constructor, initialize a VTKStructuredrid GameObject representation for Point field
             * \param renderer The OpenGL Context object
             * \param mtl The material to use
             * \param parent the GameObject parent
             * \param propID the property ID in the VBO (useful for writting at the correct place)
             * \param ptFieldValue the point field value to use
             * \param subDataset the subDataset to use */
            VTKStructuredGridPointGameObject(GameObject* parent, GLRenderer* renderer, Material* mtl, VTKStructuredGridPointVBO* gridPointVBO, uint32_t propID, const VTKFieldValue* ptFieldValue, SubDataset* subDataset);

            ~VTKStructuredGridPointGameObject();

            void setColorRange(float min, float max, ColorMode colorMode);
        private:
            VTKStructuredGridPointVBO* m_gridPointVBO; /*!< The Grid point VBO associated with this data*/
            float*   m_vals;                           /*!< The property captured value*/
            float    m_maxVal;                         /*!< The property max value*/
            float    m_minVal;                         /*!< The property min value*/
            uint32_t m_propID;                         /*!< The property ID*/
            GLuint   m_vaoID;                          /*!< VAO*/
    };
}

#endif
