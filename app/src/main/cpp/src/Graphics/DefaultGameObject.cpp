#include "Graphics/DefaultGameObject.h"

namespace sereno
{
    TextureRectangleData::TextureRectangleData() : GPUData()
    {
        //Create VAO, VBO
        glGenVertexArrays(1, &m_vaoID);
        glBindVertexArray(m_vaoID);
        {
            //Init the VBO
            glGenBuffers(1, &m_vboID);
            glBindBuffer(GL_ARRAY_BUFFER,         m_vboID);
            {
                //The data to send to the GPU
                float data[6*(2+3)] = { 0.5, -0.5, 0.0,  1.0, 0.0,
                                        0.5,  0.5, 0.0,  1.0, 1.0,
                                       -0.5, -0.5, 0.0,  0.0, 0.0,

                                        0.5,  0.5, 0.0,  1.0, 1.0,
                                       -0.5,  0.5, 0.0,  0.0, 1.0,
                                       -0.5, -0.5, 0.0,  0.0, 0.0};

                glBufferData(GL_ARRAY_BUFFER, sizeof(float)*6*(2+3), data, GL_STATIC_DRAW);

                //Set vertex attrib
                glVertexAttribPointer(MATERIAL_VPOSITION,   3, GL_FLOAT, 0, 5*sizeof(float), (void*)(0));
                glVertexAttribPointer(MATERIAL_VUV0,        2, GL_FLOAT, 0, 5*sizeof(float), (void*)(3*sizeof(float)));

                //Enable
                glEnableVertexAttribArray(MATERIAL_VPOSITION);
                glEnableVertexAttribArray(MATERIAL_VUV0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        glBindVertexArray(0);

        m_nbVertices = 6;
        m_mode       = GL_TRIANGLES;
    }

    DefaultGameObject::DefaultGameObject(GameObject* parent, GLRenderer* renderer, Material* mtl, GPUData* data) : GameObject(parent, renderer, mtl), m_gpuData(data) 
    {}
    
    void DefaultGameObject::draw(const glm::mat4& cameraMat, const glm::mat4& projMat)
    {
        glm::mat4 mat = getMatrix();
        glm::mat4 mvp = projMat*cameraMat*mat;

        glm::mat4 invMVP = glm::inverse(mvp);
        m_mtl->bindMaterial(mat, cameraMat, projMat, mvp, invMVP);

        glBindVertexArray(m_gpuData->getVAO());
        {
            glDrawArrays(m_gpuData->getMode(), 0, m_gpuData->getNbVertices());
        }
        glBindVertexArray(0);
    }
}
