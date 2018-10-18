#include "Graphics/VectorField.h"

namespace sereno
{
    VectorField::VectorField(GLRenderer* renderer, Material* mtl, GameObject* parent, 
                             FluidDataset* dataset, MeshLoader* arrowLoader) : GameObject(parent, renderer, mtl)
    {
        //Field parameters + buffers
        uint32_t fieldSize = dataset->nbCells();
        m_nbTriangles = arrowLoader->nbSurfaces*fieldSize;

        float*    fieldVertices  = (float*)   malloc(sizeof(float)   *3*arrowLoader->nbVertices *fieldSize);
        float*    fieldNormals   = (float*)   malloc(sizeof(float)   *2*arrowLoader->nbVertices *fieldSize);
        uint32_t* fieldTriangles = (uint32_t*)malloc(sizeof(uint32_t)*3*m_nbTriangles);

        uint32_t currentFace = 0;
        uint32_t currentVert = 0;

        //For each cell
        for(uint32_t k = 0; k < dataset->getGridSize()[2]; k++)
        {
            for(uint32_t j = 0; j < dataset->getGridSize()[1]; j++)
            {
                for(uint32_t i = 0; i < dataset->getGridSize()[0]; i++)
                {
                    //Compute transformation matrix
                    glm::mat4 transMat(1.0f);
                    transMat = glm::translate(transMat, glm::vec3(i, j, k));
                    transMat = transMat * dataset->getRotationQuaternion(i, j, k).getMatrix();
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

                    //Save the triangles indices
                    for(uint32_t faceID = 0; faceID < arrowLoader->nbSurfaces; faceID++)
                        for(uint32_t v = 0; v < 3; v++)
                            fieldTriangles[3*(faceID+currentFace)+v] = arrowLoader->surfaces[3*faceID+v];

                    //Advance further
                    currentVert+=arrowLoader->nbVertices;
                    currentFace+=arrowLoader->nbSurfaces;
                }
            }
        }        

        //Load VAO - VBO - EBO
        glGenVertexArraysOES(1, &m_vaoID);
        glBindVertexArrayOES(m_vaoID);
        {
            //Init the VBO and EBO
            glGenBuffers(1, &m_vboID);
            glGenBuffers(1, &m_eboID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboID);
            glBindBuffer(GL_ARRAY_BUFFER,         m_vboID);
            {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nbTriangles*sizeof(uint32_t)*3, fieldTriangles, GL_STATIC_DRAW);
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
        free(fieldTriangles);
    }

    VectorField::~VectorField()
    {
        glDeleteBuffers(1, &m_eboID);
        glDeleteBuffers(1, &m_vboID);
        glDeleteVertexArraysOES(1, &m_vaoID);
    }

    void VectorField::onDraw(const glm::mat4& cameraMat)
    {
        glm::mat4 mat    = getMatrix();
        glm::mat4 mvp    = cameraMat*mat;
        glm::mat4 invMVP = glm::inverse(mvp);
        m_mtl->bindMaterial(mat, cameraMat, mvp, invMVP);
        glBindVertexArrayOES(m_vaoID);
        {
            glDrawArrays(GL_TRIANGLES, 0, m_nbTriangles);
        }
        glBindVertexArrayOES(0);
    }
}
