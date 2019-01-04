#ifndef  MAINVFV_INC
#define  MAINVFV_INC

#include "GLSurfaceViewData.h"
#include "VFVData.h"
#include "Graphics/SciVis/VectorField.h"
#include "Graphics/Materials/ColorMaterial.h"

#include <memory>

#define MAX_SNAPSHOT_COUNTER 5

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
            std::vector<VectorField*>  m_vectorFields;          /*!< The loaded vector fields*/
            VectorField*               m_currentVF      = NULL; /*!< The current Vector Field being displayed*/
            uint32_t                   m_snapshotCnt    = 0;    /*!< The snapshot counter*/
            uint32_t*                  m_snapshotPixels = NULL; /*!< The snapshot pixels*/
            uint32_t                   m_snapshotWidth  = 0;    /*!< The snapshot width*/
            uint32_t                   m_snapshotHeight = 0;    /*!< The snapshot height*/
    };
}
#endif
