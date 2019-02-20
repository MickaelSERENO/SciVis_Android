#include "Graphics/SciVis/VTKStructuredGridPointGameObject.h"
#include "Graphics/SciVis/SciVisColor.h"
#include <algorithm>
#include <cstdlib>
#include <limits>

namespace sereno
{
    /**
     * \brief  Compute the variables needed for plane - cube intersections
     *
     * \param uMVP the mvp matrix
     * \param cubeDim the cube dimension 
     * \param planeNormal[out] the plane normal in the model space*/
    static void VTKStructuredPoint_planeCubeComputeVariables(const glm::mat4& uMVP, const glm::vec3& cubeDim, glm::vec3& planeNormal)
    {
        //Compute plane normal in the model space.
        //total computation : transpose(inverse(inverse(uMVP)))*normal).
        planeNormal = glm::normalize(glm::vec3(glm::vec4(0.0, 0.0, 1.0, 1.0)*uMVP));
    }

    /** \brief  Compute the intersection between a ray and a plane
     *
     * \param planeNormal the plane normal
     * \param planePosition a plane position
     * \param dir the ray direction
     * \param dirOrigin the origin of the ray
     * \param t[out] the t parameter : pos = dirOrigin + t*dir
     *
     * \return   true if intersection, false otherwise (t untouched) */
    static bool VTKStructuredPoint_vectorPlaneIntersection(const glm::vec3& dirOrigin,   const glm::vec3& dir,
                                                           const glm::vec3& planeNormal, const glm::vec3& planePosition, float& t)
    {
        float nDir = glm::dot(planeNormal, dir);
        if(nDir == 0.0f)
            return false;
        t = glm::dot(planeNormal, planePosition-dirOrigin)/nDir;
        return true;
    }

