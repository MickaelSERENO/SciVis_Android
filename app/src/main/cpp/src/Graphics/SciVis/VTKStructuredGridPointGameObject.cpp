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
        float  minPos[3] = {(float)((m_dimensions[0]/2.0) * ptsDesc.size[0] / m_dimensions[0] * ptsDesc.spacing[0]),
                            (float)((m_dimensions[1]/2.0) * ptsDesc.size[1] / m_dimensions[1] * ptsDesc.spacing[1]),
                            (float)((m_dimensions[2]/2.0) * ptsDesc.size[2] / m_dimensions[2] * ptsDesc.spacing[2])};

        float  maxPos[3] = {(float)((m_dimensions[0]-1+m_dimensions[0]/2.0) * ptsDesc.size[0] / m_dimensions[0] * ptsDesc.spacing[0]),
                            (float)((m_dimensions[1]-1+m_dimensions[1]/2.0) * ptsDesc.size[1] / m_dimensions[1] * ptsDesc.spacing[1]),
                            (float)((m_dimensions[2]-1+m_dimensions[2]/2.0) * ptsDesc.size[2] / m_dimensions[2] * ptsDesc.spacing[2])};

        float  maxAxis   = std::max(maxPos[0]-minPos[0], std::max(maxPos[1]-minPos[1], maxPos[2]-minPos[2]));
        float* pts = (float*)malloc(sizeof(float)*nbValues*3);

        for(uint32_t k = 0; k < m_dimensions[2]; k++)
            for(uint32_t j = 0; j < m_dimensions[1]; j++)
                for(uint32_t i = 0; i < m_dimensions[0]; i++)
                {
                    size_t id = 3*(i + j*m_dimensions[0] + k*m_dimensions[0]*m_dimensions[1]);
                    pts[id + 0] = (i-m_dimensions[0]/2.0)*ptsDesc.size[0]/m_dimensions[0]*ptsDesc.spacing[0]/maxAxis;
                    pts[id + 1] = (j-m_dimensions[1]/2.0)*ptsDesc.size[1]/m_dimensions[1]*ptsDesc.spacing[1]/maxAxis;
                    pts[id + 2] = (k-m_dimensions[2]/2.0)*ptsDesc.size[2]/m_dimensions[2]*ptsDesc.spacing[2]/maxAxis;
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

    VTKStructuredGridPointGameObject::VTKStructuredGridPointGameObject(GameObject* parent, GLRenderer* renderer, Material* mtl, VTKStructuredGridPointVBO* gridPointVBO, uint32_t propID, const VTKFieldValue* ptFieldValue, SubDataset* subDataset) : SciVis(parent, renderer, mtl, subDataset), m_gridPointVBO(gridPointVBO), m_maxVal(std::numeric_limits<float>::min()), m_minVal(std::numeric_limits<float>::max()), m_propID(propID)
    {
        const VTKStructuredPoints& ptsDesc = m_gridPointVBO->m_vtkParser->getStructuredPointsDescriptor();

        //Read and determine the max / min values
        uint8_t* vals = (uint8_t*)m_gridPointVBO->m_vtkParser->parseAllFieldValues(ptFieldValue);
        for(uint32_t i = 0; i < ptFieldValue->nbTuples; i++)
        {
            m_maxVal = std::max(m_maxVal, uint8ToFloat(vals + i*ptFieldValue->nbValuePerTuple));
            m_minVal = std::min(m_minVal, uint8ToFloat(vals + i*ptFieldValue->nbValuePerTuple));
        }

        //Store the interesting values
        size_t nbValues  = m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1]*m_gridPointVBO->m_dimensions[2];
        m_vals = (float*)malloc(sizeof(float)*4*nbValues);
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
                    m_vals[destID] = uint8ToFloat(vals + ptFieldValue->nbValuePerTuple*srcID);
                }
        free(vals);
        setColorRange(m_model->getMinClamping(), m_model->getMaxClamping(), m_model->getColorMode());
    }

    VTKStructuredGridPointGameObject::~VTKStructuredGridPointGameObject()
    {
        glDeleteVertexArraysOES(1, &m_vaoID);
        free(m_vals);
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
            if(t < max || t > min)
                for(uint32_t j = 0; j < 4; j++)
                    colors[i*4+j] = 0;
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
}
