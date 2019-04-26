#ifndef  VOLUMERENDERINGPLANEALGORITHM_INC
#define  VOLUMERENDERINGPLANEALGORITHM_INC

#define GL_GLEXT_PROTOTYPES

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 

namespace sereno
{
    /* \brief  Compute the variables needed for plane - cube intersections
     *
     * \param mv the model-view matrix
     * \param planeNormal[out] the plane normal in the model space*/
    inline void planeCubeComputeVariables(const glm::mat4& mv, glm::vec3& planeNormal)
    {
        //Compute plane normal in the model space.
        //total computation : transpose(inverse(inverse(uMVP)))*normal).
        planeNormal = glm::normalize(glm::vec3(glm::vec4(0.0, 0.0, 1.0, 1.0)*mv));
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
    inline bool vectorPlaneIntersection(const glm::vec3& dirOrigin,   const glm::vec3& dir,
                                        const glm::vec3& planeNormal, const glm::vec3& planePosition, float& t)
    {
        float nDir = glm::dot(planeNormal, dir);
        if(nDir == 0.0f)
            return false;
        t = glm::dot(planeNormal, planePosition-dirOrigin)/nDir;

        return t >= 0.0f && t <= 1.0f;
    }

    /**
     * \brief Compute the intersection between a plane and a cube
     * \param planeNormal the plane normal in the model space
     * \param planePosition the plane origin in the model space
     * \param outPoints[out] the intersections points
     * \return the number of points*/
    inline uint8_t planeCubeIntersection(const glm::vec3& planeNormal, const glm::vec3& planePosition, glm::vec3* outPoints)
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

        glm::vec3 cubePoints[8];
        cubePoints[0] = glm::vec3(-0.5f, -0.5f, -0.5f);
        cubePoints[1] = glm::vec3( 0.5f, -0.5f, -0.5f);
        cubePoints[2] = glm::vec3(-0.5f,  0.5f, -0.5f);
        cubePoints[3] = glm::vec3(-0.5f, -0.5f,  0.5f);
        cubePoints[4] = glm::vec3( 0.5f,  0.5f, -0.5f);
        cubePoints[5] = glm::vec3( 0.5f, -0.5f,  0.5f);
        cubePoints[6] = glm::vec3(-0.5f,  0.5f,  0.5f);
        cubePoints[7] = glm::vec3( 0.5f,  0.5f,  0.5f);

        //Edges along x
        if(vectorPlaneIntersection(cubePoints[0], edgeVecs[0], planeNormal, planePosition, t))
            outPoints[nbPoints++] = cubePoints[0] + edgeVecs[0] * t;
        if(vectorPlaneIntersection(cubePoints[2], edgeVecs[0], planeNormal, planePosition, t))
            outPoints[nbPoints++] = cubePoints[2] + edgeVecs[0] * t;
        if(vectorPlaneIntersection(cubePoints[3], edgeVecs[0], planeNormal, planePosition, t))
            outPoints[nbPoints++] = cubePoints[3] + edgeVecs[0] * t;
        if(vectorPlaneIntersection(cubePoints[6], edgeVecs[0], planeNormal, planePosition, t))
            outPoints[nbPoints++] = cubePoints[6] + edgeVecs[0] * t;

        //Edges along y
        if(vectorPlaneIntersection(cubePoints[0], edgeVecs[1], planeNormal, planePosition, t))
            outPoints[nbPoints++] = cubePoints[0] + edgeVecs[1] * t;
        if(vectorPlaneIntersection(cubePoints[1], edgeVecs[1], planeNormal, planePosition, t))
            outPoints[nbPoints++] = cubePoints[1] + edgeVecs[1] * t;
        if(vectorPlaneIntersection(cubePoints[3], edgeVecs[1], planeNormal, planePosition, t))
            outPoints[nbPoints++] = cubePoints[3] + edgeVecs[1] * t;
        if(vectorPlaneIntersection(cubePoints[5], edgeVecs[1], planeNormal, planePosition, t))
            outPoints[nbPoints++] = cubePoints[5] + edgeVecs[1] * t;

        //Edges along z
        if(vectorPlaneIntersection(cubePoints[0], edgeVecs[2], planeNormal, planePosition, t))
            outPoints[nbPoints++] = cubePoints[0] + edgeVecs[2] * t;
        if(vectorPlaneIntersection(cubePoints[1], edgeVecs[2], planeNormal, planePosition, t))
            outPoints[nbPoints++] = cubePoints[1] + edgeVecs[2] * t;
        if(vectorPlaneIntersection(cubePoints[2], edgeVecs[2], planeNormal, planePosition, t))
            outPoints[nbPoints++] = cubePoints[2] + edgeVecs[2] * t;
        if(vectorPlaneIntersection(cubePoints[4], edgeVecs[2], planeNormal, planePosition, t))
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
     * \brief  Compute all the planes cutting a cube
     * Planes are computed front to back, with planes always normal to the camera center (suited for orthographic projection)
     *
     * The max number of planes (maxNbPlanes) is sqrt(3)/dimPerPlane
     *
     * \param mv the Model View matrix. The plane normal will be orthogonal of the camera
     * \param dimPerPlane the space between planes
     * \param nbPointsPerPlane[out] array containing how many points the plane[i] contains. Size: maxNbPlanes*sizeof(uint8_t)
     * \param planePoints[out] array containing the points data. Size: maxNbPlanes*sizeof(float)*8*3. If nbPointsPerPlane == 3, planePoints contain a triangle, else if nbPointsPerPlane > 3, planePoints contain triangles fan (2+nbPointsPerPlane points).
     * \param nbPlanes[out] the number of planes computed
     * \param nbData[out] the size of planePoints written.
     */
    inline void computePlaneMarching(const glm::mat4& mv, float dimPerPlane, uint8_t* nbPointsPerPlane, float* planePoints, uint32_t& nbPlanes, uint32_t& nbData)
    {
        /*----------------------------------------------------------------------------*/
        /*--------------------------Determine planes points---------------------------*/
        /*----------------------------------------------------------------------------*/

        //Compute plane-cube variables
        glm::vec3 planeNormal;
        planeCubeComputeVariables(mv, planeNormal);

        LOG_INFO("plane normal : %f %f %f", planeNormal.x, planeNormal.y, planeNormal.z);

        //Determine closest point to the camera
        glm::vec4 points[8];
        points[0] = glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
        points[1] = glm::vec4( 0.5f, -0.5f, -0.5f, 1.0f);
        points[2] = glm::vec4(-0.5f,  0.5f, -0.5f, 1.0f);
        points[3] = glm::vec4(-0.5f, -0.5f,  0.5f, 1.0f);
        points[4] = glm::vec4( 0.5f,  0.5f, -0.5f, 1.0f);
        points[5] = glm::vec4( 0.5f, -0.5f,  0.5f, 1.0f);
        points[6] = glm::vec4(-0.5f,  0.5f,  0.5f, 1.0f);
        points[7] = glm::vec4( 0.5f,  0.5f,  0.5f, 1.0f);

        uint8_t minZ = 0;
        uint8_t maxZ = 0;
        glm::vec4 pMin = mv*points[0];
        pMin / pMin.w;
        glm::vec4 pMax = pMin;

        for(uint8_t i = 1; i < 8; i++)
        {
            glm::vec4 p = mv*points[i];
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
        uint32_t planePointsOffset  = 0;
        uint32_t i = 0;

        glm::vec3 planeOrigin = glm::vec3(points[maxZ]); //Start front to finish in back

        for(; glm::dot(planeOrigin - glm::vec3(points[minZ]), planeNormal) >= 0; i++, planeOrigin -= dimPerPlane*planeNormal)
//        for(; glm::dot(glm::vec3(points[maxZ]) - planeOrigin, planeNormal) >= 0; i++, planeOrigin += dimPerPlane*planeNormal)
        {
            glm::vec3 polygon[8];
            nbPointsPerPlane[i] = planeCubeIntersection(planeNormal, planeOrigin, polygon);

            //Triangle
            if(nbPointsPerPlane[i] == 3)
            {
                for(uint8_t j = 0; j < 3; j++)
                    for(uint8_t k = 0; k < 3; k++)
                        planePoints[planePointsOffset+3*j+k] = polygon[j][k];
                planePointsOffset += 3*3;
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
                    planePoints[planePointsOffset+k] = center[k]/nbPointsPerPlane[i];

                //Add the points
                for(uint8_t j = 0; j < nbPointsPerPlane[i]; j++)
                    for(uint8_t k = 0; k < 3; k++)
                        planePoints[planePointsOffset+3*(j+1)+k] = polygon[j][k];

                //Re add the first point
                for(uint8_t k = 0; k < 3; k++)
                    planePoints[planePointsOffset+3*(nbPointsPerPlane[i]+1)+k] = polygon[0][k];

                planePointsOffset += 3*(nbPointsPerPlane[i]+2);
            }
        }
        nbData   = planePointsOffset;
        nbPlanes = i;
    }
}

#endif
