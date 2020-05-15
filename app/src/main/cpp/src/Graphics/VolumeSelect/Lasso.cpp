#include "Graphics/VolumeSelect/Lasso.h"

namespace sereno
{
    Lasso::Lasso(GameObject* parent, GLRenderer* renderer, Material* mtl) : GameObject(parent, renderer, mtl)
    {
        
        //Create VAO, VBO
        glGenVertexArrays(1, &m_vaoID);
        glBindVertexArray(m_vaoID);
        {
            //Init the VBO
            glGenBuffers(1, &m_vboID);
            glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
            {


                // Vertex buffer
                float defaultData[] = {};

                glBufferData(GL_ARRAY_BUFFER, sizeof(float)*24*3, defaultData, GL_STATIC_DRAW);

                //Set vertex attrib
                glVertexAttribPointer(MATERIAL_VPOSITION,   3, GL_FLOAT, 0, 3*sizeof(float), (void*)(0));

                //Enable
                glEnableVertexAttribArray(MATERIAL_VPOSITION);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        glBindVertexArray(0);

        m_nbVertices = 0;
        m_mode       = GL_LINES;
    }

    void Lasso::clearLasso()
    {
        m_nbVertices = 0;
        data.clear();

    }

    void Lasso::startLasso(float x, float y, float z)
    {
        clearLasso();
        data.push_back(x);
        data.push_back(y);
        data.push_back(z);
    }

    void Lasso::continueLasso(float x, float y, float z)
    {
        if(x - data.at(m_nbVertices*3) > 0.01
        || x - data.at(m_nbVertices*3) < -0.01
        || y - data.at(m_nbVertices*3+1) > 0.01
        || y - data.at(m_nbVertices*3+1) < -0.01)
        {
            data.push_back(x);
            data.push_back(y);
            data.push_back(z);

            data.push_back(x);
            data.push_back(y);
            data.push_back(z);

            m_nbVertices += 2;

            //Create VAO, VBO
            glGenVertexArrays(1, &m_vaoID);
            glBindVertexArray(m_vaoID);
            {
                //Init the VBO
                glGenBuffers(1, &m_vboID);
                glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
                {
                    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*m_nbVertices*3, data.data(), GL_STATIC_DRAW);

                    //Set vertex attrib
                    glVertexAttribPointer(MATERIAL_VPOSITION,   3, GL_FLOAT, 0, 3*sizeof(float), (void*)(0));

                    //Enable
                    glEnableVertexAttribArray(MATERIAL_VPOSITION);
                }
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
            glBindVertexArray(0);
        }
    }

    bool Lasso::endLasso()
    {
        if(data.at(0) - data.at(m_nbVertices*3) < 0.1
        && data.at(0) - data.at(m_nbVertices*3) > -0.1
        && data.at(1) - data.at(m_nbVertices*3+1) < 0.1
        && data.at(1) - data.at(m_nbVertices*3+1) > -0.1)
        {
            data.push_back(data.at(0));
            data.push_back(data.at(1));
            data.push_back(data.at(2));

            m_nbVertices += 2;

            //Create VAO, VBO
            glGenVertexArrays(1, &m_vaoID);
            glBindVertexArray(m_vaoID);
            {
                //Init the VBO
                glGenBuffers(1, &m_vboID);
                glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
                {
                    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*m_nbVertices*3, data.data(), GL_STATIC_DRAW);

                    //Set vertex attrib
                    glVertexAttribPointer(MATERIAL_VPOSITION,   3, GL_FLOAT, 0, 3*sizeof(float), (void*)(0));

                    //Enable
                    glEnableVertexAttribArray(MATERIAL_VPOSITION);
                }
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
            glBindVertexArray(0);
            return true;
        }
        return false;
    }

    void Lasso::draw(const Render& render)
    {
        const glm::mat4& cameraMat = render.getCameraMatrix();
        const glm::mat4& projMat   = render.getProjectionMatrix();
        glm::mat4 mat = getMatrix();
        glm::mat4 mvp = projMat*cameraMat*mat;

        glm::mat4 invMVP = glm::inverse(mvp);
        m_mtl->bindMaterial(mat, cameraMat, projMat, mvp, invMVP, render.getCameraParams());

        glBindVertexArray(m_vaoID);
        {
            glDrawArrays(m_mode, 0, m_nbVertices);
        }
        glBindVertexArray(0);
    }

    std::vector<float> Lasso::getData()
    {
        std::vector<float> r;
        for(int i = 0; i < m_nbVertices; i += 2){
            r.push_back(data.at(i*3));
            r.push_back(data.at(i*3+1));
            r.push_back(data.at(i*3+2));
        }
        return r;
    }
}