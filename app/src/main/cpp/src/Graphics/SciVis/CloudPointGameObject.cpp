#include "Graphics/SciVis/CloudPointGameObject.h"

namespace sereno
{

    CloudPointGameObject::CloudPointGameObject(GameObject* parent, GLRenderer* renderer, Material* mtl, std::shared_ptr<CloudPointDataset> cloudPoint, SubDataset* sd) :
        SciVis(parent, renderer, mtl, sd),
        m_dataset(cloudPoint), m_model(sd)
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
                glVertexAttribPointer(MATERIAL_VPOSITION, 3, GL_FLOAT, 0, 0, (void*)(0));
                glEnableVertexAttribArray(MATERIAL_VPOSITION);
                //Color
                glVertexAttribPointer(MATERIAL_VCOLOR, 4, GL_UNSIGNED_BYTE, 0, 0, (void*)(3*sizeof(float)*cloudPoint->getNbPoints()));
                glEnableVertexAttribArray(MATERIAL_VCOLOR);
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

            if(tf != NULL && tf->getDimension() <= ptFieldDescs.size()+1) //Check if the TF can be applied. +1 == m_grads
            {
                //Use the transfer function to generate the 3D texture
                #pragma omp parallel
                {
                    float* tfInd = (float*)malloc((ptFieldDescs.size()+1)*sizeof(float)); //The indice of the transfer function

                    #pragma omp for
                    {
                        //For all values in the grid
                        for(uint32_t i = 0; i < m_dataset->getNbPoints(); i++)
                                {
                                    //For each parameter (e.g., temperature, presure, etc.)
                                    for(uint32_t j = 0; j < ptFieldDescs.size(); j++)
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

                                        //Save it at the correct indice in the TF indice (clamped into [0,1])
                                        tfInd[j] = (mag-val.minVal)/(val.maxVal-val.minVal);
                                    }

                                    //Do not forget the gradient (clamped)!
                                    if(m_model->getParent()->getGradient())
                                        tfInd[ptFieldDescs.size()] = m_model->getParent()->getGradient()[i];
                                    else
                                        tfInd[ptFieldDescs.size()] = 0;

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

            m_newCols = cols;
            m_updateTFLock.unlock();
            LOG_INFO("End Computing Colors\n");
        }).detach();
    }
}
