#ifndef  MESHLOADER_INC
#define  MESHLOADER_INC

#include <lib3ds.h>
#include <string>
#include <vector>
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


    /* \brief This structure permits to load a Mesh into the CPU memory*/
    struct MeshLoader
    {
        public:

            /* \brief Load a mesh from a .3ds file
             * \param path the 3DS file path
             * \return the MeshData*/
            MeshLoader* loadFrom3DS(const std::string& path);

            virtual ~MeshLoader();

            uint32_t  nbSurfaces; /*!< How many surfaces ?*/
            uint32_t  nbVertices; /*!< How many vertices ?*/
            float*    vertices;   /*!< Array of vertices. vertices[i] == x, vertices[i+1] == y, vertices[i+2] == z. Size : nbVertices*3*/
            float*    texels;     /*!< Texels coordinate (UV). Size : nbVertices*2*/
            uint32_t* surfaces;   /*!< Array of surface. Each cell corresponds to a vertices indice. Three cells make a triangle. Size : nbSurfaces*3*/

            std::vector<SubMeshData*> subMeshData; /*!< Sub mesh data information*/
    };
}

#endif
