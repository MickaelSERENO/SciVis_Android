#ifndef  MAINVFV_INC
#define  MAINVFV_INC

#include "GLSurfaceViewData.h"
#include "VFVData.h"
#include "image.h"
#include "Graphics/FBORenderer.h"
#include "Graphics/DefaultGameObject.h"
#include "Graphics/Texture.h"
#include "Graphics/DefaultGameObject.h"
#include "Graphics/SciVis/VectorField.h"
#include "Graphics/SciVis/DefaultSciVis.h"
#include "Graphics/SciVis/VTKStructuredGridPointGameObject.h"
#include "Graphics/SciVis/DefaultSciVis.h"
#include "Graphics/Materials/SimpleTextureMaterial.h"
#include "Graphics/Materials/ColorMaterial.h"
#include "Graphics/Materials/ColorGridMaterial.h"
#include "Graphics/Materials/PhongMaterial.h"
#include "Graphics/Materials/NormalizeMaterial.h"
#include "Graphics/Materials/CPCPMaterial.h"
#include "Graphics/Materials/RedToGrayMaterial.h"
#include "Graphics/Materials/UniColorMaterial.h"
#include "Graphics/Materials/CloudPointMaterial.h"
#include "Graphics/VolumeSelect/Lasso.h"
#include "Graphics/SciVis/TransferFunction/TFTexture.h"
#include "Graphics/SciVis/CloudPointGameObject.h"
#include "TransferFunction/GTF.h"
#include "TransferFunction/TriangularGTF.h"

#include <memory>
#include <map>
#include <queue>
#include <algorithm>
#include <glm/glm.hpp>

#define MAX_SNAPSHOT_COUNTER             30
#define VTK_STRUCTURED_POINT_VIS_DENSITY 128
#define WIDGET_WIDTH_PX     64
#define MAX_CAMERA_ANIMATION_TIMER 20

#define HISTOGRAM_HEIGHT 258
#define HISTOGRAM_WIDTH  258

#define CPCP_TEXTURE_WIDTH  512
#define CPCP_TEXTURE_HEIGHT 512

#define VIS_FBO_WIDTH  1024
#define VIS_FBO_HEIGHT 1024

//IDs of the images selected on screen
#define BOTTOM_IMAGE       0
#define LEFT_IMAGE         1
#define TOP_IMAGE          2
#define RIGHT_IMAGE        3
#define TOP_LEFT_IMAGE     4
#define TOP_RIGHT_IMAGE    5
#define BOTTOM_RIGHT_IMAGE 6
#define BOTTOM_LEFT_IMAGE  7
#define NO_IMAGE           8 //Need to be placed at the end

namespace sereno
{
    /** \brief  Type describing a job to run */
    typedef std::function<void()> MainThreadFunc;

    /** Represent the type of changement that is attached to a subdataset*/
    struct SubDatasetChangement
    {
        SubDatasetChangement(bool col = false, bool rot = false, bool sca = false, bool pos = false)
        {
            bool cpy[] = {col, rot, sca, pos};
            for(int i = 0; i < sizeof(cpy)/sizeof(cpy[0]); i++)
                _data[i] = cpy[i];
        }

        SubDatasetChangement(const SubDatasetChangement& cpy)
        {
            *this = cpy;
        }

        SubDatasetChangement& operator=(const SubDatasetChangement& cpy)
        {
            for(int i = 0; i < sizeof(_data)/sizeof(_data[0]); i++)
                _data[i] = cpy._data[i];
            return *this;
        }

        ~SubDatasetChangement()
        {}

        union
        {
            struct
            {
                bool updateColor;    /*!< Has the color been updated ?*/
                bool updateRotation; /*!< Has the rotation been updated ?*/
                bool updateScale;    /*!< Has the scaling been updated ?*/
                bool updatePos;      /*!< Has the position been updated ?*/
            };
            bool _data[4];
        };
    };

    class MainVFV
    {
        public:
            /* \brief Initialize the Main Object for VFV application.
             * \param surfaceData the Java surface data
             * \param nativeWindow the native window to draw on
             * \param mainData the main application data sent via JNI */
            MainVFV(GLSurfaceViewData* surfaceData, ANativeWindow* nativeWindow, VFVData* mainData);

            /* \brief destructor */
            ~MainVFV();

