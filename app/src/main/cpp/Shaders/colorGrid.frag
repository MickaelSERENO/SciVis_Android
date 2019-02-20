#version 300 es

precision mediump float;

uniform sampler3D uTexture0;
uniform sampler2D uTexture1;
uniform vec3      uDimension;

in vec3 varyRayNormal;
in vec4 varyRayOrigin;

out vec4 fragColor;

/** \brief  Compute the intersection between a ray and a plane
 * \param rayOrigin the ray origin
 * \param planeNormal the plane normal
 * \param planePosition the plane position 
 * \param t[out] the parameter t of the ray equation
 * \return   true if intersection, false otherwise */
bool computeRayPlaneIntersection(in vec3 rayOrigin, in vec3 planeNormal, in vec3 planePosition, out float t)
{
    float nDir = dot(planeNormal, varyRayNormal);
    if(nDir == 0.0)
        return false;

    t = dot(planeNormal, planePosition-rayOrigin)/nDir;
    return true;
}

/** \brief  Compute the ray-cube intersection
 *
 * \param rayOrigin the ray origin
 * \param t[6] the t values (pos = rayOrigin +t*varyRayNormal)
 * \param tValidity[6] the t validity (is t[i] a valid value?) */
void computeRayCubeIntersection(in vec3 rayOrigin, out float t[6], out bool tValidity[6])
{
    //Left
    tValidity[0] = computeRayPlaneIntersection(rayOrigin, vec3(-1,  0,  0),
                                               vec3(0.0, 0.0, 0.0), t[0]);
    //Right
    tValidity[1] = computeRayPlaneIntersection(rayOrigin, vec3( 1,  0,  0),
                                               vec3(1.0, 0.0, 0.0), t[1]);
    //Top
    tValidity[2] = computeRayPlaneIntersection(rayOrigin, vec3( 0,  1,  0),
                                               vec3(0.0, 1.0, 0.0), t[2]);
    //Bottom
    tValidity[3] = computeRayPlaneIntersection(rayOrigin, vec3( 0, -1,  0),
                                               vec3(0.0, 0.0, 0.0), t[3]);
    //Front
    tValidity[4] = computeRayPlaneIntersection(rayOrigin, vec3( 0,  0, -1),
                                               vec3(0.0, 0.0, 0.0), t[4]);
    //Back
    tValidity[5] = computeRayPlaneIntersection(rayOrigin, vec3( 0,  0,  1),
                                               vec3(0.0, 0.0, 1.0), t[5]);

    //Test the limits
    for(int i = 0; i < 2; i++)
    {
        //Left / Right
        if(tValidity[i])
        {
            vec3 p = t[i]*varyRayNormal + rayOrigin;
            if(p.y <= 0.0 || p.y >= 1.0 ||
               p.z <= 0.0 || p.z >= 1.0)
                tValidity[i] = false;
        }

        //Top / Bottom
        if(tValidity[i+2])
        {
            vec3 p = t[i+2]*varyRayNormal + rayOrigin;
            if(p.x <= 0.0 || p.x >= 1.0 ||
               p.z <= 0.0 || p.z >= 1.0)
                tValidity[i+2] = false;
        }

        //Front / Back
        if(tValidity[i+4])
        {
            vec3 p = t[i+4]*varyRayNormal + rayOrigin;
            if(p.x <= 0.0 || p.x >= 1.0 ||
               p.y <= 0.0 || p.y >= 1.0)
                tValidity[i+4] = false;
        }
    }
}

void main()
{
    //Compute ray - cube intersections
    float t[6];
    bool  tValidity[6];
    computeRayCubeIntersection(varyRayOrigin.xyz, t, tValidity);

    //Determine if the ray touched the cube or not
    int startValidity = 0;
    for(; !tValidity[startValidity]; startValidity++);

    if(startValidity == 6)
        discard;

    //If yes, look at the starting and end points
    float minT = t[startValidity];
    float maxT = minT;

    for(int i = startValidity+1; i < 6; i++)
    {
        if(tValidity[i])
        {
            minT = min(minT, t[i]);
            maxT = max(maxT, t[i]);
        }
    }

    //compute step and maximum number of steps
    float rayStep = 1.0/(max(max(uDimension.x, uDimension.y), uDimension.z)*4.0);

    vec3 rayPos = varyRayOrigin.xyz + minT*varyRayNormal;

    //Ray marching algorithm
    fragColor = vec4(0, 0, 0, 0);
    for(; minT < maxT; minT+=rayStep, rayPos += varyRayNormal*rayStep)
    {
        vec2 tfCoord = textureLod(uTexture0, rayPos, 0.0).rg;
        vec4 tfColor = textureLod(uTexture1, tfCoord, 0.0);
        fragColor    = (1.0-fragColor.a)*tfColor + fragColor;
    }

    //At t=maxT
    vec2 tfCoord = textureLod(uTexture0, varyRayOrigin.xyz + maxT*varyRayNormal, 0.0).rg;
    vec4 tfColor = textureLod(uTexture1, tfCoord, 0.0);
    fragColor    = (1.0-fragColor.a)*tfColor + fragColor;
}
