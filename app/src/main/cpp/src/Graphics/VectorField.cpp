#include "Graphics/VectorField.h"

namespace sereno
{
    VectorField::VectorField(GLRenderer* renderer, Material* mtl, GameObject* parent, 
                             FluidDataset* dataset, MeshLoader* arrowLoader) : GameObject(parent, renderer, mtl)
    {
        //Determine the displayable size
        //The displayable size is useful since we cannot represent every value in the screen
        //Because of occlusion and performance issue
        uint32_t maxVector = 0;
        for(uint32_t i = 0; i < 3; i++)
            if(dataset->getGridSize()[i] >= maxVector)
                maxVector = dataset->getGridSize()[i];
        uint32_t dataStep;
        uint32_t maxSize = 0;
        dataStep = (maxVector + MAX_VECTOR_ALONG_AXIS-1)/MAX_VECTOR_ALONG_AXIS;
        for(uint32_t i = 0; i < 3; i++)
        {
            m_displayableSize[i] = MAX_VECTOR_ALONG_AXIS*dataset->getGridSize()[i]/maxVector;
            maxSize = (maxSize > m_displayableSize[i]) ? maxSize : m_displayableSize[i];
        }
            
        //Update our matrix
        setScale(glm::vec3(2.0/maxSize));
        //setPosition(glm::vec3(-1.0, -1.0, 0.0));

        //Field parameters + buffers
        uint32_t fieldSize = m_displayableSize[0]*m_displayableSize[1]*m_displayableSize[2];
        m_nbPoints = 3*arrowLoader->nbVertices*fieldSize;

        float* fieldVertices  = (float*)malloc(sizeof(float)*3*arrowLoader->nbVertices*fieldSize);
        float* fieldNormals   = (float*)malloc(sizeof(float)*3*arrowLoader->nbVertices*fieldSize);

        uint32_t currentVert = 0;

        //For each cell
        for(uint32_t k = 0; k < m_displayableSize[2]; k++)
        {
            for(uint32_t j = 0; j < m_displayableSize[1]; j++)
            {
                for(uint32_t i = 0; i < m_displayableSize[0]; i++)
                {
                    //Compute transformation matrix
                    glm::mat4 transMat(1.0f);
                    transMat = glm::translate(transMat, glm::vec3(i, j, k) -
                                                        glm::vec3(m_displayableSize[0]/2.0, m_displayableSize[1]/2.0, m_displayableSize[2]/2.0));
                    transMat = transMat * dataset->getRotationQuaternion(i*dataStep, j*dataStep, k*dataStep).getMatrix();
                    transMat = glm::scale(transMat, glm::vec3(1.0f, 1.0f, 1.0f));

                    glm::mat4 tInvTransMat = glm::transpose(glm::inverse(transMat));

                    //Apply this transformation to each vertices
                    for(uint32_t vertID = 0; vertID < arrowLoader->nbVertices; vertID++)
                    {
                        //The destination vertex
                        glm::vec3 vert = transMat * glm::vec4(arrowLoader->vertices[3*vertID],
                                                              arrowLoader->vertices[3*vertID+1],
                                                              arrowLoader->vertices[3*vertID+2],
                                                              1.0f);

                        glm::vec3 norm = tInvTransMat * glm::vec4(arrowLoader->normals[3*vertID],
                                                                  arrowLoader->normals[3*vertID+1],
                                                                  arrowLoader->normals[3*vertID+2],
                                                                  1.0f);

                        for(uint32_t v = 0; v < 3; v++)
                        {
                            fieldVertices[3*(vertID + currentVert) + v] = vert[v];
                            fieldNormals [3*(vertID + currentVert) + v] = norm[v];
                        }
                    }

                    //Advance further
                    currentVert+=arrowLoader->nbVertices;
                }
            }
        }        

        //Load VAO - VBO - EBO
        glGenVertexArraysOES(1, &m_vaoID);
        glBindVertexArrayOES(m_vaoID);
        {
            //Init the VBO and EBO
            glGenBuffers(1, &m_vboID);
            glBindBuffer(GL_ARRAY_BUFFER,         m_vboID);
            {
                glBufferData(GL_ARRAY_BUFFER, sizeof(float)*(3+3)*currentVert, NULL, GL_STATIC_DRAW);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*currentVert*3, fieldVertices);                            //Points
                glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*currentVert*3, sizeof(float)*currentVert*3, fieldNormals); //Normals

                //Set vertex attrib
                glVertexAttribPointer(MATERIAL_VPOSITION, 3, GL_FLOAT, 0, 0, (void*)(0));
                glVertexAttribPointer(MATERIAL_VNORMAL,   3, GL_FLOAT, 0, 0, (void*)(sizeof(float)*currentVert*3));

                //Enable
                glEnableVertexAttribArray(MATERIAL_VPOSITION);
                glEnableVertexAttribArray(MATERIAL_VNORMAL);
            }
        }

        free(fieldVertices);
        free(fieldNormals);
    }

    VectorField::~VectorField()
    {
        glDeleteBuffers(1, &m_vboID);
        glDeleteVertexArraysOES(1, &m_vaoID);
    }

    void VectorField::draw(const glm::mat4& cameraMat)
    {
        glm::mat4 mat    = getMatrix();
        glm::mat4 mvp    = cameraMat*mat;
        glm::mat4 invMVP = glm::inverse(mvp);
        m_mtl->bindMaterial(mat, cameraMat, mvp, invMVP);
        glBindVertexArrayOES(m_vaoID);
        {
            glDrawArrays(GL_TRIANGLES, 0, m_nbPoints);
        }
        glBindVertexArrayOES(0);
    }
}
