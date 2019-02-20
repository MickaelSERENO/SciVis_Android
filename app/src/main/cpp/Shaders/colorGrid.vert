#version 300 es

precision mediump float;

uniform mat4 uMVP;
uniform mat4 uInvMVP;

in  vec2 vPosition;

out vec3 varyRayNormal;
out vec4 varyRayOrigin;

void main()
{
	gl_Position   = vec4(vPosition, -1.0, 1.0);

    varyRayNormal = normalize(vec3(vec4(0, 0, 1, 1)*uMVP)); //Normal in model space
    varyRayOrigin = uInvMVP*gl_Position;                    //Origin in model space
    varyRayOrigin/= varyRayOrigin.w;
}
