#include "Graphics/MeshData.h"

namespace sereno
{
    MeshData::MeshData(MeshLoader* loader, GLRenderer* renderer, Material* mtl) : Drawable(renderer, mtl), m_subMeshData(loader->subMeshData)
    {
        //Create VAO, VBO and EBO
        glGenVertexArraysOES(1, &m_vaoID);
        glBindVertexArrayOES(m_vaoID);
        {
            //Init the VBO and EBO
            glGenBuffers(1, &m_vboID);
            glGenBuffers(1, &m_eboID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboID);
            glBindBuffer(GL_ARRAY_BUFFER,         m_vboID);
            {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, loader->nbSurfaces*sizeof(uint32_t)*3, loader->vertices, GL_STATIC_DRAW);
                glBufferData(GL_ARRAY_BUFFER, sizeof(float)*(2+3)*loader->nbVertices, NULL, GL_STATIC_DRAW);

                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*loader->nbVertices*3, loader->vertices); //Points
                glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*loader->nbVertices*3, sizeof(float)*loader->nbVertices*2, loader->texels); //UV

                //Set vertex attrib
                glVertexAttribPointer(MATERIAL_VPOSITION,   3, GL_FLOAT, 0, 0, (void*)(0));
                glVertexAttribPointer(MATERIAL_VUV,         2, GL_FLOAT, 0, 0, (void*)(sizeof(float)*loader->nbVertices*3));

                //Enable
                glEnableVertexAttribArray(MATERIAL_VPOSITION);
                glEnableVertexAttribArray(MATERIAL_VUV);
            }
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        glBindVertexArrayOES(0);
    }

    MeshData::~MeshData()
    {
        //Delete buffers + VAO
        glDeleteBuffers(1, &m_vboID);
        glDeleteBuffers(1, &m_eboID);
        glDeleteVertexArraysOES(1, &m_vaoID);
    }

    void MeshData::draw(const glm::mat4& cameraMat)
    {
        
    }
}
