#version 300 es

precision highp float;

uniform mat4 uMVP;

in      vec3 vPosition;
out     vec3 varyUV;

void main()
{
	gl_Position = uMVP*vec4(vPosition, 1.0);
    varyUV      = vPosition;
}
