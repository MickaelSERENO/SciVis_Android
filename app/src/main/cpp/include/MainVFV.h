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

#define MAX_SNAPSHOT_COUNTER             5
#define VTK_STRUCTURED_POINT_VIS_DENSITY 128

#define SCIVIS_TF_COLOR(_, __)\
    _(RAINBOW, __)            \
    _(GRAYSCALE, __)          \
    _(WARM_COLD_CIELAB, __)   \
    _(WARM_COLD_CIELUV, __)   \
    _(WARM_COLD_MSH, __)      \

#define SCIVIS_TF_CLASS(_, __)\
    _(TriangularGTF, 2, __)   \
    _(DefaultTF, 1, __)       \

//SciVis Enum
#define DEFINE_ENUM_SCIVIS_BODY(className, Dim, colorName)\
    colorName##_##className,

#define DEFINE_ENUM_SCIVIS_BODY_(colorName, listClass)\
    listClass(DEFINE_ENUM_SCIVIS_BODY, colorName)

#define DEFINE_ENUM_SCIVIS_TF(listColor, listClass)   \
    enum SciVisTFEnum                                 \
    {                                                 \
        listColor(DEFINE_ENUM_SCIVIS_BODY_, listClass)\
        SciVisTFEnum_End                              \
    };                                                \

//SciVis gen textures
#define DEFINE_GEN_SCIVIS_TF_TEXTURE_CASE(className, Dim, colorName)\
    case colorName##_##className:                                   \
        return TFTexture<className<Dim, colorName>>::generateTexture(texSize, className<Dim, colorName>());

#define DEFINE_GEN_SCIVIS_TF_TEXTURE_CASE_(colorName, listClass) \
    listClass(DEFINE_GEN_SCIVIS_TF_TEXTURE_CASE, colorName)

#define DEFINE_GEN_SCIVIS_TF_TEXTURE(listColor, listClass)              \
    inline GLuint sciVisTFGenTexture(SciVisTFEnum e, uint32_t* texSize) \
    {                                                                   \
        switch(e)                                                       \
        {                                                               \
            listColor(DEFINE_GEN_SCIVIS_TF_TEXTURE_CASE_, listClass)    \
            default:                                                    \
                return 0;                                               \
        }                                                               \
    }

//SciVis gen get dimension
#define DEFINE_SCIVIS_GET_DIM_CASE(className, Dim, colorName)\
    case colorName##_##className:                            \
        return Dim;

#define DEFINE_SCIVIS_GET_DIM_CASE_(colorName, listClass)\
    listClass(DEFINE_SCIVIS_GET_DIM_CASE, colorName)

#define DEFINE_SCIVIS_GET_DIM(listColor, listClass)          \
    inline uint8_t sciVisTFGetDimension(SciVisTFEnum e)      \
    {                                                        \
        switch(e)                                            \
        {                                                    \
            listColor(DEFINE_SCIVIS_GET_DIM_CASE_, listClass)\
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

            MeshLoader*                m_arrowMesh;             /*!< The arrow mesh for the vector fields*/
            Material*                  m_vfMtl;                 /*!< The vector field material*/
            ColorGridMaterial*         m_colorGridMtl;          /*!< The color grid material for the VTK StructuredGridPoints*/
            std::vector<VectorField*>  m_vectorFields;          /*!< The loaded vector fields*/

            std::vector<VTKStructuredGridPointSciVis*> m_vtkStructuredGridPoints; /*!< The VTKStructuredGridPoints visualizations*/
            std::vector<GLuint>        m_sciVisTFs;

            std::vector<SciVis*>       m_sciVis;                /*!< List of visualization*/
            SciVis*                    m_currentVis     = NULL; /*!< The current visualization*/
            uint32_t                   m_snapshotCnt    = 0;    /*!< The snapshot counter*/
            uint32_t*                  m_snapshotPixels = NULL; /*!< The snapshot pixels*/
            uint32_t                   m_snapshotWidth  = 0;    /*!< The snapshot width*/
            uint32_t                   m_snapshotHeight = 0;    /*!< The snapshot height*/
    };
}
#endif
