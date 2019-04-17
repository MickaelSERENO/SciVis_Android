#version 300 es

precision mediump float;

uniform mat4 uMVP;
uniform mat4 uInvMVP;
uniform mat4 uProjMat;
uniform mat4 uInvP;
uniform mat4 uInvMV;
uniform vec4 uCameraParams;

in  vec2 vPosition;

out vec3 varyBegRayOrigin;
out vec2 varyPosition;
out vec3 varyNormal;

void main()
{
	gl_Position  = vec4(vPosition, -1.0, 1.0);
    varyPosition = vPosition;

    if(uCameraParams.w == 0.0) //Perspective mode
    {
        vec4 begRayOrigin = uInvMV * vec4(0, 0, 0.0, 1.0);
        varyBegRayOrigin  = begRayOrigin.xyz / begRayOrigin.w;
    }
    else
    {
        varyNormal = normalize((vec4(0, 0, 1, 1)*uMVP).xyz);
        vec4 begRayOrigin = uInvMVP * vec4(vPosition, -1.0, 1.0);
        varyBegRayOrigin  = begRayOrigin.xyz / begRayOrigin.w;
    }
}
