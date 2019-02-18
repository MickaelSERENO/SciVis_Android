#ifndef  MESHDATA_INC
#define  MESHDATA_INC

#define GL_GLEXT_PROTOTYPES

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#include "Graphics/Drawable.h"
#include "utils.h"
#include "MeshLoader.h"

namespace sereno
{
    /* \brief MeshData information. 
     * Contain information about a mesh*/
    class MeshData : public Drawable
    {
        public:
            /* \param meshInformation the mesh information */
            /* \param renderer object containing opengl context information
             * \param mtl the material being uses*/
            MeshData(MeshLoader* meshInformation, GLRenderer* renderer, Material* mtl);

            /* \brief Destructor, free OpenGL resources */
            virtual ~MeshData();

            /* \brief Draw the mesh on screen
             * \param cameraMat the camera matrix */
            void draw(const glm::mat4& cameraMat);

            GLuint                    m_vboID    = 0;     /*!< The vertex buffer object containing our data (position, UV)*/
            GLuint                    m_vaoID    = 0;     /*!< The vertex array object*/
            std::vector<SubMeshData*> m_subMeshData;      /*!< The subdata information*/
    };
}

#endif
