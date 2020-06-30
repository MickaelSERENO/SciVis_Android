#include "Graphics/VolumeSelect/Lasso.h"

namespace sereno
{
    Lasso::Lasso(GameObject* parent, GLRenderer* renderer, Material* mtl) : GameObject(parent, renderer, mtl)
    {
        m_vboInitialized = false;

        m_nbVertices = 0;
        m_mode       = GL_LINE_STRIP;
    }

    void Lasso::clearLasso()
    {
        m_nbVertices = 0;
        m_data.clear();

    }

    void Lasso::startLasso(float x, float y, float z)
    {
        clearLasso();
        m_data.push_back(x);
        m_data.push_back(y);
        m_data.push_back(z);

        m_nbVertices = 1;
    }

    void Lasso::continueLasso(float x, float y, float z)
    {
        if(x - m_data.at(m_nbVertices*3-3) > 0.01
        || x - m_data.at(m_nbVertices*3-3) < -0.01
        || y - m_data.at(m_nbVertices*3-2) > 0.01
        || y - m_data.at(m_nbVertices*3-2) < -0.01)
        {
            m_data.push_back(x);
            m_data.push_back(y);
            m_data.push_back(z);

            m_nbVertices += 1;
            
            buildVBO();
        }
    }

    bool Lasso::endLasso()
    {
        if(m_data.at(0) - m_data.at(m_nbVertices*3-3) < 0.1
        && m_data.at(0) - m_data.at(m_nbVertices*3-3) > -0.1
        && m_data.at(1) - m_data.at(m_nbVertices*3-2) < 0.1
        && m_data.at(1) - m_data.at(m_nbVertices*3-2) > -0.1)
        {
            m_data.push_back(m_data.at(0));
            m_data.push_back(m_data.at(1));
            m_data.push_back(m_data.at(2));

            m_nbVertices += 1;
            
            buildVBO();
            return true;
        }
        return false;
    }

    void Lasso::buildVBO()
    {
        if(m_vboInitialized){
            glDeleteBuffers(1, &m_vboID);
            glDeleteVertexArrays(1, &m_vaoID);
        }
        //Create VAO, VBO
        glGenVertexArrays(1, &m_vaoID);
        glBindVertexArray(m_vaoID);
        {
            //Init the VBO
            glGenBuffers(1, &m_vboID);
            glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
            {
                glBufferData(GL_ARRAY_BUFFER, sizeof(float)*m_nbVertices*3, m_data.data(), GL_STATIC_DRAW);
                //Set vertex attrib
                glVertexAttribPointer(MATERIAL_VPOSITION,   3, GL_FLOAT, 0, 3*sizeof(float), (void*)(0));
                //Enable
                glEnableVertexAttribArray(MATERIAL_VPOSITION);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        glBindVertexArray(0);

        m_vboInitialized = true;
    }

    void Lasso::draw(const Render& render)
    {
        if(m_enableCamera)
        {
            const glm::mat4& cameraMat = render.getCameraMatrix();
            const glm::mat4& projMat   = render.getProjectionMatrix();
            glm::mat4 mat = getMatrix();
            glm::mat4 mvp = projMat*cameraMat*mat;

            glm::mat4 invMVP = glm::inverse(mvp);
            m_mtl->bindMaterial(mat, cameraMat, projMat, mvp, invMVP, render.getCameraParams());
        }
        else
        {
            glm::mat4 mat = getMatrix();
            glm::mat4 invMVP = glm::inverse(mat);
            m_mtl->bindMaterial(mat, glm::mat4(1.0f), glm::mat4(1.0f), mat, invMVP, glm::vec4(0.0, 0.0, 0.0, 1.0));
        }

        glBindVertexArray(m_vaoID);
        {
            glDrawArrays(m_mode, 0, m_nbVertices);
        }
        glBindVertexArray(0);
    }

    const std::vector<float>& Lasso::getData() const
    {
        return m_data;
    }
}