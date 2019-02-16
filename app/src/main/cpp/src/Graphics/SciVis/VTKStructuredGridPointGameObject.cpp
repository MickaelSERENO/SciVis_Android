#include "Graphics/SciVis/VTKStructuredGridPointGameObject.h"
#include "Graphics/SciVis/SciVisColor.h"
#include <algorithm>
#include <cstdlib>
#include <limits>

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

        //Store the points
        //We will use a geometry shader for creating the cubes, permitting to have a better density (less memory taken)
        //Also the size is normalized (i.e the maximum length is 1) and centered
        size_t nbValues  = m_dimensions[0]*m_dimensions[1]*m_dimensions[2];

        float  maxAxis   = std::max(ptsDesc.spacing[0]*ptsDesc.size[0],
                                    std::max(ptsDesc.spacing[1]*ptsDesc.size[1],
                                             ptsDesc.spacing[2]*ptsDesc.size[2]));

        for(uint32_t i = 0; i < 3; i++)
            m_spacing[i] = ptsDesc.size[i]*ptsDesc.spacing[i]/m_dimensions[i]/maxAxis;

        float* pts = (float*)malloc(sizeof(float)*nbValues*3);

        for(uint32_t k = 0; k < m_dimensions[2]; k++)
            for(uint32_t j = 0; j < m_dimensions[1]; j++)
                for(uint32_t i = 0; i < m_dimensions[0]; i++)
                {
                    size_t id = 3*(i + j*m_dimensions[0] + k*m_dimensions[0]*m_dimensions[1]);
                    float pos[3] = {(float)i, (float)j, (float)k};
                    for(uint32_t l = 0; l < 3; l++)
                        pts[id + l] = (pos[l]-m_dimensions[l]/2.0)*m_spacing[l];
                }

        //Creates the VBO
        glGenBuffers(1, &m_vboID);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*nbValues*(3+4*nbPtFields), NULL, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*nbValues*3, pts);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //Free the allocated memory
        free(pts);
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
        setColorRange(m_model->getMinClamping(), m_model->getMaxClamping(), m_model->getColorMode());

        //Set VAO
        glGenVertexArrays(1, &m_vaoID);
        glBindVertexArray(m_vaoID);
            glBindBuffer(GL_ARRAY_BUFFER, m_gridPointVBO->m_vboID);

            glVertexAttribPointer(MATERIAL_VPOSITION, 3, GL_FLOAT, 0, 0, (void*)(0));
            glVertexAttribPointer(MATERIAL_VCOLOR, 4, GL_FLOAT, 0, 0, (void*)((4*propID+3)*nbValues*sizeof(float)));

            glEnableVertexAttribArray(MATERIAL_VPOSITION);
            glEnableVertexAttribArray(MATERIAL_VCOLOR);
        glBindVertexArray(0);
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
        glm::mat4 invMVP = glm::inverse(mvp);
        m_mtl->bindMaterial(mat, cameraMat, mvp, invMVP);
        glBindVertexArray(m_vaoID);
        {
            size_t nbValues  = m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1]*m_gridPointVBO->m_dimensions[2];
            setScale(glm::vec3(0.5, 0.5, 0.5));
            glDrawArrays(GL_POINTS, 0, nbValues);
        }
        glBindVertexArray(0);
    }

    void VTKStructuredGridPointGameObject::setColorRange(float min, float max, ColorMode colorMode)
    {
        size_t nbValues  = m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1]*m_gridPointVBO->m_dimensions[2];

        //Update the color. The values are already stored in m_vals.
        float* colors   = (float*)malloc(sizeof(float)*nbValues*4);
        for(uint32_t i = 0; i < nbValues; i++)
        {
            float t = (m_vals[i]-m_minVal)/(m_maxVal-m_minVal);

            //Test if inside the min-max range.
            if(t > max || t < min)
                for(uint32_t j = 0; j < 4; j++)
                    colors[i*4+j] = 1.0f;
            else
            {
                Color c = SciVis_computeColor(colorMode, t);
                colors[i*4+0] = c.r;
                colors[i*4+1] = c.g;
                colors[i*4+2] = c.b;
                colors[i*4+3] = c.a;
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_gridPointVBO->m_vboID);
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*nbValues*(3+4*m_propID), sizeof(float)*nbValues*4, colors);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        free(colors);
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
