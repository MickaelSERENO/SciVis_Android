#include "Graphics/SciVis/CloudPointGameObject.h"

namespace sereno
{

    CloudPointGameObject::CloudPointGameObject(GameObject* parent, GLRenderer* renderer, Material* mtl, std::shared_ptr<CloudPointDataset> cloudPoint, SubDataset* sd) :
        SciVis(parent, renderer, mtl, sd), m_dataset(cloudPoint)
    {
        glGenVertexArrays(1, &m_vaoID);
        glBindVertexArray(m_vaoID);
            glGenBuffers(1, &m_vboID);
            glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
                //Generate the buffer
                //3 position channel (x, y, z), 4 color channel (RGBA)
                glBufferData(GL_ARRAY_BUFFER, (sizeof(uint8_t)*4+sizeof(float)*3)*cloudPoint->getNbPoints(), NULL, GL_DYNAMIC_DRAW);

                //Configurate the VAO
                //Position
                glEnableVertexAttribArray(MATERIAL_VPOSITION);
                glVertexAttribPointer(MATERIAL_VPOSITION, 3, GL_FLOAT, 0, 0, (void*)(0));
                //Color
                glEnableVertexAttribArray(MATERIAL_VCOLOR);
                glVertexAttribPointer(MATERIAL_VCOLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)(3*sizeof(float)*cloudPoint->getNbPoints()));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    CloudPointGameObject::~CloudPointGameObject()
    {
        m_updateTFLock.lock();
        glDeleteVertexArrays(1, &m_vaoID);
        glDeleteBuffers(1, &m_vboID);

        if(m_newCols != nullptr)
            free(m_newCols);
    }

    void CloudPointGameObject::draw(const Render& render)
    {
        if(m_enableCamera)
        {
            const glm::mat4& cameraMat    = render.getCameraMatrix();
            const glm::mat4& projMat      = render.getProjectionMatrix();
            const glm::vec4& cameraParams = render.getCameraParams();

            glm::mat4 mat = getMatrix();
            glm::mat4 mvp = projMat*cameraMat*mat;

            glm::mat4 invMVP = glm::inverse(mvp);
            m_mtl->bindMaterial(mat, cameraMat, projMat, mvp, invMVP, cameraParams);
        }

        else
        {
            glm::mat4 mat = getMatrix();
            glm::mat4 invMVP = glm::inverse(mat);
            m_mtl->bindMaterial(mat, glm::mat4(1.0f), glm::mat4(1.0f), mat, invMVP, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        }

        m_updateColorLock.lock();
            uint8_t* cols = m_newCols;
            m_newCols = nullptr;
        m_updateColorLock.unlock();

        //New data computed, update VBO
        if(cols)
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
                if(!m_isPositionInit)
                {
                    m_isPositionInit = true;
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*3*m_dataset->getNbPoints(), m_dataset->getPointPositions());
                }
                glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*3*m_dataset->getNbPoints(), sizeof(uint8_t)*4*m_dataset->getNbPoints(), cols);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            free(cols);
        }

        glBindVertexArray(m_vaoID);
        {
            glDrawArrays(GL_POINTS, 0, m_dataset->getNbPoints());
        }
        glBindVertexArray(0);
    }

    void CloudPointGameObject::load()
    {
        onTFChanged();
    }

    void CloudPointGameObject::onTFChanged()
    {
        //Check if values are loaded
        if(!m_model->getParent()->areValuesLoaded())
        {
            LOG_WARNING("Cannot update the visualization because the dataset has not finished to load\n");
            return;
        }

        std::thread([this]()
        {
            //Maximum two parallel call: one pending and one executing (in case the transfer function got updated)
            if(m_isWaitingTF)
            {
                LOG_INFO("Already waiting\n");
                return;
            }
            m_isWaitingTF = true;
            m_updateTFLock.lock();
            m_isWaitingTF = false;
            LOG_INFO("Computing Colors\n");

            const std::vector<PointFieldDesc>& ptFieldDescs = m_model->getParent()->getPointFieldDescs();
            uint8_t* cols = (uint8_t*)malloc(sizeof(uint8_t)*4*m_dataset->getNbPoints());

            //Apply the transfer function
            std::shared_ptr<TF> tf = getModel()->getTransferFunction();


            if(tf != NULL && tf->getDimension() - tf->hasGradient() <= ptFieldDescs.size()) //Check if the TF can be applied.
            {
                //Check for the indice enabled
                std::vector<uint32_t> indices;
                for(uint32_t h = 0; h < tf->getDimension() - tf->hasGradient(); h++)
                    if(tf->getEnabledDimensions()[h])
                        indices.push_back(ptFieldDescs[h].id);

                //Get the associated gradient
                DatasetGradient* grad = getModel()->getParent()->getOrComputeGradient(indices);

                //Use the transfer function to generate the 3D texture
                #pragma omp parallel
                {
                    float* tfInd = (float*)malloc(tf->getDimension()*sizeof(float)); //The indice of the transfer function

                    #pragma omp for
                    {
                        //For all values in the grid
                        for(uint32_t i = 0; i < m_dataset->getNbPoints(); i++)
                        {
                            //For each parameter (e.g., temperature, presure, etc.)
                            for(uint32_t j = 0; j < tf->getDimension() - tf->hasGradient(); j++)
                            {
                                if(tf->getEnabledDimensions()[j])
                                {
                                    const PointFieldDesc& val = ptFieldDescs[j];
                                    uint8_t valueFormatInt = VTKValueFormatInt(val.format);

                                    //Compute the vector magnitude
                                    float mag = 0;
                                    if(val.nbValuePerTuple > 1)
                                    {
                                        for(uint32_t k = 0; k < val.nbValuePerTuple; k++)
                                        {
                                            float readVal = readParsedVTKValue<float>((uint8_t*)(val.values.get()) + i*valueFormatInt*val.nbValuePerTuple + k*valueFormatInt, val.format);
                                            mag = readVal*readVal;
                                        }
                                        mag = sqrt(mag);
                                    }
                                    else
                                        mag = readParsedVTKValue<float>((uint8_t*)(val.values.get()) + i*valueFormatInt*val.nbValuePerTuple, val.format);

                                    if(mag > 0)
                                        LOG_INFO("OK\n");
                                    //Save it at the correct indice in the TF indice (clamped into [0,1])
                                    tfInd[j] = (mag-val.minVal)/(val.maxVal-val.minVal);
                                }
                                else
                                    tfInd[j] = 0;
                            }

                            //Do not forget the gradient (clamped)!
                            if(grad && tf->hasGradient())
                                tfInd[tf->getDimension()-1] = grad->grads.get()[i];
                            else
                                tfInd[tf->getDimension()-1] = 0;

                            //Apply the transfer function
                            uint8_t outCol[4];
                            tf->computeColor(tfInd, outCol);
                            for(uint8_t h = 0; h < 3; h++)
                                cols[4*i+h] = outCol[h];
                            cols[4*i+3] = tf->computeAlpha(tfInd);
                        }
                    }

                    free(tfInd);
                }
            }

            m_updateColorLock.lock();
                if(m_newCols)
                    free(m_newCols);
                m_newCols = cols;
            m_updateColorLock.unlock();
            m_updateTFLock.unlock();
            LOG_INFO("End Computing Colors\n");
        }).detach();
    }
}
