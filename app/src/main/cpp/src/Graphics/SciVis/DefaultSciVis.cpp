#include "Graphics/SciVis/DefaultSciVis.h"

namespace sereno
{
    DefaultSciVis::DefaultSciVis(GLRenderer* renderer, Material* mtl, GameObject* parent, SubDataset* model, GLuint tfTexture, uint8_t tfTextureDim) : SciVis(parent, renderer, mtl, model, tfTexture, tfTextureDim)
    {
        Cube c;

        //Load VAO - VBO
        glGenVertexArrays(1, &m_vaoID);
        glBindVertexArray(m_vaoID);
        {
            //Init the VBO
            glGenBuffers(1, &m_vboID);
            glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
            {
                uint32_t nbVertices = c.getNbVertices();
                m_nbPoints = nbVertices;

                glBufferData(GL_ARRAY_BUFFER, sizeof(float)*(3+3)*nbVertices, NULL, GL_STATIC_DRAW); //3 points, 3 normals
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*nbVertices*3, c.getVertices());    //Points
                glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*nbVertices*3, sizeof(float)*nbVertices*3, c.getNormals()); //Normals

                //Set vertex attrib
                glVertexAttribPointer(MATERIAL_VPOSITION, 3, GL_FLOAT, 0, 0, (void*)(0));
                glVertexAttribPointer(MATERIAL_VNORMAL,   3, GL_FLOAT, 0, 0, (void*)(sizeof(float)*nbVertices*3));

                //Enable
                glEnableVertexAttribArray(MATERIAL_VPOSITION);
                glEnableVertexAttribArray(MATERIAL_VNORMAL);
            }
        }
        glBindVertexArray(0);
    }

    DefaultSciVis::~DefaultSciVis()
    {
        glDeleteBuffers(1, &m_vboID);
        glDeleteVertexArrays(1, &m_vaoID);
    }

    void DefaultSciVis::draw(const Render& render)
    {
        const glm::mat4& cameraMat = render.getCameraMatrix();
        const glm::mat4& projMat   = render.getProjectionMatrix();
        glm::mat4 mat = getMatrix();
        glm::mat4 mvp = projMat*cameraMat*mat;
        glm::mat4 invMVP = glm::inverse(mvp);

        m_mtl->bindMaterial(mat, cameraMat, projMat, mvp, invMVP, render.getCameraParams());
        m_mtl->bindTexture(m_tfTexture, m_tfTextureDim, 0);

        glBindVertexArray(m_vaoID);
        {
            glDrawArrays(GL_TRIANGLES, 0, m_nbPoints);
        }
        glBindVertexArray(0);
    }
}
