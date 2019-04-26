#version 300 es

precision highp float;

uniform vec4 uCameraParams;

uniform sampler3D uTexture0;
uniform vec3      uDimension;

in vec3 varyBegRayOrigin;
in vec2 varyPosition;
in vec3 varyNormal;
in vec4 varyEndRayOrigin;

out vec4 fragColor;

/** \brief  Compute the intersection between a ray and a plane
 * \param rayOrigin the ray origin
 * \param planeNormal the plane normal
 * \param planePosition the plane position 
 * \param t[out] the parameter t of the ray equation
 * \return   true if intersection, false otherwise */
bool computeRayPlaneIntersection(in vec3 rayOrigin, in vec3 rayNormal, in vec3 planeNormal, in vec3 planePosition, out float t)
{
    float nDir = dot(planeNormal, rayNormal);
    //if(nDir == 0.0)
    //    return false;

    t = dot(planeNormal, planePosition-rayOrigin)/nDir;
    return t > 0.0;
}

/** \brief  Compute the ray-cube intersection
 *
 * \param rayOrigin the ray origin
 * \param t[6] the t values (pos = rayOrigin +t*rayNormal)
 * \param tValidity[6] the t validity (is t[i] a valid value?) */
void computeRayCubeIntersection(in vec3 rayOrigin, in vec3 rayNormal, out float t[6], out bool tValidity[6])
{
    //Left
    tValidity[0] = computeRayPlaneIntersection(rayOrigin, rayNormal, vec3(-1,  0,  0),
                                               vec3(-0.5, -0.5, -0.5), t[0]);
    //Right
    tValidity[1] = computeRayPlaneIntersection(rayOrigin, rayNormal, vec3( 1,  0,  0),
                                               vec3(0.5, -0.5, -0.5), t[1]);
    //Top
    tValidity[2] = computeRayPlaneIntersection(rayOrigin, rayNormal, vec3( 0,  1,  0),
                                               vec3(-0.5, 0.5, -0.5), t[2]);
    //Bottom
    tValidity[3] = computeRayPlaneIntersection(rayOrigin, rayNormal, vec3( 0, -1,  0),
                                               vec3(-0.5, -0.5, -0.5), t[3]);
    //Front
    tValidity[4] = computeRayPlaneIntersection(rayOrigin, rayNormal, vec3( 0,  0, -1),
                                               vec3(-0.5, -0.5, -0.5), t[4]);
    //Back
    tValidity[5] = computeRayPlaneIntersection(rayOrigin, rayNormal, vec3( 0,  0,  1),
                                               vec3(-0.5, -0.5, 0.5), t[5]);

    //Test the limits
    for(int i = 0; i < 2; i++)
    {
        //Left / Right
        if(tValidity[i])
        {
            vec3 p = t[i]*rayNormal + rayOrigin;
            if(p.y < -0.5 || p.y > 0.5 ||
               p.z < -0.5 || p.z > 0.5)
                tValidity[i] = false;
        }

        //Top / Bottom
        if(tValidity[i+2])
        {
            vec3 p = t[i+2]*rayNormal + rayOrigin;
            if(p.x < -0.5 || p.x > 0.5 ||
               p.z < -0.5 || p.z > 0.5)
                tValidity[i+2] = false;
        }

        //Front / Back
        if(tValidity[i+4])
        {
            vec3 p = t[i+4]*rayNormal + rayOrigin;
            if(p.x < -0.5 || p.x > 0.5 ||
               p.y < -0.5 || p.y > 0.5)
                tValidity[i+4] = false;
        }
    }
}

void main()
{
    fragColor = vec4(0, 0, 0, 0);

    //Compute starting point + normal
    vec3 rayOrigin = varyBegRayOrigin;
    vec3 rayNormal;

    if(uCameraParams.w == 0.0) //Perspective mode
        rayNormal = normalize(varyEndRayOrigin.xyz/varyEndRayOrigin.w - rayOrigin.xyz);
    else //Orthographic mode, normal already computed
        rayNormal = varyNormal;

    //Compute ray - cube intersections
    float t[6];
    bool  tValidity[6];
    computeRayCubeIntersection(rayOrigin, rayNormal, t, tValidity);

    //Determine if the ray touched the cube or not
    int startValidity = 0;
    for(; !tValidity[startValidity] && startValidity < 6; startValidity++);

    if(startValidity == 6)
        return;

    //If yes, look at the starting and end points
    float minT = t[startValidity];
    float maxT = minT;

    for(int i = startValidity+1; i < 6; i++)
    {
        if(tValidity[i])
        {
            minT = min(minT, t[i]);
            maxT = max(maxT, t[i]);
            break;
        }
    }

    //Test if only one intersection (i.e, camera in the middle of the dataset)
    if(minT == maxT)
        minT = 0.0;

    //compute step and maximum number of steps
    vec3 minPos = rayOrigin.xyz + (minT)*rayNormal + vec3(0.5, 0.5, 0.5);
    vec3 maxPos = rayOrigin.xyz + (maxT)*rayNormal + vec3(0.5, 0.5, 0.5);

    const float rayStep  = 0.50;
    float nbValues = 1.0 + length((maxPos-minPos)*uDimension)/rayStep;

    vec3 rayStepNormal = rayStep*rayNormal/uDimension;
    vec3 rayPos = minPos;

    //Ray marching algorithm
    while(nbValues >= 0.0)
    {
        nbValues -= 1.0;
        rayPos += rayStepNormal;
        vec4 tfColor = textureLod(uTexture0, rayPos,  0.0);
        //vec4 tfColor = textureLod(uTexture1, tfCoord, 0.0);
        //vec4 tfColor = tfCoord.xxxy;
        tfColor.a *= rayStep;
        vec4 col  = vec4(tfColor.xyz, 1.0);
        fragColor = fragColor + (1.0 - fragColor.a)*tfColor.a*col;

        if(fragColor.a >= 0.95)
        {
            fragColor.a = 1.0;
            return;
        }
    }

    //At t=maxT
    /*vec2 tfCoord = textureLod(uTexture0, rayOrigin.xyz + maxT*rayNormal + vec3(0.5, 0.5, 0.5), 2.0).rg;
    vec4 tfColor = textureLod(uTexture1, tfCoord, 0.0);

    fragColor.xyz = fragColor.xyz + (1.0 - fragColor.a)*tfColor.a*tfColor.xyz;
    fragColor.a = fragColor.a + tfColor.a*(1.0 - fragColor.a);*/
}
