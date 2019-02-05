#version 300 es
precision mediump float;

in vec3 vPosition;
in vec2 vTextureCoord;

out vec2 varyTextureCoord;
uniform mat4 uMVP;

void main()
{
	varyTextureCoord  = vTextureCoord;
	gl_Position       = uMVP*vec4(vPosition, 1.0);
}
