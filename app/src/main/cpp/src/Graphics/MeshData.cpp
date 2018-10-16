#include "Graphics/MeshData.h"

namespace sereno
{
    MeshData::MeshData(Drawable* parent) : Drawable(parent, NULL)
    {}

    MeshData::~MeshData()
    {
        //Delete buffers + VAO
        glDeleteBuffers(1, &m_vboID);
        glDeleteBuffers(1, &m_eboID);
        glDeleteVertexArraysOES(1, &m_vaoID);
    }

    MeshData* MeshData::loadFrom3DS(Drawable* parent, const std::string& path)
    {
        Lib3dsFile* file3ds = lib3ds_file_open(path.c_str());
        if(!file3ds->meshes)
        {
            LOG_ERROR("Could not load properly the file %s\n", path.c_str());
            lib3ds_file_free(file3ds);
            return NULL;
        }

        Lib3dsMesh**     mesh3ds = file3ds->meshes;
        Lib3dsMaterial** mtl3ds  = file3ds->materials;

        uint32_t nbElems  = 0;
        uint32_t nbPoints = 0;

        //Go through all the meshes for knowing the number of points and elements
        for(uint32_t i = 0; i < file3ds->nmeshes; i++)
        {
            Lib3dsMesh* subMesh = file3ds->meshes[i];
            nbElems            += subMesh->nfaces;
            nbPoints           += subMesh->nvertices;
        }

        //The extracted value : texels, points and elems
        float* texels = (float*)malloc(sizeof(float)*2*nbPoints);
        float* points = (float*)malloc(sizeof(float)*3*nbPoints);
        int*   elems  = (int*)malloc(sizeof(int)*nbElems*3);

        nbElems  = 0;
        nbPoints = 0;
        SubMeshData* currentData = NULL;

        for(uint32_t it = 0; it < file3ds->nmeshes; it++)
        {
            Lib3dsMesh* subMesh = file3ds->meshes[it];
            const char* oldMaterial = NULL;

            //Fill the elements array and determine when the material changed
            for(uint32_t i = 0; i < subMesh->nfaces; i++, nbElems++)
            {
                //If the material has changed, recreate a sub data
                if(oldMaterial == NULL || std::string(oldMaterial) != mtl3ds[subMesh->faces[i].material]->name)
                {
                    //Create and fill the new 3DS internal data
                    SubMeshData* data = (SubMeshData*)malloc(sizeof(SubMeshData));
                    currentData       = data;
                    data->nbVertices  = 0;

                    //Work with the material
                    Lib3dsMaterial* mtl = mtl3ds[subMesh->faces[i].material];

                    //Copy only the diffuse color. If we want a more complicate draw model, we shall update our internal data
                    for(uint8_t j = 0; j < 3; j++)
                        data->color[j] = mtl->diffuse[j];
                    data->color[3] = 1.0;

                    //Look for the texture
                    if(mtl->texture1_map.name[0])
                        data->textureName = mtl->texture1_map.name[0];

                    //Remember this material.
                    oldMaterial = mtl->name;

                    //Prepend to our internal data list
                    m_subMeshData.push_back(data);
                }

                currentData->nbVertices += 3;

                for(uint32_t j = 0; j < 3; j++)
                    elems[j + 3*nbElems] = subMesh->faces[i].index[j];
            }

            //Copy Positions
            for(uint32_t i = 0; i < subMesh->nvertices; i++)
                for(uint32_t j = 0; j < 3; j++)
                    points[3*i+j + nbPoints*3] = subMesh->vertices[i][j];

            //Copy Texels
            if(subMesh->texcos)
            {
                for(uint32_t i = 0; i < subMesh->nvertices; i++)
                    for(uint32_t j = 0; j < 2; j++)
                        texels[2*i+j + 2*nbPoints] = subMesh->texcos[i][j];
            }
            else
                for(uint32_t i = 0; i < subMesh->nvertices; i++)
                    for(uint32_t j = 0; j < 3; j++)
                        texels[2*i+j + 2*nbPoints] = -1;

            nbPoints += subMesh->nvertices;
        }

        MeshData* data   = new MeshData(parent);
        data->m_glInited = true;

        //Create VAO, VBO and EBO
        glGenVertexArraysOES(1, &data->m_vaoID);
        glBindVertexArrayOES(data->m_vaoID);
        {
            //Init the VBO and EBO
            glGenBuffers(1, &data->m_vboID);
            glGenBuffers(1, &data->m_eboID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->m_eboID);
            glBindBuffer(GL_ARRAY_BUFFER,         data->m_vboID);
            {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, nbElems*sizeof(int)*3, elems, GL_STATIC_DRAW);
                glBufferData(GL_ARRAY_BUFFER, sizeof(float)*(2+3)*nbPoints, NULL, GL_STATIC_DRAW);

                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*nbPoints*3, points);                           //Points
                glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*nbPoints*3,    sizeof(float)*nbPoints*2, texels); //UV

                //Set vertex attrib
                glVertexAttribPointer(MATERIAL_VPOSITION,   3, GL_FLOAT, 0, 0, (void*)(0));
                glVertexAttribPointer(MATERIAL_VUV,         2, GL_FLOAT, 0, 0, (void*)(sizeof(float)*nbPoints*3));

                //Enable
                glEnableVertexAttribArray(MATERIAL_VPOSITION);
                glEnableVertexAttribArray(MATERIAL_VUV);
            }
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        glBindVertexArrayOES(0);

        //Free everything
        free(texels);
        free(points);
        free(elems);
        lib3ds_file_free(file3ds);

        return data;
    }
}
