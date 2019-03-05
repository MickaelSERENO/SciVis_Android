#include "Graphics/SciVis/VTKStructuredGridPointGameObject.h"
#include "SciVisColor.h"
#include "Graphics/SciVis/VolumeRenderingPlaneAlgorithm.h"
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <omp.h>

namespace sereno
{

    /**
     * \brief  Determine the dimensions of a VTKStructuredGrid with a desired density
     *
     * \param ptsDesc the point descripor of the structuredgrid
     * \param desiredDensity the desired density
     * \param [out] outDimensions the output dimensions
     */
    static void VTKStructuredPoint_getDimensions(const VTKStructuredPoints& ptsDesc, uint32_t desiredDensity, uint32_t* outDimensions)
    {
        if(ptsDesc.size[0] == 0 || ptsDesc.size[1] == 0 || ptsDesc.size[2] == 0)
            for(uint32_t i = 0; i < 3; i++)
                outDimensions[i] = 0;
        else
        {
            uint32_t x = (ptsDesc.size[0] + desiredDensity - 1) / desiredDensity;
            uint32_t y = (ptsDesc.size[1] + desiredDensity - 1) / desiredDensity;
            uint32_t z = (ptsDesc.size[2] + desiredDensity - 1) / desiredDensity;

            uint32_t maxRatio = std::max(std::max(x, y), z);
            for(uint32_t i = 0; i < 3; i++)
                outDimensions[i] = ptsDesc.size[i]/maxRatio;
        }
    }

    VTKStructuredGridPointVBO::VTKStructuredGridPointVBO(GLRenderer* renderer, std::shared_ptr<VTKParser> vtkParser, uint32_t nbPtFields, uint32_t desiredDensity) : m_vtkParser(vtkParser)
    {
        //Determine dimensions
        const VTKStructuredPoints& ptsDesc = m_vtkParser->getStructuredPointsDescriptor();
        VTKStructuredPoint_getDimensions(ptsDesc, desiredDensity, m_dimensions);

        //Determine the new spacing
        float  maxAxis   = std::max(ptsDesc.spacing[0]*ptsDesc.size[0],
                                    std::max(ptsDesc.spacing[1]*ptsDesc.size[1],
                                             ptsDesc.spacing[2]*ptsDesc.size[2]));
        for(uint32_t i = 0; i < 3; i++)
            m_spacing[i] = ptsDesc.size[i]*ptsDesc.spacing[i]/m_dimensions[i]/maxAxis;

        //We use a ray marching algorithm, so we only need the window size
        glGenBuffers(1, &m_vboID);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*6*2, NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    VTKStructuredGridPointVBO::~VTKStructuredGridPointVBO()
    {
        glDeleteBuffers(1, &m_vboID);
    }

    VTKStructuredGridPointGameObject::VTKStructuredGridPointGameObject(GameObject* parent, GLRenderer* renderer, Material* mtl, 
                                                                       VTKStructuredGridPointVBO* gridPointVBO, uint32_t propID, 
                                                                       const VTKFieldValue* ptFieldValue, SubDataset* subDataset, 
                                                                       GLuint tfTexture, uint8_t tfTextureDim) : 
        SciVis(parent, renderer, mtl, subDataset, tfTexture, tfTextureDim), 
        m_gridPointVBO(gridPointVBO), m_maxVal(-std::numeric_limits<float>::max()), m_minVal(std::numeric_limits<float>::max()), 
        m_propID(propID)
    {
        const VTKStructuredPoints& ptsDesc = m_gridPointVBO->m_vtkParser->getStructuredPointsDescriptor();

        //Read and determine the max / min values
        uint8_t* vals = (uint8_t*)m_gridPointVBO->m_vtkParser->parseAllFieldValues(ptFieldValue);
        for(uint32_t i = 0; i < ptFieldValue->nbTuples; i++)
        {
            float val = readParsedVTKValue<double>(vals + i*VTKValueFormatInt(ptFieldValue->format)*ptFieldValue->nbValuePerTuple,
                                                   ptFieldValue->format);
            m_maxVal = std::max(m_maxVal, val);
            m_minVal = std::min(m_minVal, val);
        }
        float amp[2] = {m_maxVal, m_minVal};
        subDataset->setAmplitude(amp);

        //Store the interesting values
        size_t nbValues  = m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1]*m_gridPointVBO->m_dimensions[2];
        m_vals = (float*)malloc(sizeof(float)*nbValues);
        for(uint32_t k = 0; k < m_gridPointVBO->m_dimensions[2]; k++)
            for(uint32_t j = 0; j < m_gridPointVBO->m_dimensions[1]; j++)
                for(uint32_t i = 0; i < m_gridPointVBO->m_dimensions[0]; i++)
                {
                    size_t destID = i + 
                                    j*m_gridPointVBO->m_dimensions[0] + 
                                    k*m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1];
                    size_t srcID  = i*ptsDesc.size[0]/m_gridPointVBO->m_dimensions[0] +
                                    j*ptsDesc.size[1]/m_gridPointVBO->m_dimensions[1]*ptsDesc.size[0] + 
                                    k*ptsDesc.size[2]/m_gridPointVBO->m_dimensions[2]*ptsDesc.size[1]*ptsDesc.size[0];
                    m_vals[destID] = readParsedVTKValue<double>(vals + srcID*VTKValueFormatInt(ptFieldValue->format)*ptFieldValue->nbValuePerTuple,
                                                                ptFieldValue->format);
                }
        free(vals);

