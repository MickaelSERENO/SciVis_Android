#ifndef  MAINVFV_INC
#define  MAINVFV_INC

#include "GLSurfaceViewData.h"
#include "VFVData.h"
#include "Graphics/SciVis/VectorField.h"
#include "Graphics/SciVis/VTKStructuredGridPointGameObject.h"
#include "Graphics/Materials/ColorMaterial.h"
#include "Graphics/Materials/ColorGridMaterial.h"
#include "Graphics/SciVis/TransfertFunction/TransfertFunction.h"
#include "Graphics/SciVis/TransfertFunction/GTF.h"
#include "Graphics/SciVis/TransfertFunction/TriangularGTF.h"
#include "Graphics/SciVis/TransfertFunction/DefaultTF.h"

#include <memory>
#include <map>

#define MAX_SNAPSHOT_COUNTER             30
#define VTK_STRUCTURED_POINT_VIS_DENSITY 128

#define SCIVIS_TF_COLOR(_, __, ___)\
    _(RAINBOW, __, ___)            \
    _(GRAYSCALE, __, ___)          \
    _(WARM_COLD_CIELAB, __, ___)   \
    _(WARM_COLD_CIELUV, __, ___)   \
    _(WARM_COLD_MSH, __, ___)      \

#define SCIVIS_TF_CLASS(_, __)\
    _(TriangularGTF, 2, __)   \
    _(DefaultTF, 1, __)       \

//SciVis Enum
#define DEFINE_ENUM_SCIVIS_BODY(colorName, className, Dim)\
    colorName##_##className,

#define DEFINE_ENUM_SCIVIS_BODY_(className, Dim, listColor)\
    listColor(DEFINE_ENUM_SCIVIS_BODY, className, Dim)

#define DEFINE_ENUM_SCIVIS_TF(listColor, listClass)   \
    enum SciVisTFEnum                                 \
    {                                                 \
        listClass(DEFINE_ENUM_SCIVIS_BODY_, listColor)\
        SciVisTFEnum_End                              \
    };                                                \

//SciVis gen textures
#define DEFINE_GEN_SCIVIS_TF_TEXTURE_CASE(colorName, className, Dim)\
    case colorName##_##className:                                   \
        return TFTexture<className<Dim, colorName>>::generateTexture(texSize, className<Dim, colorName>());

#define DEFINE_GEN_SCIVIS_TF_TEXTURE_CASE_(className, Dim, listColor) \
    listColor(DEFINE_GEN_SCIVIS_TF_TEXTURE_CASE, className, Dim)

#define DEFINE_GEN_SCIVIS_TF_TEXTURE(listColor, listClass)              \
    inline GLuint sciVisTFGenTexture(SciVisTFEnum e, uint32_t* texSize) \
    {                                                                   \
        switch(e)                                                       \
        {                                                               \
            listClass(DEFINE_GEN_SCIVIS_TF_TEXTURE_CASE_, listColor)    \
            default:                                                    \
                return 0;                                               \
        }                                                               \
    }

//SciVis gen get dimension
#define DEFINE_SCIVIS_GET_DIM_CASE(colorName, className, Dim)\
    case colorName##_##className:                            \
        return Dim;

#define DEFINE_SCIVIS_GET_DIM_CASE_(className, Dim, listColor)\
    listColor(DEFINE_SCIVIS_GET_DIM_CASE, className, Dim)

#define DEFINE_SCIVIS_GET_DIM(listColor, listClass)          \
    inline uint8_t sciVisTFGetDimension(SciVisTFEnum e)      \
    {                                                        \
        switch(e)                                            \
        {                                                    \
            listClass(DEFINE_SCIVIS_GET_DIM_CASE_, listColor)\
            default:                                         \
                return 0;                                    \
        }                                                    \
    }

namespace sereno
{
    //Generate sereno::SciVisTFEnum
    //They are composing of every entry of SCIVIS_TF_COLOR and SCIVIS_TF_CLASS :
    //color_class (with color and class at the correct value)
    DEFINE_ENUM_SCIVIS_TF(SCIVIS_TF_COLOR, SCIVIS_TF_CLASS)

    //Generate sereno::sciVisTFGenTexture
    DEFINE_GEN_SCIVIS_TF_TEXTURE(SCIVIS_TF_COLOR, SCIVIS_TF_CLASS)

    //Generate sereno::sciVisTFGetDimension
    DEFINE_SCIVIS_GET_DIM(SCIVIS_TF_COLOR, SCIVIS_TF_CLASS)

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

            MeshLoader*                m_arrowMesh;    /*!< The arrow mesh for the vector fields*/
            Material*                  m_vfMtl;        /*!< The vector field material*/
            ColorGridMaterial*         m_colorGridMtl; /*!< The color grid material for the VTK StructuredGridPoints*/
            std::vector<VectorField*>  m_vectorFields; /*!< The loaded vector fields*/
            std::vector<GLuint>        m_sciVisTFs;    /*!< The TF texture used for Scientific Visualization*/

            std::vector<VTKStructuredGridPointSciVis*> m_vtkStructuredGridPoints; /*!< The VTKStructuredGridPoints visualizations*/
            std::map<SciVis*, SciVisTFEnum>            m_sciVisDefaultTF;         /*!< Mapping between SciVis and starting of TF used (e.g., RAINBOW_GTF for all GTF TFs)*/

            std::vector<SciVis*>         m_sciVis;                 /*!< List of visualization*/
            SciVis*                      m_currentVis      = NULL; /*!< The current visualization*/
            uint32_t                     m_snapshotCnt     = 0;    /*!< The snapshot counter*/
            std::map<SciVis*, std::shared_ptr<Snapshot>> m_snapshots; /*!< The snapshot pixels per Scientific Visualization*/

            typedef std::pair<SciVis*, SciVisTFEnum> SciVisPair;
    };
}
#endif
