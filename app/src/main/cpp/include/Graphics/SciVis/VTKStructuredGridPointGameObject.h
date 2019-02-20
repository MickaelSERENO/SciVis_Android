#ifndef  VTKSTRUCTUREDGRIDPOINTGAMEOBJECT_INC
#define  VTKSTRUCTUREDGRIDPOINTGAMEOBJECT_INC

#define GL_GLEXT_PROTOTYPES

#define DIM_PER_PLANE (1.0f/256.0f)

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include "Graphics/SciVis/SciVis.h"
#include "Graphics/GLRenderer.h"
#include "Graphics/Materials/Material.h"
#include "Datasets/VTKDataset.h"
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

            /**
             * \brief  Get the spacing between each point
             * \return the spacing between each point (3 float values)
             */
            const float*    getSpacing() {return m_spacing;}

            /**
             * \brief  Get the grid dimensions
             * \return the grid dimensions (3 integer values)
             */
            const uint32_t* getDimensions() {return m_dimensions;}
        private:
            std::shared_ptr<VTKParser> m_vtkParser; /*!< The VTKParser in use*/
            GLuint   m_vboID;                       /*!< The VBO generated. Will be shared among VTKStructuredGridGameObject objects*/
            uint32_t m_dimensions[3];               /*!< The dimension in use*/
            float    m_spacing[3];                  /*!< The space between each point*/
            uint32_t m_nbPlanes;                    /*!< The number of planes this object can handle */
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
             * \param subDataset the subDataset to use 
             * \param tfTexture the transfert function texture to apply
             * \param tfTextureDim the transfert function texture dimension*/
            VTKStructuredGridPointGameObject(GameObject* parent, GLRenderer* renderer, Material* mtl, 
                                             VTKStructuredGridPointVBO* gridPointVBO, uint32_t propID, 
                                             const VTKFieldValue* ptFieldValue, SubDataset* subDataset, 
                                             GLuint tfTexture, uint8_t tfTextureDim);

            /** \brief  Destructor */
            ~VTKStructuredGridPointGameObject();

            void draw(const glm::mat4& cameraMat);

            void setColorRange(float min, float max, ColorMode colorMode);
        private:
            VTKStructuredGridPointVBO* m_gridPointVBO; /*!< The Grid point VBO associated with this data*/
            float*   m_vals;                           /*!< The property captured value*/
            GLuint   m_texture;                        /*!< The 3D texture containing the dataset values*/
            float    m_maxVal;                         /*!< The property max value*/
            float    m_minVal;                         /*!< The property min value*/
            uint32_t m_propID;                         /*!< The property ID*/
            GLuint   m_vaoID;                          /*!< VAO*/
    };

    /** \brief  Structure regrouping every information needed for VTKStructuredGridPoint visualization */
    struct VTKStructuredGridPointSciVis
    {
        /**
         * \brief  Constructor
         *
         * \param renderer The OpenGL Context object
         * \param mtl The material to use
         * \param d the dataset to use
         * \param desiredDensity the desired displayed density
         * \param tfTexture the transfert function texture to apply
         * \param tfTextureDim the transfert function texture dimension*/
        VTKStructuredGridPointSciVis(GLRenderer* renderer, Material* material, std::shared_ptr<VTKDataset> d, 
                                     uint32_t desiredDensity, GLuint tfTexture, uint8_t tfTextureDim);

        /** \brief  Destructor */
        ~VTKStructuredGridPointSciVis();

        VTKStructuredGridPointVBO*         vbo;         /*!< The VBO to use*/
        std::shared_ptr<VTKDataset>        dataset;     /*!< The dataset bind*/
        VTKStructuredGridPointGameObject** gameObjects; /*!< The gameObjects created. No parent assigned yet*/
    };
}

#endif