        //Set VAO
        glGenVertexArrays(1, &m_vaoID);
        glBindVertexArray(m_vaoID);
            glBindBuffer(GL_ARRAY_BUFFER, m_gridPointVBO->m_vboID);
            glVertexAttribPointer(MATERIAL_VPOSITION, 2, GL_FLOAT, 0, 0, (void*)(0));
            glEnableVertexAttribArray(MATERIAL_VPOSITION);
        glBindVertexArray(0);

        //Create 3D texture
        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_3D, m_texture);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_OES);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_OES);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER_OES);
            setColorRange(m_model->getMinClamping(), m_model->getMaxClamping(), m_model->getColorMode());
        glBindTexture(GL_TEXTURE_3D, 0);

        setScale(glm::vec3(0.5, 0.5, 0.5));

        setRotate(m_model->getGlobalRotate());
    }

    VTKStructuredGridPointGameObject::~VTKStructuredGridPointGameObject()
    {
        glDeleteVertexArrays(1, &m_vaoID);
        free(m_vals);
    }

    void VTKStructuredGridPointGameObject::draw(const glm::mat4& cameraMat)
    {
        glm::mat4 mat    = getMatrix();
        glm::mat4 mvp    = cameraMat*mat;

        /*----------------------------------------------------------------------------*/
        /*--------Determine the 4 rectangle points where the cube is on screen--------*/
        /*----------------------------------------------------------------------------*/

        glm::vec4 points[8];
        points[0] = glm::vec4(0.f, 0.f, 0.f, 1.0f);
        points[1] = glm::vec4(1.0, 0.f, 0.f, 1.0f);
        points[2] = glm::vec4(0.f, 1.0, 0.f, 1.0f);
        points[3] = glm::vec4(0.f, 0.f, 1.0, 1.0f);
        points[4] = glm::vec4(1.0, 1.0, 0.f, 1.0f);
        points[5] = glm::vec4(1.0, 0.f, 1.0, 1.0f);
        points[6] = glm::vec4(0.f, 1.0, 1.0, 1.0f);
        points[7] = glm::vec4(1.0, 1.0, 1.0, 1.0f);

        for(uint8_t i = 0; i < 8; i++)
        {
            points[i] = mvp*points[i];
            points[i]/=points[i].w;
        }

        glm::vec2 minPos = points[0];
        glm::vec2 maxPos = minPos;

        for(uint8_t i = 1; i < 8; i++)
        {
            minPos.x = std::min(minPos.x, points[i].x);
            minPos.y = std::min(minPos.y, points[i].y);

            maxPos.x = std::max(maxPos.x, points[i].x);
            maxPos.y = std::max(maxPos.y, points[i].y);
        }

        float vboData[6*2] = {minPos.x, minPos.y,
                              maxPos.x, maxPos.y,
                              minPos.x, maxPos.y,
                              minPos.x, minPos.y,
                              maxPos.x, minPos.y,
                              maxPos.x, maxPos.y};

        /*----------------------------------------------------------------------------*/
        /*---------------------------------Update VBO---------------------------------*/
        /*----------------------------------------------------------------------------*/

        glBindBuffer(GL_ARRAY_BUFFER, m_gridPointVBO->m_vboID);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vboData), vboData);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        /*----------------------------------------------------------------------------*/
        /*------------------------------------Draw------------------------------------*/
        /*----------------------------------------------------------------------------*/

        glm::mat4 invMVP = glm::inverse(mvp);
        m_mtl->bindMaterial(mat, cameraMat, mvp, invMVP);
        m_mtl->bindTexture(m_texture,   3, 0);
        m_mtl->bindTexture(m_tfTexture, 2, 1);

        glDepthMask(false);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindVertexArray(m_vaoID);
        {
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glBindVertexArray(0);
        glDepthMask(true);
        glDisable(GL_BLEND);
    }

    void VTKStructuredGridPointGameObject::setColorRange(float min, float max, ColorMode colorMode)
    {
        size_t nbValues  = m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1]*m_gridPointVBO->m_dimensions[2];

        //Update the values of the 3D texture
        //Store scalar and gradient magnitude values
        float* vals = (float*)malloc(sizeof(float)*nbValues*2);

        //Compute scalar
        float maxGrad = 0.0f;
        #pragma omp parallel
        {
            #pragma omp for
            {
                for(uint32_t i = 0; i < nbValues; i++)
                {
                    float t = (m_vals[i]-m_minVal)/(m_maxVal-m_minVal);

                    //Test if inside the min-max range.
                    if(t > max || t < min)
                        vals[2*i] = -1.0f;
                    else
                        vals[2*i] = t;
                }
            }

        /*----------------------------------------------------------------------------*/
        /*--------------------------Compute gradient values---------------------------*/
        /*----------------------------------------------------------------------------*/

            //Central difference used
            #pragma omp for reduction(max:maxGrad)
            {
                for(uint32_t k = 1; k < m_gridPointVBO->m_dimensions[2]-1; k++)
                    for(uint32_t j = 1; j < m_gridPointVBO->m_dimensions[1]-1; j++)
                        for(uint32_t i = 1; i < m_gridPointVBO->m_dimensions[0]-1; i++)
                        {
                            uint32_t ind = i + j*m_gridPointVBO->m_dimensions[0] + 
                                           k*m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1];
                            float    x1  = vals[2*(ind-1)];
                            float    x2  = vals[2*(ind+1)];
                            float    y1  = vals[2*(ind-m_gridPointVBO->m_dimensions[0])];
                            float    y2  = vals[2*(ind+m_gridPointVBO->m_dimensions[0])];
                            float    z1  = vals[2*(ind-m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1])];
                            float    z2  = vals[2*(ind+m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1])];

                            vals[2*ind+1] = (x1-x2)*(x1-x2)/m_gridPointVBO->m_spacing[0]+
                                            (y1-y2)*(y1-y2)/m_gridPointVBO->m_spacing[1]+
                                            (z1-z2)*(z1-z2)/m_gridPointVBO->m_spacing[2];
                            maxGrad = std::max(vals[2*ind+1], maxGrad);
                        }
            }


            //Normalize the gradient
            #pragma omp for
            {
                for(uint32_t k = 1; k < m_gridPointVBO->m_dimensions[2]-1; k++)
                    for(uint32_t j = 1; j < m_gridPointVBO->m_dimensions[1]-1; j++)
                        for(uint32_t i = 1; i < m_gridPointVBO->m_dimensions[0]-1; i++)
                        {
                            uint32_t ind = i + j*m_gridPointVBO->m_dimensions[0] + 
                                           k*m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1];
                            vals[2*ind+1] /= maxGrad;
                        }
            }

                /*----------------------------------------------------------------------------*/
                /*---------------Compute gradient values for Edge (grad = 0.0f)---------------*/
                /*----------------------------------------------------------------------------*/

            //for k = 0 and k = max
            #pragma omp for
            {
                for(uint32_t j = 0; j < m_gridPointVBO->m_dimensions[1]; j++)
                    for(uint32_t i = 0; i < m_gridPointVBO->m_dimensions[0]; i++)
                    {
                        uint32_t offset = (m_gridPointVBO->m_dimensions[2]-1)*m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1];
                        vals[2*(i+j*m_gridPointVBO->m_dimensions[0])+1]        = 0.0f;
                        vals[2*(i+j*m_gridPointVBO->m_dimensions[0]+offset)+1] = 0.0f;
                    }
            }

            //for j = 0 and j = max
            #pragma omp for
            {
                for(uint32_t k = 0; k < m_gridPointVBO->m_dimensions[2]; k++)
                    for(uint32_t i = 0; i < m_gridPointVBO->m_dimensions[0]; i++)
                    {
                        uint32_t offset = (m_gridPointVBO->m_dimensions[1]-1)*m_gridPointVBO->m_dimensions[0];
                        vals[2*(i+k*m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1])+1]        = 0.0f;
                        vals[2*(i+k*m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1]+offset)+1] = 0.0f;
                    }
            }

                //for i = 0 and i = max
            #pragma omp for
            {
                for(uint32_t k = 0; k < m_gridPointVBO->m_dimensions[2]; k++)
                    for(uint32_t j = 0; j < m_gridPointVBO->m_dimensions[1]; j++)
                    {
                        uint32_t offset = m_gridPointVBO->m_dimensions[0]-1;
                        vals[2*(j*m_gridPointVBO->m_dimensions[0]+
                                k*m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1])+1]        = 0.0f;
                        vals[2*(j*m_gridPointVBO->m_dimensions[0]+
                                k*m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1]+offset)+1] = 0.0f;
                    }
            }
        }

        glBindTexture(GL_TEXTURE_3D, m_texture);
            glTexImage3D(GL_TEXTURE_3D, 0, GL_RG32F,
                         m_gridPointVBO->m_dimensions[0], m_gridPointVBO->m_dimensions[1], m_gridPointVBO->m_dimensions[2], 
                         0, GL_RG, GL_FLOAT, vals);
        glBindBuffer(GL_TEXTURE_3D, 0);
        free(vals);
    }

    VTKStructuredGridPointSciVis::VTKStructuredGridPointSciVis(GLRenderer* renderer, Material* material, std::shared_ptr<VTKDataset> d, 
                                                               uint32_t desiredDensity, GLuint tfTexture, uint8_t tfTextureDim) : dataset(d)
    {
        //Create every objects
        //No parent assigned yet
        vbo         = new VTKStructuredGridPointVBO(renderer, d->getParser(), d->getPtFieldValues().size(), desiredDensity);
        gameObjects = (VTKStructuredGridPointGameObject**)malloc(sizeof(VTKStructuredGridPointGameObject*)*d->getPtFieldValues().size());
        for(uint32_t i = 0; i < d->getPtFieldValues().size(); i++)
            gameObjects[i] = new VTKStructuredGridPointGameObject(NULL, renderer, material, vbo, i, d->getPtFieldValues()[i], d->getSubDataset(i), tfTexture, tfTextureDim);
    }

    VTKStructuredGridPointSciVis::~VTKStructuredGridPointSciVis()
    {
        for(uint32_t i = 0; i < dataset->getPtFieldValues().size(); i++)
            delete gameObjects[i];
        delete gameObjects;
        delete vbo;
    }
}
