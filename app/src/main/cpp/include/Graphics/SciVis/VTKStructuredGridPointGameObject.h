#ifndef  VTKSTRUCTUREDGRIDPOINTGAMEOBJECT_INC
#define  VTKSTRUCTUREDGRIDPOINTGAMEOBJECT_INC

#define GL_GLEXT_PROTOTYPES

#define DIM_PER_PLANE (1.0f/256.0f)

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <mutex>
#include <thread>
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
             * \param subDataset the subDataset to use*/
            VTKStructuredGridPointGameObject(GameObject* parent, GLRenderer* renderer, Material* mtl, 
                                             VTKStructuredGridPointVBO* gridPointVBO, uint32_t propID, SubDataset* subDataset);

            /** \brief  Destructor */
            ~VTKStructuredGridPointGameObject();

            void draw(const Render& render);

            void onTFChanged();

            void load();
        private:
            /* \brief Compute the gradient values
             * \param vals the volume data
             * \param ptsDesc the volume descriptor
             * \param ptFieldValue the field value format*/
            void computeGradient(uint8_t* vals, const VTKStructuredPoints& ptsDesc, const VTKFieldValue* ptFieldValue);

            VTKStructuredGridPointVBO* m_gridPointVBO; /*!< The Grid point VBO associated with this data*/
            GLuint   m_texture;                        /*!< The 3D texture containing the dataset values*/
            int      m_glVersion = -1;                 /*!< The OpenGL version*/
            float    m_maxVal;                         /*!< The property max value*/
            float    m_minVal;                         /*!< The property min value*/
            uint32_t m_propID;                         /*!< The property ID*/
            GLuint   m_vaoID;                          /*!< VAO*/
            uint8_t* m_newCols = nullptr;              /*!< The new colors computed in a separate thread to update the 3D image*/
            bool     m_isWaiting3DImage = false;       /*!< Is the object waiting to compute a 3D image?*/
            std::mutex m_updateTFLock;                 /*!< The mutex locking the updateTF call*/
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
         * \param desiredDensity the desired displayed density*/
        VTKStructuredGridPointSciVis(GLRenderer* renderer, Material* material, std::shared_ptr<VTKDataset> d, uint32_t desiredDensity);

        /** \brief  Destructor */
        ~VTKStructuredGridPointSciVis();

        VTKStructuredGridPointVBO*                     vbo;         /*!< The VBO to use*/
        std::shared_ptr<VTKDataset>                    dataset;     /*!< The dataset bound*/
        std::vector<VTKStructuredGridPointGameObject*> gameObjects; /*!< The gameObjects created.*/
    };
}

#endif
