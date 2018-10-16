#ifndef  MESHDATA_INC
#define  MESHDATA_INC

#define GL_GLEXT_PROTOTYPES

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <lib3ds.h>
#include <string>

#include "Graphics/Drawable.h"
#include "utils.h"

namespace sereno
{
    /* \brief SubMesh data. Contain information about partial triangles in the global mesh data */
    struct SubMeshData
    {
        uint32_t    nbVertices;       /*!< How many vertices are concerned about this data ?*/
        float       color[4];         /*!< What are the colors of these vertices ?*/
        std::string textureName = ""; /*!< The texture name*/
    };

    /* \brief MeshData information. 
     * Contain information about a mesh*/
    class MeshData : public Drawable
    {
        public:
            /* \brief Destructor, free OpenGL resources */
            virtual ~MeshData();

            /* \brief Load a mesh from a .3ds file
             * \param parent the Drawable parent
             * \param path the 3DS file path
             * \return the MeshData*/
            MeshData* loadFrom3DS(Drawable* parent, const std::string& path);
        private:
            /* \brief Private constructor. Used the loadFrom* for getting a new meshData */
            MeshData(Drawable* parent);

            GLuint                    m_vboID    = 0;     /*!< The vertex buffer object containing our data (position, UV)*/
            GLuint                    m_eboID    = 0;     /*!< The element buffer object containing the element call*/
            GLuint                    m_vaoID    = 0;     /*!< The vertex array object*/
            bool                      m_glInited = false; /*!< Are the GL objects initialized ?*/
            std::vector<SubMeshData*> m_subMeshData;      /*!< The subdata information*/
    };
}

#endif
