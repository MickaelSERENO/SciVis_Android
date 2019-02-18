#version 300 es

precision lowp float;

in vec3  vPosition;
in float vUV0;

uniform mat4 uMVP;

out float varyUV;

void main()
{
    varyUV = vUV0;
	gl_Position = uMVP*vec4(vPosition, 1.0);
}