    /**
     * \brief  Compute the intersection between a plane and a cube
     * \param planeNormal the plane normal in the model space
     * \param planePosition the plane origin in the model space
     * \param cubeDim the cube dimensions
     * \param outPoints[out] the intersections points
     * \return the number of points*/
    static uint8_t VTKStructuredPoint_planeCubeIntersection(const glm::vec3& planeNormal, const glm::vec3& planePosition, const glm::vec3& cubeDim, glm::vec3* outPoints)
    {
        uint8_t nbPoints = 0; //The number of points
        float t;              //the parameter of each cube ray

        /*----------------------------------------------------------------------------*/
        /*--------------------Compute edge - points intersections---------------------*/
        /*----------------------------------------------------------------------------*/

        glm::vec3 edgeVecs[3];
        edgeVecs[0] = glm::vec3(1.0, 0.0, 0.0);
        edgeVecs[1] = glm::vec3(0.0, 1.0, 0.0);
        edgeVecs[2] = glm::vec3(0.0, 0.0, 1.0);

        glm::vec3 cubePoints[7] = {glm::vec3(0.0f, 0.0f, 0.0f),
                                   glm::vec3(1.0f, 0.0f, 0.0f),
                                   glm::vec3(0.0f, 1.0f, 0.0f),
                                   glm::vec3(0.0f, 0.0f, 1.0f),
                                   glm::vec3(1.0f, 1.0f, 0.0f),
                                   glm::vec3(1.0f, 0.0f, 1.0f),
                                   glm::vec3(0.0f, 1.0f, 1.0f)};

        //Edges along x
        if(VTKStructuredPoint_vectorPlaneIntersection(cubePoints[0], edgeVecs[0], planeNormal, planePosition, t) && t >= 0.f && t <= 1.f)
            outPoints[nbPoints++] = cubePoints[0] + edgeVecs[0] * t;
        if(VTKStructuredPoint_vectorPlaneIntersection(cubePoints[2], edgeVecs[0], planeNormal, planePosition, t) && t >= 0.f && t <= 1.f)
            outPoints[nbPoints++] = cubePoints[2] + edgeVecs[0] * t;
        if(VTKStructuredPoint_vectorPlaneIntersection(cubePoints[3], edgeVecs[0], planeNormal, planePosition, t) && t >= 0.f && t <= 1.f)
            outPoints[nbPoints++] = cubePoints[3] + edgeVecs[0] * t;
        if(VTKStructuredPoint_vectorPlaneIntersection(cubePoints[6], edgeVecs[0], planeNormal, planePosition, t) && t >= 0.f && t <= 1.f)
            outPoints[nbPoints++] = cubePoints[6] + edgeVecs[0] * t;

        //Edges along y
        if(VTKStructuredPoint_vectorPlaneIntersection(cubePoints[0], edgeVecs[1], planeNormal, planePosition, t) && t >= 0.f && t <= 1.f)
            outPoints[nbPoints++] = cubePoints[0] + edgeVecs[1] * t;
        if(VTKStructuredPoint_vectorPlaneIntersection(cubePoints[1], edgeVecs[1], planeNormal, planePosition, t) && t >= 0.f && t <= 1.f)
            outPoints[nbPoints++] = cubePoints[1] + edgeVecs[1] * t;
        if(VTKStructuredPoint_vectorPlaneIntersection(cubePoints[3], edgeVecs[1], planeNormal, planePosition, t) && t >= 0.f && t <= 1.f)
            outPoints[nbPoints++] = cubePoints[3] + edgeVecs[1] * t;
        if(VTKStructuredPoint_vectorPlaneIntersection(cubePoints[5], edgeVecs[1], planeNormal, planePosition, t) && t >= 0.f && t <= 1.f)
            outPoints[nbPoints++] = cubePoints[5] + edgeVecs[1] * t;

        //Edges along z
        if(VTKStructuredPoint_vectorPlaneIntersection(cubePoints[0], edgeVecs[2], planeNormal, planePosition, t) && t >= 0.f && t <= 1.f)
            outPoints[nbPoints++] = cubePoints[0] + edgeVecs[2] * t;
        if(VTKStructuredPoint_vectorPlaneIntersection(cubePoints[1], edgeVecs[2], planeNormal, planePosition, t) && t >= 0.f && t <= 1.f)
            outPoints[nbPoints++] = cubePoints[1] + edgeVecs[2] * t;
        if(VTKStructuredPoint_vectorPlaneIntersection(cubePoints[2], edgeVecs[2], planeNormal, planePosition, t) && t >= 0.f && t <= 1.f)
            outPoints[nbPoints++] = cubePoints[2] + edgeVecs[2] * t;
        if(VTKStructuredPoint_vectorPlaneIntersection(cubePoints[4], edgeVecs[2], planeNormal, planePosition, t) && t >= 0.f && t <= 1.f)
            outPoints[nbPoints++] = cubePoints[4] + edgeVecs[2] * t;

        /*----------------------------------------------------------------------------*/
        /*------------------------------Sort the points-------------------------------*/
        /*----------------------------------------------------------------------------*/

        //The sorting is based on cross products. Since we are in a convex hull polygon, we considered the first intersection as the "center" of the polygon
        if(nbPoints < 3) 
            return nbPoints;

        glm::vec3 origin = outPoints[0];

        std::sort(outPoints, outPoints + nbPoints, [&](const glm::vec3 &a, const glm::vec3 &b) -> bool 
        {
            glm::vec3 v = glm::cross(a-origin, b-origin);
            return glm::dot(v, planeNormal) > 0;
        });

        return nbPoints;
    }

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

        //Creates the VBO
        //It must be high enough to contain N planes with 6 edges looking for the largest diagonal
        m_nbPlanes = 2.0f/DIM_PER_PLANE+1;

