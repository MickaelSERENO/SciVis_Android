#include "MeshLoader.h"

namespace sereno
{
    MeshLoader* MeshLoader::loadFrom3DS(const std::string& path)
    {
        Lib3dsFile* file3ds = lib3ds_file_open(path.c_str());
        if(!file3ds || !file3ds->meshes)
        {
            LOG_ERROR("Could not load properly the file %s\n", path.c_str());
            if(file3ds)
                lib3ds_file_free(file3ds);
            return NULL;
        }

        Lib3dsMesh**     mesh3ds = file3ds->meshes;
        Lib3dsMaterial** mtl3ds  = file3ds->materials;

        uint32_t nbPoints = 0;

        //Go through all the meshes for knowing the number of points and elements
        for(uint32_t i = 0; i < file3ds->nmeshes; i++)
        {
            Lib3dsMesh* subMesh = file3ds->meshes[i];
            nbPoints           += subMesh->nfaces*3;
        }

        //The extracted value : texels, points and elems
        float* texels = (float*)malloc(sizeof(float)*2*nbPoints);
        float* points = (float*)malloc(sizeof(float)*3*nbPoints);
        float* norms  = (float*)malloc(sizeof(float)*3*nbPoints);

        SubMeshData* currentData = NULL;
        MeshLoader* loader = new MeshLoader();
        uint32_t nbCurrentPoints = 0;

        for(uint32_t it = 0; it < file3ds->nmeshes; it++)
        {
            Lib3dsMesh* subMesh = mesh3ds[it];
            const char* oldMaterial = NULL;

            float (*faceNormals)[3] = (float (*)[3])malloc(sizeof(float)*3*3*subMesh->nfaces); //The normals of all the faces
            lib3ds_mesh_calculate_vertex_normals(subMesh, faceNormals);
            
            //Fill the elements array and determine when the material changed
            for(uint32_t i = 0; i < subMesh->nfaces; i++)
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
                    loader->subMeshData.push_back(data);
                }

                currentData->nbVertices += 3;

                //COpy points & normals
                for(uint32_t j = 0; j < 3; j++)
                {
                    for(uint32_t k = 0; k < 3; k++)
                    {
                        uint32_t indice = 9*i + 3*j + k + 3*nbCurrentPoints;
                        norms[indice]   = faceNormals[3*i+j][k];
                        points[indice]  = subMesh->vertices[subMesh->faces[i].index[j]][k];
                    }
                }

                //Copy Texels
                if(subMesh->texcos)
                {
                    for(uint32_t j = 0; j < 2; j++)
                        for(uint32_t k = 0; k < 2; k++)
                        {
                            uint32_t indice = 6*i + 2*j + k + 2*nbCurrentPoints;
                            texels[indice]   = subMesh->texcos[subMesh->faces[i].index[j]][k];
                        }
                }
                else
                    for(uint32_t j = 0; j < 2; j++)
                        for(uint32_t k = 0; k < 2; k++)
                        {
                            uint32_t indice = 6*i + 2*j + k + 2*nbCurrentPoints;
                            texels[indice] = -1;
                        }

                }
            free(faceNormals);

            nbCurrentPoints += subMesh->nfaces*3;
        }

        loader->nbVertices = nbPoints;
        loader->normals    = norms;
        loader->vertices   = points;
        loader->texels     = texels;

        lib3ds_file_free(file3ds);
        return loader;
    }

    MeshLoader::~MeshLoader()
    {
        for(SubMeshData* data : subMeshData)
            free(data);
        free(normals);
        free(vertices);
        free(texels);
    }
}
