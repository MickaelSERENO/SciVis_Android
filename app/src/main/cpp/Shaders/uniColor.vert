#version 300 es
precision mediump float;

in vec3 vPosition;
in vec2 vUV0;

out vec2 varyUV;
uniform mat4 uMVP;

void main()
{
	varyUV      = vUV0;
	gl_Position = uMVP*vec4(vPosition, 1.0);
}