            /* \brief Run the application */
            void run();

            /* \brief  Add a new job to run on 
             * \param func the functor */
            void runOnMainThread(const MainThreadFunc& func);
        private:
            /* \brief  Function called when values of the VTKDataset are loaded
             * \param dataset the dataset of interest
             * \param status the status of the loading */
            void onLoadVTKDataset(VTKDataset* dataset, uint32_t status);

            /* \brief Function called when values of the CloudPointDataset are loaded
             * \param dataset the dataset of interest
             * \param status the status of the loading*/
            void onLoadCloudPointDataset(CloudPointDataset* dataset, uint32_t status);

            /** \brief  Function called when values of a Dataset are loaded
             * \param dataset the dataset of interest
             * \param status the status of the loading */
            void onLoadDataset(Dataset* dataset, uint32_t status);

            /** \brief  Place the widgets on the screen */
            void placeWidgets();

            /* \brief  Find the headset camera transformation if possible
             * \param outPos[out] the headset camera position. NULL if no position required
             * \param outRot[out] the headset camera rotation. NULL if no rotation required
             * \return true if headset camera found, false otherwise. In the later case, no values are modified*/
            bool findHeadsetCameraTransformation(glm::vec3* outPos = NULL, Quaternionf* outRot = NULL);

            /** \brief  Place the 3D Camera in space 
             * \param forceReset should we force the reset ?*/
            void placeCamera(bool forceReset = false);

            /* \brief Handles the touch event 
             * \param event the touch event received*/ 
            void handleTouchAction(TouchEvent* event);

            /** \brief Handles the VFVData event*/
            void handleVFVDataEvent();

            /* \brief  Add a new subdataset changement into the m_modelCHanged list
             * \param sd the subdataset being updated
             * \param sdChangement the changement metadata*/
            void addSubDataChangement(const SubDataset* sd, const SubDatasetChangement& sdChangement);

            /* \brief Create a new visualization from a SubDataset if it does not already exist
             * \param sd the subdataset to add. This function will first check if it has a bound SciVis
             * \return the known SciVis if it already existed, the new one if not*/
            SciVis* createVisualization(SubDataset* sd);

            /* \brief Remove a SubDataset from the visualization
             * \param sd the subdataset to remove. This function will fetch all scivis related to this subdataset*/
            void removeSubDataset(SubDataset* sd);

            /* \brief Remove the sci vis data
             * \param sciVis the sciVis data to remove*/
            void removeSciVis(SciVis* sciVis);

            GLSurfaceViewData* m_surfaceData; /*!< The GL Surface associated with this application */
            VFVData*           m_mainData;    /*!< The main data*/

            MeshLoader*               m_arrowMesh;       /*!< The arrow mesh for the vector fields*/

            /*----------------------------------------------------------------------------*/
            /*------------------------All the Materials being used------------------------*/
            /*----------------------------------------------------------------------------*/
            Material*                 m_vfMtl;            /*!< The vector field material*/
            ColorGridMaterial*        m_colorGridMtl;     /*!< The color grid material for the VTK StructuredGridPoints*/
            SimpleTextureMaterial*    m_3dTextureMtl;     /*!< Material to draw the 3d manip texture objects*/
            SimpleTextureMaterial*    m_notConnectedTextureMtl; /*!< Material to draw the not connected texture object*/
            PhongMaterial*            m_colorPhongMtl;    /*!< Material to draw default scivis gameobjects*/
            NormalizeMaterial*        m_normalizeMtl;     /*!< Material used to normalize a texture*/
            CPCPMaterial*             m_cpcpMtl;          /*!< The Continuous Parallel Coordinate Plot material*/
            RedToGrayMaterial*        m_redToGrayMtl;     /*!< The Red to Gray material*/
            UniColorMaterial*         m_lassoMaterial;    /*!< Material to draw the volume selection lasso*/
            SimpleTextureMaterial*    m_currentVisFBOMtl; /*!< The Material to use for the current vis FBO*/
            CloudPointMaterial*       m_cloudPointMtl;    /*!< The Material to use for cloud point datasets*/