        //7 points per plane maximum (center counted)
        glGenBuffers(1, &m_vboID);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*8*m_nbPlanes, NULL, GL_DYNAMIC_DRAW);
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
            glVertexAttribPointer(MATERIAL_VPOSITION, 3, GL_FLOAT, 0, 0, (void*)(0));
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
        /*--------------------------Determine planes points---------------------------*/
        /*----------------------------------------------------------------------------*/

        //Compute plane-cube variables
        glm::vec3 planeNormal;
        glm::vec3 cubeDim = glm::vec3(m_gridPointVBO->m_dimensions[0], m_gridPointVBO->m_dimensions[1], m_gridPointVBO->m_dimensions[2]) *
                            glm::vec3(m_gridPointVBO->m_spacing[0], m_gridPointVBO->m_spacing[1], m_gridPointVBO->m_spacing[2]);
        VTKStructuredPoint_planeCubeComputeVariables(mvp, cubeDim, planeNormal);

        //Determine closest point to the camera
        glm::vec4 points[8];
        points[0] = glm::vec4(0.f, 0.f, 0.f, 1.0f);
        points[1] = glm::vec4(1.0, 0.f, 0.f, 1.0f);
        points[2] = glm::vec4(0.f, 1.0, 0.f, 1.0f);
        points[3] = glm::vec4(0.f, 0.f, 1.0, 1.0f);
        points[4] = glm::vec4(1.0, 1.0, 0.f, 1.0f);
        points[5] = glm::vec4(1.0, 0.f, 1.0, 1.0f);
        points[6] = glm::vec4(0.f, 1.0, 1.0, 1.0f);
        points[7] = glm::vec4(1.0, 1.0, 1.0, 1.0f);

        uint8_t minZ = 0;
        uint8_t maxZ = 0;
        glm::vec4 pMin = mvp*points[0];
        pMin / pMin.w;
        glm::vec4 pMax = pMin;

        for(uint8_t i = 1; i < 8; i++)
        {
            glm::vec4 p = mvp*points[i];
            p /= p.w;
            if(p.z < pMin.z)
            {
                minZ = i;
                pMin = p;
            }
            else if(p.z > pMax.z)
            {
                maxZ = i;
                pMax = p;
            }
        }

        //Determine all the planes
        float*   planeData        = (float*)malloc(sizeof(float)*8*3*m_gridPointVBO->m_nbPlanes);
        uint8_t* nbPointsPerPlane = (uint8_t*)malloc(sizeof(uint8_t)*m_gridPointVBO->m_nbPlanes);
        uint32_t nbPlanes         = 0;
        uint32_t planeDataOffset  = 0;
        uint32_t i = 0;

        glm::vec3 planeOrigin = glm::vec3(points[minZ])-DIM_PER_PLANE*planeNormal; //Start back to finish in front

        for(; i < m_gridPointVBO->m_nbPlanes && glm::dot(planeOrigin - glm::vec3(points[maxZ]), planeNormal) <= 0; i++, planeOrigin += DIM_PER_PLANE*planeNormal)
        {
            glm::vec3 polygon[8];
            nbPointsPerPlane[i] = VTKStructuredPoint_planeCubeIntersection(planeNormal, planeOrigin, cubeDim, polygon);

            //Triangle
            if(nbPointsPerPlane[i] == 3)
            {
                for(uint8_t j = 0; j < 3; j++)
                    for(uint8_t k = 0; k < 3; k++)
                        planeData[planeDataOffset+3*j+k] = polygon[j][k];
                planeDataOffset += 3*3;
            }

            //Polygon (GL_TRIANGLE_FAN)
            else if(nbPointsPerPlane[i] > 3)
            {
                //Compute center
                glm::vec3 center(0.0, 0.0, 0.0);
                for(uint8_t j = 0; j < nbPointsPerPlane[i]; j++)
                    for(uint8_t k = 0; k < 3; k++)
                        center[k] += polygon[j][k];

                //Add center
                for(uint8_t k = 0; k < 3; k++)
                    planeData[planeDataOffset+k] = center[k]/nbPointsPerPlane[i];

                //Add the points
                for(uint8_t j = 0; j < nbPointsPerPlane[i]; j++)
                    for(uint8_t k = 0; k < 3; k++)
                        planeData[planeDataOffset+3*(j+1)+k] = polygon[j][k];

                //Re add the first point
                for(uint8_t k = 0; k < 3; k++)
                    planeData[planeDataOffset+3*(nbPointsPerPlane[i]+1)+k] = polygon[0][k];

                planeDataOffset += 3*(nbPointsPerPlane[i]+2);
            }
        }
        nbPlanes = i;
        LOG_ERROR("nbPlanes %u\n", nbPlanes);

        /*----------------------------------------------------------------------------*/
        /*---------------------------------Update VBO---------------------------------*/
        /*----------------------------------------------------------------------------*/

        glBindBuffer(GL_ARRAY_BUFFER, m_gridPointVBO->m_vboID);
            glBufferSubData(GL_ARRAY_BUFFER, 0, planeDataOffset*sizeof(float), planeData);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        /*----------------------------------------------------------------------------*/
        /*------------------------------------Draw------------------------------------*/
        /*----------------------------------------------------------------------------*/

        glm::mat4 invMVP = glm::inverse(mvp);
        m_mtl->bindMaterial(mat, cameraMat, mvp, invMVP);
        m_mtl->bindTexture(m_texture,   3, 0);
        m_mtl->bindTexture(m_tfTexture, 2, 1);

        glDisable(GL_CULL_FACE);
        glDepthMask(false);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindVertexArray(m_vaoID);
        {
            planeDataOffset = 0;
            for(uint32_t i = 0; i < nbPlanes; i++)
            {
                if(nbPointsPerPlane[i] == 3)
                {
                    glDrawArrays(GL_TRIANGLES, planeDataOffset, 3);
                    planeDataOffset += 3;
                }
                else if(nbPointsPerPlane[i] > 3)
                {
                    glDrawArrays(GL_TRIANGLE_FAN, planeDataOffset, nbPointsPerPlane[i]+2);
                    planeDataOffset += nbPointsPerPlane[i]+2;
                }
            }
        }
        glBindVertexArray(0);
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glDepthMask(true);
    }

    void VTKStructuredGridPointGameObject::setColorRange(float min, float max, ColorMode colorMode)
    {
        size_t nbValues  = m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1]*m_gridPointVBO->m_dimensions[2];

        //Update the values of the 3D texture
        //Store scalar and gradient magnitude values
        float* vals = (float*)malloc(sizeof(float)*nbValues*2);

        //Compute scalar
        for(uint32_t i = 0; i < nbValues; i++)
        {
            float t = (m_vals[i]-m_minVal)/(m_maxVal-m_minVal);

            //Test if inside the min-max range.
            if(t > max || t < min)
                vals[2*i] = -1.0f;
            else
                vals[2*i] = t;
        }

        /*----------------------------------------------------------------------------*/
        /*--------------------------Compute gradient values---------------------------*/
        /*----------------------------------------------------------------------------*/

        //Central difference used

        float maxGrad = 0.0f;

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

        //Normalize the gradient
        for(uint32_t k = 1; k < m_gridPointVBO->m_dimensions[2]-1; k++)
            for(uint32_t j = 1; j < m_gridPointVBO->m_dimensions[1]-1; j++)
                for(uint32_t i = 1; i < m_gridPointVBO->m_dimensions[0]-1; i++)
                {
                    uint32_t ind = i + j*m_gridPointVBO->m_dimensions[0] + 
                                   k*m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1];
                    vals[2*ind+1] /= maxGrad;
                }

        /*----------------------------------------------------------------------------*/
        /*---------------Compute gradient values for Edge (grad = 0.0f)---------------*/
        /*----------------------------------------------------------------------------*/

        //for k = 0 and k = max
        for(uint32_t j = 0; j < m_gridPointVBO->m_dimensions[1]; j++)
            for(uint32_t i = 0; i < m_gridPointVBO->m_dimensions[0]; i++)
            {
                uint32_t offset = (m_gridPointVBO->m_dimensions[2]-1)*m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1];
                vals[2*(i+j*m_gridPointVBO->m_dimensions[0])+1]        = 0.0f;
                vals[2*(i+j*m_gridPointVBO->m_dimensions[0]+offset)+1] = 0.0f;
            }

        //for j = 0 and j = max
        for(uint32_t k = 0; k < m_gridPointVBO->m_dimensions[2]; k++)
            for(uint32_t i = 0; i < m_gridPointVBO->m_dimensions[0]; i++)
            {
                uint32_t offset = (m_gridPointVBO->m_dimensions[1]-1)*m_gridPointVBO->m_dimensions[0];
                vals[2*(i+k*m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1])+1]        = 0.0f;
                vals[2*(i+k*m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1]+offset)+1] = 0.0f;
            }

        //for i = 0 and i = max
        for(uint32_t k = 0; k < m_gridPointVBO->m_dimensions[2]; k++)
            for(uint32_t j = 0; j < m_gridPointVBO->m_dimensions[1]; j++)
            {
                uint32_t offset = m_gridPointVBO->m_dimensions[0]-1;
                vals[2*(j*m_gridPointVBO->m_dimensions[0]+
                        k*m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1])+1]        = 0.0f;
                vals[2*(j*m_gridPointVBO->m_dimensions[0]+
                        k*m_gridPointVBO->m_dimensions[0]*m_gridPointVBO->m_dimensions[1]+offset)+1] = 0.0f;
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
