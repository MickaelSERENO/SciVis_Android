#version 300 es
precision mediump float;

in vec3 vPosition;
in vec4 vColor;
in vec2 vUV0;

uniform mat4 uMVP;

out vec4 varyColor;
out vec2 varyUV;

void main()
{
	varyColor = vColor;
	varyUV    = vUV0;

	gl_Position = uMVP*vec4(vPosition, 1.0);
}
