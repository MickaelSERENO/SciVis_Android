#ifndef  MESHDATA_INC
#define  MESHDATA_INC

#define GL_GLEXT_PROTOTYPES

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#include "Graphics/GPUData.h"
#include "Graphics/Drawable.h"
#include "utils.h"
#include "MeshLoader.h"

namespace sereno
{
    class MeshGPUData : public GPUData
    {
        public:
            /* \param meshInformation the mesh information */
            MeshGPUData(MeshLoader* meshInformation);
        private:
            std::vector<SubMeshData*> m_subMeshData; /*!< The subdata information*/
    };

    /* \brief MeshData information. 
     * Contain information about a mesh*/
    class MeshData : public Drawable
    {
        public:
            /* \param meshInformation the mesh information
             * \param renderer object containing opengl context information
             * \param mtl the material being uses*/
            MeshData(MeshGPUData* meshInformation, GLRenderer* renderer, Material* mtl);

            /* \brief Draw the mesh on screen
             * \param cameraMat the camera matrix */
            void draw(const glm::mat4& cameraMat);

        private:
            MeshGPUData* m_meshData = NULL;
    };
}

#endif