            /*----------------------------------------------------------------------------*/
            /*---------------------------All the SciVis loaded----------------------------*/
            /*----------------------------------------------------------------------------*/
            std::vector<VectorField*> m_vectorFields;    /*!< The loaded vector fields*/
            std::vector<DefaultSciVis*> m_defaultSciVis;   /*!< List of default visualization*/
            std::vector<VTKStructuredGridPointSciVis*> m_vtkStructuredGridPoints; /*!< The VTKStructuredGridPoints visualizations*/
            std::vector<std::shared_ptr<CloudPointDataset>> m_cloudPointDatasets; /*!< The cloud point datasets opened*/
            std::vector<SciVis*> m_sciVis;                 /*!< List of visualization*/
            std::vector<CloudPointGameObject*> m_cloudPointSciVis; /*!< The CloudPoint Scientific Visualization Graphical Object*/
            SciVis*              m_currentVis         = NULL;  /*!< The current visualization*/
            bool                 m_curSDCanBeModified = true;  /*!< Can our current SubDataset be modified?*/
            FBO*                 m_currentVisFBO      = NULL;  /*!< The FBO to render the current visualization in*/
            FBORenderer*         m_currentVisFBORenderer = NULL;  /*!< The FBORenderer to use for the current visualization*/
            DefaultGameObject*   m_currentVisFBOGO    = NULL;  /*!< The Default Game Object for the FBO rendering for the current visualization game object*/

            /*----------------------------------------------------------------------------*/
            /*---------------------------Volume selection data----------------------------*/
            /*----------------------------------------------------------------------------*/

            bool                 m_selecting;
            Lasso*               m_lasso;
            glm::vec3            m_tabletPos;
            Quaternionf          m_tabletRot;
            float                m_tabletScale;

            /*----------------------------------------------------------------------------*/
            /*-------------------------------Snapshot data--------------------------------*/
            /*----------------------------------------------------------------------------*/
            uint32_t             m_snapshotCnt     = 0;    /*!< The snapshot counter*/
            std::map<SciVis*, std::shared_ptr<Snapshot>> m_snapshots; /*!< The snapshot pixels per Scientific Visualization*/

            /*----------------------------------------------------------------------------*/
            /*--------------------------------Texture Data--------------------------------*/
            /*----------------------------------------------------------------------------*/
            TextureRectangleData*  m_gpuTexVBO; /*!< GPU VBO information for drawing textures*/
            Texture2D*             m_3dImageManipTex; /*!< All the textures used by the Widgets used for 3D manipulations*/
            DefaultGameObject*     m_3dImageManipGO;  /*!< 3D manipulation gameobjects widgets*/
            Texture2D*             m_notConnectedTex; /*!< The texture displayed when no headset is bound*/
            DefaultGameObject*     m_notConnectedGO;  /*!< The gameobject widget drawing the not connected texture*/
            uint32_t               m_currentWidgetAction = NO_IMAGE; /*!< The current widget in use*/

            /*----------------------------------------------------------------------------*/
            /*-------------------------------Animation data-------------------------------*/
            /*----------------------------------------------------------------------------*/
            uint32_t               m_animationTimer = 0; /*!< The annimation timer*/
            bool                   m_inAnimation    = false; /*!< Are we in an animation?*/ 
            glm::vec3              m_animationStartingPoint; /*!< The animation starting point*/
            glm::vec3              m_animationEndingPoint;   /*!< The animation ending point*/
            Quaternionf            m_animationRotation;      /*!< The animation rotation to apply (camera rotation)*/

            std::map<const SubDataset*, SubDatasetChangement> m_modelChanged; /*!< Map of the current model being changed*/

            /*----------------------------------------------------------------------------*/
            /*---------------------------------CPCP data----------------------------------*/
            /*----------------------------------------------------------------------------*/
            FBO*         m_rawCPCPFBO;      /*!< The framebuffer containing raw values of the current CPCP */
            FBORenderer* m_cpcpFBORenderer; /*!< The CPCP renderer*/

            /*----------------------------------------------------------------------------*/
            /*------------------------------Main Thread Data------------------------------*/
            /*----------------------------------------------------------------------------*/
            std::queue<MainThreadFunc> m_mainThreadFuncs;      /*!< Jobs to execute in the Main thread*/
            std::mutex                 m_mainThreadFuncsMutex; /*!< Mutex bound to the m_mainThreadFuncs object*/
    };
}
#endif
