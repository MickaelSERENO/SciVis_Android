#ifndef  MAINVFV_INC
#define  MAINVFV_INC

#include "GLSurfaceViewData.h"
#include "VFVData.h"
#include "Graphics/SciVis/VectorField.h"
#include "Graphics/SciVis/VTKStructuredGridPointGameObject.h"
#include "Graphics/Materials/ColorMaterial.h"
#include "Graphics/Materials/ColorGridMaterial.h"

#include <memory>

#define MAX_SNAPSHOT_COUNTER             5
#define VTK_STRUCTURED_POINT_VIS_DENSITY 64

namespace sereno
{
    class MainVFV : public IVFVCallback
    {
        public:
            /* \brief Initialize the Main Object for VFV application.
             * \param surfaceData the Java surface data
             * \param mainData the main application data sent via JNI */
            MainVFV(GLSurfaceViewData* surfaceData, VFVData* mainData);

            /* \brief destructor */
            ~MainVFV();

            /* \brief Run the application */
            void run();

            /* \brief Callback called before a data is being removed. 
             * Pay attention that this function is asynchronous 
             * \param dataPath the data path */
            void onRemoveData(const std::string& dataPath);

            /* \brief Callback called when a data has been added
             * Pay attention that this function is asynchronous 
             * \param dataPath the data path */
            void onAddData(const std::string& dataPath);
        private:
            GLSurfaceViewData* m_surfaceData; /*!< The GL Surface associated with this application */
            VFVData*           m_mainData;    /*!< The main data*/

            MeshLoader*                m_arrowMesh;             /*!< The arrow mesh for the vector fields*/
            ColorMaterial*             m_arrowMtl;              /*!< The arrow material for the vector fields*/
            ColorGridMaterial*         m_colorGridMtl;          /*!< The color grid material for the VTK StructuredGridPoints*/
            std::vector<VectorField*>  m_vectorFields;          /*!< The loaded vector fields*/

            std::vector<VTKStructuredGridPointSciVis*> m_vtkStructuredGridPoints; /*!< The VTKStructuredGridPoints visualizations*/

            std::vector<SciVis*>       m_sciVis;                /*!< List of visualization*/
            SciVis*                    m_currentVis     = NULL; /*!< The current visualization*/
            uint32_t                   m_snapshotCnt    = 0;    /*!< The snapshot counter*/
            uint32_t*                  m_snapshotPixels = NULL; /*!< The snapshot pixels*/
            uint32_t                   m_snapshotWidth  = 0;    /*!< The snapshot width*/
            uint32_t                   m_snapshotHeight = 0;    /*!< The snapshot height*/
    };
}
#endif
