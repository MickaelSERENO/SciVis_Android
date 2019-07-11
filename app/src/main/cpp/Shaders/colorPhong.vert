#version 300 es
precision mediump float;

in vec3 vPosition;
in vec3 vNormal;

out vec3 varyNormal;
out vec4 varyPosition;
out vec3 varyLightDir;

uniform mat4 uMVP;
uniform mat4 uInvMV;
uniform mat4 uCameraMat;
uniform mat4 uObjMat;
uniform vec3 uLightDir;

void main()
{
	gl_Position  = uMVP*vec4(vPosition, 1.0);
    varyNormal   = normalize(vec3(vec4(vNormal,1.0)*uInvMV));
    varyPosition = uCameraMat * uObjMat * vec4(vPosition, 1.0);
    varyPosition /= varyPosition.w;
    varyLightDir = normalize(uLightDir*inverse(mat3(uCameraMat)));
}

