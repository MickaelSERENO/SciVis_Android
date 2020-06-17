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

    VTKStructuredGridPointVBO::VTKStructuredGridPointVBO(GLRenderer* renderer, std::shared_ptr<VTKParser> vtkParser, uint32_t desiredDensity) : m_vtkParser(vtkParser)
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
                                                                       VTKStructuredGridPointVBO* gridPointVBO, SubDataset* subDataset):
        SciVis(parent, renderer, mtl, subDataset), 
        m_gridPointVBO(gridPointVBO), m_maxVal(-std::numeric_limits<float>::max()), m_minVal(std::numeric_limits<float>::max())
    {
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
        glBindTexture(GL_TEXTURE_3D, 0);

        onTFChanged();

        m_glVersion = renderer->getGLESVersion();

        setScale(m_model->getScale());
        setRotate(m_model->getGlobalRotate());
        setPosition(m_model->getPosition());
    }

    VTKStructuredGridPointGameObject::~VTKStructuredGridPointGameObject()
    {
        glDeleteVertexArrays(1, &m_vaoID);
        glDeleteTextures(1, &m_texture);

        if(m_newCols)
            free(m_newCols);
    }

    void VTKStructuredGridPointGameObject::draw(const Render& render)
    {
        /*----------------------------------------------------------------------------*/
        /*--------Determine the 4 rectangle points where the cube is on screen--------*/
        /*----------------------------------------------------------------------------*/

        glm::mat4 mvp;

        m_mtl->bindTexture(m_texture, 3, 0);
        if(m_enableCamera)
        {
            const glm::mat4& cameraMat    = render.getCameraMatrix();
            const glm::mat4& projMat      = render.getProjectionMatrix();
            const glm::vec4& cameraParams = render.getCameraParams();

            glm::mat4 mat = getMatrix();
            mvp = projMat*cameraMat*mat;

            glm::mat4 invMVP = glm::inverse(mvp);
            m_mtl->bindMaterial(mat, cameraMat, projMat, mvp, invMVP, cameraParams);
        }

        else
        {
            glm::mat4 mat = getMatrix();
            mvp = mat;
            glm::mat4 invMVP = glm::inverse(mvp);
            m_mtl->bindMaterial(mat, glm::mat4(1.0f), glm::mat4(1.0f), mvp, invMVP, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        }

        glm::vec4 points[8];
        points[0] = glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
        points[1] = glm::vec4( 0.5f, -0.5f, -0.5f, 1.0f);
        points[2] = glm::vec4(-0.5f,  0.5f, -0.5f, 1.0f);
        points[3] = glm::vec4(-0.5f, -0.5f,  0.5f, 1.0f);
        points[4] = glm::vec4( 0.5f,  0.5f, -0.5f, 1.0f);
        points[5] = glm::vec4( 0.5f, -0.5f,  0.5f, 1.0f);
        points[6] = glm::vec4(-0.5f,  0.5f,  0.5f, 1.0f);
        points[7] = glm::vec4( 0.5f,  0.5f,  0.5f, 1.0f);

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
        /*-------------------------------Update Texture-------------------------------*/
        /*----------------------------------------------------------------------------*/

        uint8_t* cols = m_newCols;
        m_newCols = nullptr;
        if(cols)
        {
            //Update the 3D texture
            glBindTexture(GL_TEXTURE_3D, m_texture);
                glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA,
                             m_gridPointVBO->m_dimensions[0], m_gridPointVBO->m_dimensions[1], m_gridPointVBO->m_dimensions[2],
                             0, GL_RGBA, GL_UNSIGNED_BYTE, cols);
            glBindBuffer(GL_TEXTURE_3D, 0);

            //Free everything
            free(cols);
        }

        /*----------------------------------------------------------------------------*/
        /*------------------------------------Draw------------------------------------*/
        /*----------------------------------------------------------------------------*/

        glDisable(GL_CULL_FACE);
        glBindVertexArray(m_vaoID);
        {
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glBindVertexArray(0);
        glEnable(GL_CULL_FACE);
    }

    void VTKStructuredGridPointGameObject::load()
    {
        onTFChanged();
    }

    void VTKStructuredGridPointGameObject::onTFChanged()
    {
        //CHeck if values are loaded
        if(!m_model->getParent()->areValuesLoaded())
        {
            LOG_WARNING("Cannot update the visualization because the dataset has not finished to load\n");
            return;
        }

        std::thread([this]()
        {
            //Maximum two parallel call: one pending and one executing (in case the transfer function got updated)
            if(m_isWaiting3DImage)
            {
                LOG_INFO("Already waiting\n");
                return;
            }
            m_isWaiting3DImage = true;
            m_updateTFLock.lock();
            m_isWaiting3DImage = false;
            LOG_INFO("Computing Colors\n");

            const VTKStructuredPoints& ptsDesc = m_gridPointVBO->m_vtkParser->getStructuredPointsDescriptor();
            const std::vector<PointFieldDesc>& ptFieldDescs = m_model->getParent()->getPointFieldDescs();

            const VTKDataset* vtk = (VTKDataset*)(m_model->getParent());

            //The RGBA data variables (nb values and array of colors)
            size_t nbValues  = m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1]*m_gridPointVBO->m_dimensions[2];
            uint8_t* cols = (uint8_t*)malloc(sizeof(uint8_t)*nbValues*4);

            //Apply the transfer function
            std::shared_ptr<TF> tf = getModel()->getTransferFunction();

            if(tf != NULL && tf->getDimension() <= ptFieldDescs.size()+1) //Check if the TF can be applied. +1 == m_grads
            {
                //Use the transfer function to generate the 3D texture
                #pragma omp parallel
                {
                    float* tfInd = (float*)malloc((ptFieldDescs.size()+1)*sizeof(float)); //The indice of the transfer function

                    #pragma omp for
                    {
                        //For all values in the grid
                        for(uint32_t k = 0; k < m_gridPointVBO->m_dimensions[2]; k++)
                            for(uint32_t j = 0; j < m_gridPointVBO->m_dimensions[1]; j++)
                                for(uint32_t i = 0; i < m_gridPointVBO->m_dimensions[0]; i++)
                                {
                                    //The destination indice in the 3D texture
                                    size_t destID = i+
                                                    j*m_gridPointVBO->m_dimensions[0] +
                                                    k*m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1];

                                    //The source indice to read in the dataset raw data
                                    size_t srcID  = (i*ptsDesc.size[0]/m_gridPointVBO->m_dimensions[0]) +
                                                    ptsDesc.size[0]*(j*ptsDesc.size[1]/m_gridPointVBO->m_dimensions[1]) +
                                                    ptsDesc.size[1]*ptsDesc.size[0]*(k*ptsDesc.size[2]/m_gridPointVBO->m_dimensions[2]);

                                    if(!vtk->getMask(srcID))
                                    {
                                        for(uint8_t h = 0; h < 3; h++)
                                            cols[4*destID+h] = 0;
                                        cols[4*destID+3] = 0;
                                        continue;
                                    }

                                    //For each parameter (e.g., temperature, presure, etc.)
                                    for(uint32_t h = 0; h < ptFieldDescs.size(); h++)
                                    {
                                        const PointFieldDesc& val = ptFieldDescs[h];
                                        uint8_t valueFormatInt = VTKValueFormatInt(val.format);

                                        //Compute the vector magnitude
                                        float mag = 0;
                                        for(uint32_t l = 0; l < val.nbValuePerTuple; l++)
                                        {
                                            float readVal = readParsedVTKValue<float>((uint8_t*)(val.values.get()) + srcID*valueFormatInt*val.nbValuePerTuple + l*valueFormatInt, val.format);
                                            mag = readVal*readVal;
                                        }
                                        mag = sqrt(mag);

                                        //Save it at the correct indice in the TF indice (clamped into [0,1])
                                        tfInd[h] = (mag-val.minVal)/(val.maxVal-val.minVal);
                                    }

                                    //Do not forget the gradient (clamped)!
                                    tfInd[ptFieldDescs.size()] = m_model->getParent()->getGradient()[srcID];

                                    //Apply the transfer function
                                    uint8_t outCol[4];
                                    tf->computeColor(tfInd, outCol);
                                    for(uint8_t h = 0; h < 3; h++)
                                        cols[4*destID+h] = outCol[h];
                                    cols[4*destID+3] = tf->computeAlpha(tfInd);
                                }
                    }

                    free(tfInd);
                }
            }

            m_newCols = cols;
            m_updateTFLock.unlock();
            LOG_INFO("End Computing Colors\n");

        }).detach();
    }

    VTKStructuredGridPointSciVis::VTKStructuredGridPointSciVis(GLRenderer* renderer, Material* material, std::shared_ptr<VTKDataset> d, uint32_t desiredDensity) : dataset(d)
    {
        vbo = new VTKStructuredGridPointVBO(renderer, d->getParser(), desiredDensity);
    }

    VTKStructuredGridPointSciVis::~VTKStructuredGridPointSciVis()
    {
        for(auto go : gameObjects)
            delete go;
        delete vbo;
    }
}
