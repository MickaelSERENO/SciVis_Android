#version 300 es

precision highp float;

uniform mat4 uMVP;
uniform mat4 uInvMVP;
uniform mat4 uInvMV;
uniform vec4 uCameraParams;

in  vec2 vPosition;

out vec3 varyBegRayOrigin;
out vec2 varyPosition;
out vec3 varyNormal;
out vec4 varyEndRayOrigin;

void main()
{
	gl_Position      = vec4(vPosition, -1.0, 1.0);
    varyPosition     = vPosition;
    varyNormal       = vec3(0.0, 0.0, 1.0);
    varyBegRayOrigin = vec3(0.0, 0.0, 0.0);
    varyEndRayOrigin = vec4(0.0, 0.0, 0.0, 1.0);

    if(uCameraParams.w == 0.0) //Perspective mode
    {
        vec4 begRayOrigin = uInvMV * vec4(0, 0, 0.0, 1.0);
        varyBegRayOrigin  = begRayOrigin.xyz / begRayOrigin.w;
        varyEndRayOrigin  = uInvMVP * vec4(varyPosition, 1.0, 1.0);
    }
    else
    {
        varyNormal = normalize((vec4(0, 0, 1, 1)*uMVP).xyz);
        vec4 begRayOrigin = uInvMVP * vec4(vPosition, -1.0, 1.0);
        varyBegRayOrigin  = begRayOrigin.xyz / begRayOrigin.w;
    }
}
