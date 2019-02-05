#version 300 es
precision mediump float;

in vec3 vPosition;
in vec4 vColor;
in vec2 vTextureCoord;

uniform mat4 uMVP;

out vec4 varyColor;
out vec2 varyTextureCoord;

void main()
{
	varyColor         = vColor;
	varyTextureCoord  = vTextureCoord;

	gl_Position       = uMVP*vec4(vPosition, 1.0);
}
