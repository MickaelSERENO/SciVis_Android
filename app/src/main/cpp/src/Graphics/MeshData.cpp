#include "Graphics/MeshData.h"

namespace sereno
{
    MeshData::MeshData(MeshLoader* loader, GLRenderer* renderer, Material* mtl) : Drawable(renderer, mtl), m_subMeshData(loader->subMeshData)
    {
        //Create VAO, VBO
        glGenVertexArrays(1, &m_vaoID);
        glBindVertexArray(m_vaoID);
        {
            //Init the VBO
            glGenBuffers(1, &m_vboID);
            glBindBuffer(GL_ARRAY_BUFFER,         m_vboID);
            {
                glBufferData(GL_ARRAY_BUFFER, sizeof(float)*(2+3)*loader->nbVertices, NULL, GL_STATIC_DRAW);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*loader->nbVertices*3, loader->vertices); //Points
                glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*loader->nbVertices*3, sizeof(float)*loader->nbVertices*2, loader->texels); //UV

                //Set vertex attrib
                glVertexAttribPointer(MATERIAL_VPOSITION,   3, GL_FLOAT, 0, 0, (void*)(0));
                glVertexAttribPointer(MATERIAL_VUV0,        2, GL_FLOAT, 0, 0, (void*)(sizeof(float)*loader->nbVertices*3));

                //Enable
                glEnableVertexAttribArray(MATERIAL_VPOSITION);
                glEnableVertexAttribArray(MATERIAL_VUV0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        glBindVertexArray(0);
    }

    MeshData::~MeshData()
    {
        //Delete buffers + VAO
        glDeleteBuffers(1, &m_vboID);
        glDeleteVertexArrays(1, &m_vaoID);
    }

    void MeshData::draw(const glm::mat4& cameraMat)
    {
    }
}
