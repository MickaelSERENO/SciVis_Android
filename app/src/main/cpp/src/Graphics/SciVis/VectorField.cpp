#include "Graphics/SciVis/VectorField.h"
#include "Graphics/SciVis/SciVisColor.h"

namespace sereno
{
    VectorField::VectorField(GLRenderer* renderer, Material* mtl, GameObject* parent, 
                             const std::shared_ptr<BinaryDataset> dataset, const MeshLoader* arrowLoader, GLuint tfTexture, uint8_t tfTextureDim) : 
        SciVis(parent, renderer, mtl, dataset->getSubDataset(0), tfTexture, tfTextureDim), m_binaryDataset(dataset)
    {
        //Field variables
        const float*    vel      = dataset->getVelocity();
        const uint32_t* gridSize = dataset->getGridSize();
        float           minAmp   = m_model->getMinAmplitude();
        float           maxAmp   = m_model->getMaxAmplitude();

        //Determine the displayable size
        //The displayable size is useful since we cannot represent every value in the screen
        //Because of occlusion and performance issue
        uint32_t maxVector = 0;
        for(uint32_t i = 0; i < 3; i++)
            if(gridSize[i] >= maxVector)
                maxVector = gridSize[i];
        uint32_t maxSize = 0;
        m_dataStep = (maxVector + MAX_VECTOR_ALONG_AXIS-1)/MAX_VECTOR_ALONG_AXIS;
        for(uint32_t i = 0; i < 3; i++)
        {
            m_displayableSize[i] = MAX_VECTOR_ALONG_AXIS*gridSize[i]/maxVector;
            maxSize = (maxSize > m_displayableSize[i]) ? maxSize : m_displayableSize[i];
        }
            
        //Update our matrix
        setScale(glm::vec3(2.0/maxSize));
        //setPosition(glm::vec3(-1.0, -1.0, 0.0));

        //Field parameters + buffers
        uint32_t fieldSize = m_displayableSize[0]*m_displayableSize[1]*m_displayableSize[2];
        m_nbPoints = 3*arrowLoader->nbVertices*fieldSize;
        m_nbVerticesPerArrow = arrowLoader->nbVertices;

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
                    uint32_t velPos = m_dataStep*(i + j*gridSize[0] + k*gridSize[1]*gridSize[0]);

                    //Compute transformation matrix
                    glm::mat4 transMat(1.0f);
                    transMat = glm::translate(transMat, glm::vec3(i, j, k) -
                                                        glm::vec3(m_displayableSize[0]/2.0, m_displayableSize[1]/2.0, m_displayableSize[2]/2.0));
                    transMat = transMat * dataset->getRotationQuaternion(i*m_dataStep, j*m_dataStep, k*m_dataStep).getMatrix();

                    float    amp    = 0.0;

                    for(uint32_t l = 0; l < 3; l++)
                        amp += vel[3*velPos+l]*vel[3*velPos+l];
                    amp = sqrt(amp);

                    float s = (amp-minAmp)/(maxAmp-minAmp);
                    transMat = glm::scale(transMat, glm::vec3(s, s, s));

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
        glGenVertexArrays(1, &m_vaoID);
        glBindVertexArray(m_vaoID);
        {
            //Init the VBO and EBO
            glGenBuffers(1, &m_vboID);
            glBindBuffer(GL_ARRAY_BUFFER,         m_vboID);
            {
                glBufferData(GL_ARRAY_BUFFER, sizeof(float)*(3+3+4)*currentVert, NULL, GL_STATIC_DRAW); //3 points, 3 normals, 4 colors
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*currentVert*3, fieldVertices);                          //Points
                glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*currentVert*3, sizeof(float)*currentVert*3, fieldNormals); //Normals

                //Set vertex attrib
                glVertexAttribPointer(MATERIAL_VPOSITION, 3, GL_FLOAT, 0, 0, (void*)(0));
                glVertexAttribPointer(MATERIAL_VNORMAL,   3, GL_FLOAT, 0, 0, (void*)(sizeof(float)*currentVert*3));
                glVertexAttribPointer(MATERIAL_VCOLOR,    4, GL_FLOAT, 0, 0, (void*)(sizeof(float)*currentVert*6));

                //Enable
                glEnableVertexAttribArray(MATERIAL_VPOSITION);
                glEnableVertexAttribArray(MATERIAL_VNORMAL);
                glEnableVertexAttribArray(MATERIAL_VCOLOR);
            }
        }
        glBindVertexArray(0);

        setColorRange(m_model->getMinClamping(), m_model->getMaxClamping(), m_model->getColorMode());

        free(fieldVertices);
        free(fieldNormals);
    }

    VectorField::~VectorField()
    {
        glDeleteBuffers(1, &m_vboID);
        glDeleteVertexArrays(1, &m_vaoID);
    }

    void VectorField::draw(const glm::mat4& cameraMat)
    {
        glm::mat4 mat    = getMatrix();
        glm::mat4 mvp    = cameraMat*mat;
        glm::mat4 invMVP = glm::inverse(mvp);
        m_mtl->bindMaterial(mat, cameraMat, mvp, invMVP);
        glBindVertexArray(m_vaoID);
        {
            glDrawArrays(GL_TRIANGLES, 0, m_nbPoints);
        }
        glBindVertexArray(0);
    }

    void VectorField::setColorRange(float min, float max, ColorMode colorMode)
    {
        uint32_t     size   = m_displayableSize[0]*m_displayableSize[1]*m_displayableSize[2]*m_nbVerticesPerArrow;
        float*       color  = (float*)malloc(4*sizeof(float)*size);

        //Store fluid dataset constants
        const float*    vel      = m_binaryDataset->getVelocity();
        const uint32_t* gridSize = m_binaryDataset->getGridSize();
        float           minAmp   = m_binaryDataset->getSubDataset(0)->getMinAmplitude();
        float           maxAmp   = m_binaryDataset->getSubDataset(0)->getMaxAmplitude();

        //Set the color for every vector
        for(uint32_t k = 0; k < m_displayableSize[2]; k++)
        {
            for(uint32_t j = 0; j < m_displayableSize[1]; j++)
            {
                for(uint32_t i = 0; i < m_displayableSize[0]; i++)
                {
                    //Determine the amplitude of this value
                    uint32_t colPos = i+j*m_displayableSize[0]+k*m_displayableSize[0]*m_displayableSize[1];
                    uint32_t velPos = m_dataStep*(i + j*gridSize[0] + k*gridSize[1]*gridSize[0]);
                    float    amp    = 0.0;

                    for(uint32_t l = 0; l < 3; l++)
                        amp += vel[3*velPos+l]*vel[3*velPos+l];
                    amp = sqrt(amp);

                    float t = (amp-minAmp)/(maxAmp-minAmp);
                    
                    //Clamp
                    if(t < min || t > max)
                    {
                        for(uint32_t v = 0; v < m_nbVerticesPerArrow; v++)
                            for(uint32_t l = 0; l < 4; l++)
                                color[m_nbVerticesPerArrow*4*colPos + 4*v + l] = 0.0;
                    }

                    //If inside the range, determine the color based on the color mode and on t (ratio)
                    else
                    {
                        Color c = SciVis_computeColor(colorMode, t);

                        //Store the color
                        for(uint32_t v = 0; v < m_nbVerticesPerArrow; v++)
                        {
                            color[4*m_nbVerticesPerArrow*colPos + v*4 + 0] = c.r;
                            color[4*m_nbVerticesPerArrow*colPos + v*4 + 1] = c.g;
                            color[4*m_nbVerticesPerArrow*colPos + v*4 + 2] = c.b;
                            color[4*m_nbVerticesPerArrow*colPos + v*4 + 3] = 1.0f;
                        }
                    }
                }
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_vboID),
            glBufferSubData(GL_ARRAY_BUFFER, 6*sizeof(float)*size, 4*sizeof(float)*size, color);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        free(color);
    }
}
