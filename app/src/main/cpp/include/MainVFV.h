#ifndef  MAINVFV_INC
#define  MAINVFV_INC

#include "GLSurfaceViewData.h"
#include "VFVData.h"
#include "image.h"
#include "Graphics/Texture.h"
#include "Graphics/DefaultGameObject.h"
#include "Graphics/SciVis/VectorField.h"
#include "Graphics/SciVis/VTKStructuredGridPointGameObject.h"
#include "Graphics/Materials/SimpleTextureMaterial.h"
#include "Graphics/Materials/ColorMaterial.h"
#include "Graphics/Materials/ColorGridMaterial.h"
#include "Graphics/SciVis/TransferFunction/TFTexture.h"
#include "TransferFunction/GTF.h"
#include "TransferFunction/TriangularGTF.h"

#include <memory>
#include <map>

#define MAX_SNAPSHOT_COUNTER             30
#define VTK_STRUCTURED_POINT_VIS_DENSITY 128
#define WIDGET_WIDTH_PX     64
#define MAX_CAMERA_ANIMATION_TIMER 20

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
        private:
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

            /* \brief Remove the sci vis data
             * \param sciVis the sciVis data to remove*/
            void removeSciVis(SciVis* sciVis);

            GLSurfaceViewData* m_surfaceData; /*!< The GL Surface associated with this application */
            VFVData*           m_mainData;    /*!< The main data*/

            MeshLoader*               m_arrowMesh;       /*!< The arrow mesh for the vector fields*/
            Material*                 m_vfMtl;           /*!< The vector field material*/
            ColorGridMaterial*        m_colorGridMtl;    /*!< The color grid material for the VTK StructuredGridPoints*/
            SimpleTextureMaterial*    m_3dTextureMtl;    /*!< Material to draw the 3d manip texture objects*/
            SimpleTextureMaterial*    m_notConnectedTextureMtl; /*!< Material to draw the not connected texture object*/
            std::vector<VectorField*> m_vectorFields;    /*!< The loaded vector fields*/
            std::vector<GLuint>       m_sciVisTFTextures;/*!< The TF texture used for Scientific Visualization*/

            std::vector<VTKStructuredGridPointSciVis*> m_vtkStructuredGridPoints; /*!< The VTKStructuredGridPoints visualizations*/
            std::vector<SciVis*> m_sciVis;                 /*!< List of visualization*/
            SciVis*              m_currentVis      = NULL; /*!< The current visualization*/
            uint32_t             m_snapshotCnt     = 0;    /*!< The snapshot counter*/

            std::map<SciVis*, std::shared_ptr<Snapshot>> m_snapshots; /*!< The snapshot pixels per Scientific Visualization*/
            std::map<SubDataset*, TF*> m_sciVisTFs; /*!< The subdataset personal transfer function*/

            TextureRectangleData*  m_gpuTexVBO; /*!< GPU VBO information for drawing textures*/
            Texture*               m_3dImageManipTex; /*!< All the textures used by the Widgets used for 3D manipulations*/
            DefaultGameObject*     m_3dImageManipGO;  /*!< 3D manipulation gameobjects widgets*/
            Texture*               m_notConnectedTex; /*!< The texture displayed when no headset is bound*/
            DefaultGameObject*     m_notConnectedGO;  /*!< The gameobject widget drawing the not connected texture*/
            uint32_t               m_currentWidgetAction = NO_IMAGE; /*!< The current widget in use*/

            uint32_t               m_animationTimer = 0; /*!< The annimation timer*/
            bool                   m_inAnimation    = false; /*!< Are we in an animation?*/ 
            glm::vec3              m_animationStartingPoint; /*!< The animation starting point*/
            glm::vec3              m_animationEndingPoint;   /*!< The animation ending point*/
            Quaternionf            m_animationRotation;      /*!< The animation rotation to apply (camera rotation)*/

            std::map<const SubDataset*, SubDatasetChangement> m_modelChanged; /*!< Map of the current model being changed*/
    };
}
#endif
